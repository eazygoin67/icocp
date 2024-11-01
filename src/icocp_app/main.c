#include "icocp.h"

static bool str_starts_with(const char* str, const char* starts_width);
static void print_usage();

int main(int arg_count, char* args[])
{
    printf("--- IcoCP - Png to Ico Converter --- \n\n");

    if (arg_count == 2)
    {
        const char* input = args[1];
        if (strcmp(input, "--help") == 0)
        {
            print_usage();
            return EXIT_SUCCESS;
        }
    }

    if (arg_count >= 3)
    {
        const char* input_filename = NULL;
        const char* output_filename = NULL;
        icocp_conv_params_t params = { 0 };

        for (int i = 1; i < arg_count; ++i)
        {
            const char* arg = args[i];
            if (strcmp(arg, "--input") == 0 || strcmp(arg, "--output") == 0)
            {
                const char** filename_result = (arg[2] == 'o') ? &output_filename : &input_filename;
                if (i >= (arg_count - 1))
                {
                    printf("Error: Expect filename after parameter \"%s\"\n", arg);
                    return EXIT_FAILURE;
                }

                const char* filename_arg = args[++i];
                if (filename_arg == NULL || *filename_arg == 0)
                {
                    printf("Error: Invalid filename specified after \"%s\"\n", arg);
                    return EXIT_FAILURE;
                }

                *filename_result = filename_arg;
            }
            else if (str_starts_with(arg, "--max_images="))
            {
                params.max_images = strtol(arg + 13, NULL, 10);
            }
            else if (str_starts_with(arg, "--filtering="))
            {
                const int32_t k_param_check_offset = sizeof("--filtering=") - 1;
                switch (arg[k_param_check_offset])
                {
                    case 'p':
                    case 'P': { params.filtering = icocp_filtering_point; } break;

                    case 'b':
                    case 'B': { params.filtering = icocp_filtering_bilinear; } break;

                    case 'c':
                    case 'C': { params.filtering = icocp_filtering_catmullrom; } break;
                }
            }
            else if (strcmp(arg, "--cursor") == 0)
            {
                params.as_cursor = true;
            }
            else if (str_starts_with(arg, "--hotspot="))
            {
                const char *value_str = arg + sizeof("--hotspot=") - 1;
                const char* next = NULL;
                int32_t x = 0, y = 0;

                x = strtol(value_str, &next, 10);
                if (next && (*next == 'x' || *next == ','))
                {
                    ++next;
                    y = strtol(next, NULL, 10);
                }

                params.cursor_hotspot_x = x;
                params.cursor_hotspot_y = y;
            }
            else
            {
                printf("Warning: Unknown parameter \"%s\" - skipping...\n", arg);
            }
        }

        if (!input_filename)
        {
            printf("Error: No input filename specified\n");
            return EXIT_FAILURE;
        }

        if (!output_filename)
        {
            printf("Error: No output filename specified\n");
            return EXIT_FAILURE;
        }

        printf("Converting file \"%s\" to icon file \"%s\" ... \n", input_filename, output_filename);
        const icocp_err_t result = icocp_convert_image_file_to_icon(
            input_filename,
            output_filename,
            &params
        );

        switch (result)
        {
            case icocp_err_success:
            {
                printf("Success!\n");
            }
            break;

            case icocp_err_invalid_input: 
            {
                printf("Error: Invalid input\n");
            }
            break;

            case icocp_err_invalid_file: 
            {
                printf("Error: Unable to open input file\n");
            }
            break;

            case icocp_err_input_write_failure: 
            {
                printf("Error: Failed to write t output file\n");
            }
            break;

            case icocp_err_invalid_image_data: 
            {
                printf("Error: Invalid image - expected RGB/RGBA input "
                       "image of size that is square and at least 16x16 "
                       "pixels in size\n");
            }
            break;

            default: 
            {
                printf("Error: Undefined error\n");
            }
            break;
        }

        return (result == icocp_err_success) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    printf("Error: Invalid input. Use command \"icocp --help\" for usage.\n");
    return EXIT_FAILURE;
}

static bool str_starts_with(const char* str, const char* starts_width)
{
    if (!starts_width || !str || *starts_width == 0 || *str == 0) return false;
    return strncmp(starts_width, str, strlen(starts_width)) == 0;
}

static void print_usage()
{
    printf("Usage:\n");

    printf("    icoCP --help :: Print help text\n\n");

    printf("    icocp --input /image/to/convert.png --output icon/output/file.ico --max_images=n :: Convert a image file to an icon.\n");
    printf("        --input :: specifies that the next parameter will be the input file. Supports png, bmp and tga.\n");
    printf("        --output :: specifies that the next parameter will be the output file. Expects a .ico file.\n");
    printf("        --max_images=n :: (OPTIONAL) limits the number of sub images created to n where n > 1.\n");
    printf("        --filtering=op :: (OPTIONAL) specifies filtering when doing downsampling between subimages sizes. Possible values:\n");
    printf("                    Point - Simple point sampling\n");
    printf("                    Bilinear - Simple bilinear sampling\n");
    printf("                    Point - An interpolating cubic spline\n");
    printf("        --cursor :: (OPTIONAL) Exports the icon as in a cursor format instead. Generally uses the .cur extension.\n");
    printf("        --hotspot=x,y :: (OPTIONAL) Specifies the cursor hotspot value.\n");

    printf("\n");
}