#include "icocp.h"

static bool str_starts_with(const char* str, const char* starts_width);
static void print_usage();

int main(int arg_count, char* args[])
{
    printf("icocp\n");

    if (arg_count <= 1)
    {
        print_usage();
        return EXIT_FAILURE;
    }
     
    if (arg_count == 2)
    {
        const char* input = args[1];
        if (strcmp(input, "--help"))
        {
            print_usage();
            return EXIT_SUCCESS;
        }
    }

    if (arg_count >= 3)
    {
        const char* const input_filename = args[1];
        const char* const output_filename = args[2];
        
        icocp_conv_params_t params = { 0 };
        for (int i = 3; i < arg_count; ++i)
        {
            const char *arg = args[i];
            
            if (str_starts_with(arg, "--max_images="))
            {
                params.max_images = strtol(arg + 13, NULL, 10);
            }
        }
        
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
                       "image of size 256x256, 48x48, 32x32 or 16x16\n");
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

    return EXIT_FAILURE;
}

static bool str_starts_with(const char* str, const char* starts_width)
{
    if (!starts_width || !str || *starts_width == 0 || *str == 0) return false;
    return strncmp(starts_width, str, strlen(starts_width)) == 0;
}

static void print_usage()
{
    printf("--- IcoCP - Png to Ico Converter --- \n\n");
    printf("Usage:\n");

    printf("    icoCP --help :: Print help text\n\n");

    printf("    icocp /image/to/convert.png icon/output/file.ico --max_images=n :: Convert a image file to an icon\n");
    printf("        --max_images=n :: limits the number of sub images created to n where n > 1\n\n");

    //printf("    icocp /some/icon/file.ico :: Outputs basic information on the icon file\n");
}