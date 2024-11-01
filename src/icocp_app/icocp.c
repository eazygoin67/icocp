#include "icocp.h"

#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize2.h>

typedef struct loaded_file_t
{
    uint8_t* data;
    size_t   length;

} loaded_file_t;

static loaded_file_t common_file_load_into_memory(
    const char* filename, 
    bool add_null_terminator)
{
    loaded_file_t result = { 0 };

    FILE* fp = (filename && *filename) ? fopen(filename, "rb") : NULL;
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        const size_t file_length = (size_t)_ftelli64(fp);
        fseek(fp, 0, SEEK_SET);

        const size_t memory_buffer_length = file_length + ((add_null_terminator) ? 1 : 0);
        uint8_t* data = (memory_buffer_length) ? malloc(memory_buffer_length) : NULL;

        if (data)
        {
            const size_t bytes_read = fread(data, 1, file_length, fp);
            if (bytes_read == file_length)
            {
                if (add_null_terminator)
                {
                    data[file_length] = 0;
                }

                result.data = data;
                result.length = memory_buffer_length;
            }
            else
            {
                free(data);
            }
        }
    }

    return result;
}

static void common_file_free(loaded_file_t* file)
{
    if (file && file->data)
    {
        free(file->data);
        memset(file, 0, sizeof(*file));
    }
}

__forceinline static bool common_file_is_loaded(loaded_file_t* file)
{
    return (file && file->data && file->length);
}

typedef struct icocp_icon_body_data_t
{
    uint8_t* png_data[ICOCP_MAX_ICON_SUBIMAGES];
    int32_t  png_data_size[ICOCP_MAX_ICON_SUBIMAGES];
    int32_t  png_data_offset[ICOCP_MAX_ICON_SUBIMAGES];
    int32_t  png_data_count;

} icocp_icon_body_data_t;
static void icocp_write_png_data_callback(void* userdata, void* data, int size);

static const int32_t k_icon_target_sizes[ICOCP_MAX_ICON_SUBIMAGES] = { 16, 32, 48, 64, 96, 128, 256 };

__forceinline static bool common_file_write_i8(FILE*  file,  int8_t val)  { return (fwrite(&val, sizeof(val), 1, file) == sizeof(val)); }
__forceinline static bool common_file_write_i16(FILE* file,  int16_t val) { return (fwrite(&val, sizeof(val), 1, file) == sizeof(val)); }
__forceinline static bool common_file_write_i32(FILE* file,  int32_t val) { return (fwrite(&val, sizeof(val), 1, file) == sizeof(val)); }
__forceinline static bool common_file_write_i64(FILE* file,  int64_t val) { return (fwrite(&val, sizeof(val), 1, file) == sizeof(val)); }
__forceinline static bool common_file_write_u8(FILE*  file, uint8_t val)  { return (fwrite(&val, sizeof(val), 1, file) == sizeof(val)); }
__forceinline static bool common_file_write_u16(FILE* file, uint16_t val) { return (fwrite(&val, sizeof(val), 1, file) == sizeof(val)); }
__forceinline static bool common_file_write_u32(FILE* file, uint32_t val) { return (fwrite(&val, sizeof(val), 1, file) == sizeof(val)); }
__forceinline static bool common_file_write_u64(FILE* file, uint64_t val) { return (fwrite(&val, sizeof(val), 1, file) == sizeof(val)); }

__forceinline static stbir_filter icocp_get_stbir_filter_from_icocp_filter(icocp_filtering_t filtering)
{
    switch (filtering)
    {
        case icocp_filtering_point: return STBIR_FILTER_POINT_SAMPLE;
        case icocp_filtering_bilinear: return STBIR_FILTER_TRIANGLE;
        case icocp_filtering_catmullrom: return STBIR_FILTER_CATMULLROM;
    }

    // Point sampling is often a nice default at common icon sizes
    return STBIR_FILTER_POINT_SAMPLE; 
}

icocp_err_t icocp_convert_image_file_to_icon(
    const char* input_filename,
    const char* output_filename,
    const icocp_conv_params_t* params)
{
    if (!input_filename || !*input_filename) return icocp_err_invalid_input;

    icocp_err_t result = icocp_err_unknown;

    loaded_file_t file = common_file_load_into_memory(input_filename, false);
    if (common_file_is_loaded(&file))
    {
        result = icocp_convert_image_data_to_icon(
            file.data, 
            file.length, 
            output_filename,
            params
        );
        common_file_free(&file);
    }
    else
    {
        result = icocp_err_invalid_file;
    }
    
    return result;
}

icocp_err_t icocp_convert_image_data_to_icon(
    const uint8_t* data,
    size_t datalen,
    const char* output_filename,
    const icocp_conv_params_t* params)
{
    if (!output_filename || !*output_filename) return icocp_err_invalid_input;

    icocp_err_t result = icocp_err_unknown;

    int32_t width = 0;
    int32_t height = 0;
    int32_t color_channels = 0;

    uint8_t* image_data = stbi_load_from_memory(
        data, 
        (int32_t)datalen, 
        &width, 
        &height, 
        &color_channels, 
        4
    );
    if (image_data)
    {
        int32_t target_size_index = -1;
        if (width == height)
        {
            if (color_channels == 3 || color_channels == 4)
            {
                target_size_index = (int32_t)ICOCP_MAX_ICON_SUBIMAGES - 1;
                while (target_size_index >= 0)
                {
                    if (k_icon_target_sizes[target_size_index] <= width)
                    {
                        break;
                    }

                    --target_size_index;
                }
            }
        }
        
        if (target_size_index >= 0)
        {
            int32_t images_to_process = target_size_index + 1;
            if (params != NULL &&
                params->max_images < ICOCP_MAX_ICON_SUBIMAGES &&
                params->max_images > 0 &&
                params->max_images < images_to_process)
            {
                images_to_process = params->max_images;
            }

            FILE* output_file = fopen(output_filename, "wb");
            if (output_file)
            {
                icocp_icon_body_data_t body_data = { 0 };

                int32_t current_target_size_index = target_size_index;
                while (current_target_size_index >= 0 && images_to_process > 0)
                {
                    const int32_t target_size = k_icon_target_sizes[current_target_size_index];

                    uint8_t *buffer = image_data;
                    bool was_resized = false;

                    if (width != target_size)
                    {
                        const stbir_filter filtering = icocp_get_stbir_filter_from_icocp_filter(
                            (params != NULL) ? params->filtering : icocp_filtering_point
                        );

                        const int32_t color_format = (color_channels == 3) ? STBIR_RGB : STBIR_RGBA;

                        buffer = (uint8_t *)malloc(target_size * target_size * color_channels);
                        buffer = stbir_resize(
                            image_data, width, height, width * color_channels,
                            buffer, target_size, target_size, target_size * color_channels,
                            color_format, STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP, filtering
                        );

                        was_resized = true;
                    }

                    stbi_write_png_to_func(
                        &icocp_write_png_data_callback,
                        &body_data,
                        target_size,
                        target_size,
                        color_channels, 
                        buffer, 
                        target_size * color_channels
                    );

                    if (was_resized)
                    {
                        free(buffer);
                    }

                    --current_target_size_index;
                    --images_to_process;
                }

                if (body_data.png_data_count > 0)
                {
                    const uint32_t size_of_header_data = 6 + (uint32_t)body_data.png_data_count * 16;

                    // Write ICO Header (6 bytes)
                    common_file_write_u16(output_file, 0); // Two reserved bytes - always 0
                    common_file_write_u16(output_file, (params && params->as_cursor) ? 2 : 1); // 2 byte value - 1 for icon, 2 for cursor
                    common_file_write_u16(output_file, (uint16_t)body_data.png_data_count); // 2 byte value - number of images

                    // Write ICONDIRENTRY headers for each subimage (16 bytes each)
                    for (uint16_t i = 0; i < body_data.png_data_count; ++i)
                    {
                        const uint8_t image_size = 
                            (k_icon_target_sizes[target_size_index - i] > 255) 
                                ? 0 
                                : (uint8_t)k_icon_target_sizes[target_size_index - i];

                        // width - 0 means 256
                        common_file_write_u8(output_file, image_size); 

                        // height - 0 means 256
                        common_file_write_u8(output_file, image_size); 
                        
                        // color palette
                        common_file_write_u8(output_file, 0); 

                        // reserved - expected 0
                        common_file_write_u8(output_file, 0); 

                        // color planes or x hotspot for cursors
                        common_file_write_u16(
                            output_file, 
                            (params && params->as_cursor) ? (uint16_t)params->cursor_hotspot_x : 0
                        ); 

                        // bits per pixel or y hotspot for cursors
                        common_file_write_u16(
                            output_file, 
                            (params && params->as_cursor) ? (uint16_t)params->cursor_hotspot_y : 32
                        ); 
                        
                        // size of png
                        common_file_write_u32(output_file, (uint32_t)body_data.png_data_size[i]); 

                        // offset
                        common_file_write_u32(
                            output_file, 
                            (uint32_t)body_data.png_data_offset[i] + size_of_header_data
                        ); 
                    }

                    for (uint16_t i = 0; i < body_data.png_data_count; ++i)
                    {
                        fwrite(
                            body_data.png_data[i],
                            (size_t)body_data.png_data_size[i],
                            1,
                            output_file
                        );

                        free(body_data.png_data[i]);
                    }
                }

                fclose(output_file);
                result = icocp_err_success;
            }
        } 
        else 
        {
            result = icocp_err_invalid_image_data;
        }

        stbi_image_free(image_data);
    }

    return result;
}

static void icocp_write_png_data_callback(void* userdata, void* data, int size)
{
    if (!userdata || !data || size <= 0) return;

    icocp_icon_body_data_t* body_data = (icocp_icon_body_data_t*)userdata;
    if (body_data->png_data_count >= ICOCP_MAX_ICON_SUBIMAGES) return;

    const int32_t index = (body_data->png_data_count++);

    body_data->png_data[index] = (uint8_t*)malloc(size);
    memcpy(body_data->png_data[index], data, size);

    body_data->png_data_size[index] = size;

    const int32_t offset = (index > 0) 
        ? body_data->png_data_offset[index - 1] + body_data->png_data_size[index - 1] 
        : 0;
    body_data->png_data_offset[index] = offset;
}