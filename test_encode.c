#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;
    int status;
    if (argc > 2)
    {
        // Call check_operation_type function
        status = check_operation_type(argv);
        if (status == e_encode)
        {
            printf("INFO : Validating arguments\n");
            // Call read_and_validate_encode_args function
            status = read_and_validate_encode_args(argv, &encInfo);
            if (status == e_failure)
            {
                printf("ERROR : Invalid file names\n");
            }
            else
            {
                printf("INFO : OK\n");
                // Call do_encoding function
                status = do_encoding(&encInfo);
                if (status == e_failure)
                {
                    printf("INFO : Encoding failure\n");
                }
            }
        }
        else if (status == e_decode)
        {
            printf("INFO : Validating arguments\n");
            // Call read_and_validate_decode_args function
            status = read_and_validate_decode_args(argv, &decInfo);
            if (status == d_failure)
            {
                printf("Error : Invalid file names\n");
            }
            else
            {
                // Call do_decoding function
                status == do_decoding(&decInfo);
                if (status == d_failure)
                {
                    printf("INFO : Decoding failure\n");
                }
            }
        }
        else
        {
            printf("ERROR : Invalid option\nUsage : ./a.out -e <src.bmp> <secret_file> [output.bmp]\n./a.out -d <stego.bmp [outputfile]\n"); 
        }
    }
    else
        printf("ERROR : Invalid Number of arguments\nUsage : ./a.out -e <src.bmp> <secret_file> [output.bmp]\n./a.out -d <stego.bmp [outputfile]\n"); 
 
   return 0;
}
