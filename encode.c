#include <stdio.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* check_operation_type() checks whether the argument passed is for encode or decode */
OperationType check_operation_type(char * argv[])
{
    if (argv[1][0] == '-')
    {
        if (argv[1][1] == 'e')
            return e_encode;
        else if (argv[1][1] == 'd')
            return e_decode;
    }
    return e_unsupported;

}

/* 
   read_and_validate_encode_args() checks the argument for .bmp file.
   It also stores the secret filename
*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (strstr(argv[2], ".bmp"))
    {
        encInfo -> src_image_fname = argv[2];
    }
    else
    {
        printf("Error : It is not .bmp file\n");
        return e_failure;
    }

    if (argv[3])
    {
        encInfo -> secret_fname = argv[3];
    }
    else
    {
        printf("Error : Secret file not passed\n");
        return e_failure;
    }

    if (argv[4])
    {
        if (strstr(argv[4], ".bmp"))
            encInfo -> stego_image_fname = argv[4];
        else
        {
            printf("Error : Given output file is not .bmp file\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO : Output File not mentioned. Creating stego.bmp as default\n");
        encInfo -> stego_image_fname = "stego.bmp";
    }
    return e_success;
}

/* Perform encoding functions */
Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO : Opening required files\n");
    if (open_files(encInfo) == e_failure)
    {
        printf("INFO : Open Files returns failure\n");
        return e_failure; 
    }
    else
        printf("INFO : Done\n");

    strcpy(encInfo -> extn_secret_file,  strrchr(encInfo -> secret_fname, '.'));
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);

    printf("INFO : ##### Encoding Procedure Started #####\n");
    printf("INFO : Checking for %s size\n", encInfo -> secret_fname);
    if (encInfo -> size_secret_file == 0)
    {
        printf("INFO : Secret file is empty\n");
        return e_failure;
    }
    else
        printf("INFO : Done. Not Empty\n");

    printf("INFO : Checking for %s capacity to handle %s\n", encInfo -> src_image_fname, encInfo -> secret_fname);
    if (check_capacity(encInfo) == e_failure)
    {
        printf("INFO : Done. Image don't have capacity to encode the message\n");
        return e_failure;
    }
    else
    {
        printf("INFO : Done. Found OK\n");
    }
  
    printf("INFO : Copying Image Header\n");
    if (copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)
    {
        printf("ERROR : Header not copied\n");
        return e_failure;
    }
    else
    {
        printf("INFO : Done\n");
    }

    printf("INFO : Encoding Magic String Signature\n");
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        printf("ERROR : Encoding Magic String Failure\n");
        return e_failure;
    }
    else
    {
        printf("INFO : Done\n");
    }

    printf("INFO : Encoding %s File Extention Size\n", encInfo -> secret_fname);
    if (encode_secret_file_extn_size((uint)strlen(encInfo -> extn_secret_file), encInfo) == e_failure)
    {
        printf("ERROR : Encoding Secret File extension size failure\n");
        return e_success;
    } 
    else
    {
        printf("INFO : Done\n");
    }

    printf("INFO : Encoding %s File Extention\n", encInfo -> secret_fname);
    if (encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_failure)
    {
        printf("ERROR : Encoding Secret File extension failure\n");
        return e_failure;
    }
    else
    {
        printf("INFO : Done\n");
    }
   
    printf("INFO : Encoding %s File Size\n", encInfo -> secret_fname);
    if (encode_secret_file_size((long)encInfo -> size_secret_file, encInfo) == e_failure)
    {
        printf("ERROR : Encoding Secret File size failure\n");
        return e_success;
    }
    else
    {
        printf("INFO : Done\n");
    }

    printf("INFO : Encoding %s File Data\n", encInfo -> secret_fname);
    if (encode_secret_file_data(encInfo) == e_failure)
    {
        printf("ERROR : Encoding Secret File Data failure\n");
        return e_failure;
    }
    else
    {
        printf("INFO : Done\n");
    }

    printf("INFO : Copying Left Over Data\n");
    if (copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)
    {
        printf("ERROR : Copying Remaining Data failure\n");
        return e_failure;
    }
    else
    {
        printf("INFO : Done\n");
    }

    printf("INFO : ##### Encoding Done Successfully #####\n");

}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);

    // Return image capacity
    return width * height * 3;
}

Status open_files(EncodeInfo *encInfo)
{
    short int signature;
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    fread(&signature, sizeof(short), 1, encInfo -> fptr_src_image);
    if (signature != 0x4d42)
    {
        printf("%s is not having BM signature\n", encInfo -> src_image_fname);
        return e_failure;
    }
    printf("INFO : Opened %s\n", encInfo -> src_image_fname);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);

    printf("INFO : Opened %s\n", encInfo -> secret_fname);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    printf("INFO : Opened %s\n", encInfo -> stego_image_fname);

    // No failure return e_success
    return e_success;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);

    return ftell(fptr);
}

/* check_capacity() is called to check if the secret file can fit into the image file */
Status check_capacity(EncodeInfo *encInfo)
{
    long int capacity;

    capacity = (strlen(MAGIC_STRING) + sizeof(int) + strlen(encInfo -> extn_secret_file) + sizeof(unsigned long int) + encInfo -> size_secret_file) * 8 + 54;

    if (capacity < encInfo -> image_capacity)
        return e_success;
    else
        return e_failure;
}

/* copy_bmp_header() is called to caopy the header files from image file to stego file */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char bmp_header[54];
    rewind(fptr_src_image);
    if (fread(bmp_header, sizeof(char), 54, fptr_src_image) == 54)
    {
        if (fwrite(bmp_header, sizeof(char), 54, fptr_stego_image) == 54)
        {
            return e_success;
        }
    }
    else
        return e_failure;
}

/* encode_byte_to_lsb() is called to copy the encoded bytes into the lsb */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        if (((unsigned char)data >> 7 - i) & 1)
        {
            image_buffer[i] |= 1;
        }
        else
            image_buffer[i] &= ~1;
    }
    return e_success;
}

/* encode_data_to_image() is called to encode the data before sending it to the image file */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[8];
    for (int i = 0; i < size; i++)
    {
        if (fread(image_buffer, sizeof(char), 8, fptr_src_image) == 8)
        {
            encode_byte_to_lsb(data[i], image_buffer);
            if (fwrite(image_buffer, sizeof(char), 8, fptr_stego_image) != 8)
                return e_failure;   
        }
        else
            return e_failure;
    }
    return e_success;
}

/* encode_magic_string() is called to encode the magic string and store it into stego file */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    if (encode_data_to_image((char *)magic_string, strlen(magic_string), encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)
        return e_failure;
    else
        return e_success;
}

/* encode_secret_file_extn_size() is called encode the secret file extention size and copy it into stego file */
Status encode_secret_file_extn_size(uint extn_size, EncodeInfo *encInfo)
{
    char ch;
    unsigned int mask = 0xFF000000;
    for (int i = 0; i < sizeof(int); i++)
    {
        ch = (extn_size & mask) >> 24 - (i * 8);
        mask >>= 8;
        encode_data_to_image(&ch, 1, encInfo -> fptr_src_image, encInfo -> fptr_stego_image);        
    }
    return e_success;
}

/* This function encodes the secret file extention and copies it into stego file */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    if (encode_data_to_image((char *)file_extn, strlen(file_extn), encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)
        return e_failure;
    else
        return e_success;
} 

/* This function encodes the secret file size and stores it into stego file */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    unsigned long int mask = 0xFF00000000000000;
    char ch;
    for (int i = 0; i < sizeof(long); i++)
    {
        ch = (file_size & mask) >> 56 - (i * 8);
        mask >>= 8;
        encode_data_to_image(&ch, 1, encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
    }
    return e_success;
}

/* This function encodes the secret file data and stores it into the stego file */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo -> fptr_secret);
    char sec_d[encInfo -> size_secret_file];
    fread(sec_d, sizeof(char), (int)(encInfo -> size_secret_file), encInfo -> fptr_secret);
    if (encode_data_to_image(sec_d, (int)(encInfo -> size_secret_file), encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_failure)
        return e_failure;
    else
        return e_success;
}

/* This function copies the remaining data from the image file into stego file */
Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    long int start, end, size;
    start = ftell(fptr_src_image);
    fseek(fptr_src_image, 0, SEEK_END);
    end = ftell(fptr_src_image);
    size = end - start;
    fseek(fptr_src_image, -size, SEEK_CUR);
    char bmp_remaining[size];
    if (fread(bmp_remaining, sizeof(char), size, fptr_src_image) == size)
    { 
        if (fwrite(bmp_remaining, sizeof(char), size, fptr_stego_image) == size)
        return e_success;
    }
    else
        return e_failure;
       
}
