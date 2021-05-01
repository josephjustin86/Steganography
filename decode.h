#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
#define DECODE_FILE_EXTN_SIZE 32
#define DECODE_FILE_SIZE 64
typedef struct _DecodeInfo
{
    /* Encoded File Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char image_data[MAX_IMAGE_BUF_SIZE];
    uint image_data_size;

    /* Decoded File Info */
    char *decoded_fname;
    FILE *fptr_decoded_file;
    int extn_size;
    char extn_decoded_file[MAX_FILE_SUFFIX];
    char *decoded_data;
    long size_decoded_file;

} DecodeInfo;

/* Decoding functions prototype */

/* Declare read and validate function */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Declare open encoded files function */
Status open_encoded_files(DecodeInfo *decInfo);

/* Declare the encoding function */
Status do_decoding(DecodeInfo *decInfo);

/* Declare the decode magic string function */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Declare the decode secret fileextention size function */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Declare the decode secret file extention function */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Declare the decode secret file size function */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Declare the data from image function */
Status decode_data_from_image(FILE *fptr_stego_image, char *data, int size);

/* Declare the decode the byte to lsb function */
Status decode_byte_from_lsb(char *image_buffer, char *data);

/* Declare the decode decret file data function */
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif
