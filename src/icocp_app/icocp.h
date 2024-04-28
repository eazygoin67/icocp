#pragma once 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <string.h>
#include <math.h>

#define ICOCP_MAX_ICON_SUBIMAGES 4

typedef enum icocp_err_t
{
    icocp_err_success,
    icocp_err_invalid_input,
    icocp_err_invalid_file,
    icocp_err_input_write_failure,
    icocp_err_invalid_image_data,
    
    icocp_err_unknown

} icocp_err_t;

typedef struct icocp_conv_params_t
{
    int32_t max_images;

} icocp_conv_params_t;

typedef struct icocp_loaded_image_data_t
{
    int32_t width;
    int32_t height;
    int32_t color_channels;

    uint8_t* data;

} icocp_loaded_image_data_t;

icocp_err_t icocp_convert_image_file_to_icon(
    const char* input_filename,
    const char* output_filename,
    const icocp_conv_params_t* params);
icocp_err_t icocp_convert_image_data_to_icon(
    const uint8_t* data,
    size_t datalen,
    const char* output_filename,
    const icocp_conv_params_t* params);