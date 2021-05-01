#include <stdio.h>
#include "decode.h"
#include "types.h"

/* Function Definitions */
/* This function reads the arguments and validates whether it is .bmp file or not */
/* It also checks whether the output file is present */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (strstr(argv[2], ".bmp"))
    {
        decInfo -> stego_image_fname = argv[2];
    }
else
    {
        printf("Error : It is not .bmt file\n");
        return d_failure;
    }

    if (argv[3])
    {
        decInfo -> decoded_fname = argv[3];
    }
    else
    {
        printf("INFO : Output File not mentioned. Creating decoded.txt as default\n");
        decInfo -> decoded_fname = "decoded.txt";
    }
    return d_success;

}

/* This function performs the decoding function */
Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO : ##### Decoding Procedure Started #####\n");
    printf("INFO : Opening required file\n");
    if (open_encoded_files(decInfo) == d_failure)
    {
        printf("INFO : Open files returns failure\n");
        return d_failure;
    }

    printf("INFO : Decoding Magic String Signature\n");
    if (decode_magic_string(MAGIC_STRING, decInfo) == d_failure)
    {
        printf("ERROR : Decoding Magic String Signature Failure\n");
        return d_failure;
    }
    else
    {
        printf("INFO : Done\n");
    }

    printf("INFO : Decoding Output File Extention Size\n");
    if (decode_secret_file_extn_size(decInfo) == d_failure)
    {
        printf("ERROR : Decoding Output File extention size failure\n");
        return d_failure;
    }
    else
    {
        printf("INFO : Done\n");
    }
    
    printf("INFO : Decoding Output File Extention\n");
    if (decode_secret_file_extn(decInfo) == d_failure)
    {
        printf("ERROR : Decoding Output File Extention failure\n");
        return d_failure;
    }
    else
    {
        printf("INFO : Done\n");
    }
   
    if (strstr(decInfo -> decoded_fname, ".txt") == NULL)
    {
        strcat(decInfo -> decoded_fname, decInfo -> extn_decoded_file);
    }
    
    decInfo -> fptr_decoded_file = fopen(decInfo -> decoded_fname, "w");
    if (decInfo -> fptr_decoded_file == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo -> decoded_fname);
        return d_failure;
    }


    printf("INFO : Decoding %s File Size\n", decInfo -> decoded_fname);
    if (decode_secret_file_size(decInfo) == d_failure)
    {
        printf("ERROR : Decoding Secret File Size failure\n");
        return d_failure;
    }
    else
    {
        printf("INFO : Done\n");
    }
    
    printf("INFO : Decoding %s File Data\n", decInfo -> decoded_fname);
    if (decode_secret_file_data(decInfo) == d_failure)
    {
        printf("ERROR : Decoding Output File Data failure\n");
        return d_failure;
    }
    else
    {
        printf("INFO : Done\n");
    }

    printf("INFO : ##### Decoding Done Successfully #####\n");

}

/* This function opens the files and intializes the pointers */
Status open_encoded_files(DecodeInfo *decInfo)
{
    decInfo -> fptr_stego_image = fopen(decInfo -> stego_image_fname, "r");
    if (decInfo -> fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo -> stego_image_fname);
        return d_failure;
    }
    printf("INFO : Opened %s\n", decInfo -> stego_image_fname);

    printf("INFO : Done. Opened all required files\n");

    return d_success;
}

/* Ths function decodes the magic string */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    fseek(decInfo -> fptr_stego_image, 54, SEEK_SET);
    char data[strlen(magic_string)];
    char array[8];
    for (int i = 0; i < strlen(magic_string); i++)
    {
        fread(array, 8, 1, decInfo -> fptr_stego_image);
        for (int j = 0; j < sizeof(char)*8; j++)
        {
            data[i] = ((array[j] & 1) << (7 - j)) | data[i];
        } 
    }
    // Check if the magic string matches

    if (strcmp(magic_string, data) == 0)
        return d_success;
    else
        return d_failure;
}

/* This function decodes the secret file extention size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{ 
    char file_size[DECODE_FILE_EXTN_SIZE];
	fread(file_size, sizeof(char), DECODE_FILE_EXTN_SIZE, decInfo->fptr_stego_image);
	if (ferror(decInfo->fptr_stego_image))
    {
         fprintf(stderr,"Error: While reading the data from stego image file\n");
         clearerr(decInfo->fptr_stego_image);
         return d_failure;
    }

	decInfo->extn_size = 0;
	for (uint i = 0; i < DECODE_FILE_EXTN_SIZE; i++)
	{
		decInfo->extn_size <<= 1;
		decInfo->extn_size |= (uint) (file_size[i] & 0x01);
	}
    return d_success;
}

/* This file decodes the secret file extention */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    int i, j;
    char data[sizeof(int)];
    char array[8];
    for (i = 0; i < sizeof(int); i++)
    {
        fread(array, 8, 1, decInfo -> fptr_stego_image);
        for (j = 0; j < sizeof(char)*8; j++)
        {
            data[i] = ((array[j] & 1) << (7 - j)) | data[i];
        }
    } 
    data[i] = '\0';
    strcpy(decInfo -> extn_decoded_file, data);
    return d_success;
}

/* This function decodes the secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{    
    char file_size[DECODE_FILE_SIZE];
	fread(file_size, sizeof(char), DECODE_FILE_SIZE, decInfo->fptr_stego_image);
	if (ferror(decInfo->fptr_stego_image))
    {
         fprintf(stderr,"Error: While reading the data from stego image file\n");
         clearerr(decInfo->fptr_stego_image);
         return d_failure;
    }
	decInfo->size_decoded_file = 0;
	for (uint i = 0; i < DECODE_FILE_SIZE; i++)
	{
		decInfo->size_decoded_file <<= 1;
		decInfo->size_decoded_file |= (uint) (file_size[i] & 0x01);
	}
	return d_success;    
}

/* This function decodes the secret file data and writes it into the output file */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    int i, j;
    char data[decInfo -> size_decoded_file+1];
    decode_data_from_image (decInfo -> fptr_stego_image, data, (int)decInfo -> size_decoded_file);
     data[decInfo -> size_decoded_file] = '\0';    
    fwrite(data, sizeof(char), (int)decInfo -> size_decoded_file, decInfo -> fptr_decoded_file);
    return d_success;
}

/* This function decodes the data from the image file */
Status decode_data_from_image (FILE *fptr_stego_image, char *data, int size)
{
    int i;
    char image_buffer[8];
    for (i = 0; i < size; i++)
    {
        fread(image_buffer, sizeof(char), 8, fptr_stego_image);
        decode_byte_from_lsb(image_buffer, &data[i]);
    }
    return d_success;
}

/* This file decodes the bytes from the lsb */
Status decode_byte_from_lsb (char *image_buffer, char *data)
{
    int i;
    *data = 0;
    for (i = 0; i < 8; i++)
    {
        *data = ((image_buffer[i] &1) << (7 - i)) | *data;
    }
    return d_success;
}
