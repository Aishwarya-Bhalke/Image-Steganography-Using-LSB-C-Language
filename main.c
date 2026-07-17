//NAME : AISHWARYA B BHALKE
//BATCH ID : 25012A_096
//DATE OF SUBMISSION : 24 AUGUST 2025
//PROJECT NAME : LSB STEGANOGRAPHY

#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "encode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("./lsb_steg:\n");
        printf("Encoding: ./lsb_steg -e <.bmp file> <.txt file> [output file]\n");
        printf("Encoding: ./lsb_steg -d <.bmp file> [output file]\n");
        return 0;
    }
    // Check if the operation type is ENCODE
    if (check_operation_type(argv) == e_encode)
    {
        EncodeInfo encodeInfo;

        // Ensure correct number of arguments for encoding (4 or 5)
        if (argc >= 4 && argc <= 5)
        {
            // Validate input arguments for encoding
            if (read_and_validate_encode_args(argv, &encodeInfo) == e_failure)
            {
                fprintf(stderr, "Error : Validation of encoding arguments failed.\n");
                printf("Encoding: ./lsb_steg -e <.bmp file> <.txt file> [output file]\n");
                return e_failure;
            }

            // Perform the encoding process
            if (do_encoding(&encodeInfo) == e_success)
            {
                printf("✅ Encoding completed successfully.\n\n");
            }
            else
            {
                fprintf(stderr, "Encoding failed.\n");
                return e_failure;
            }
        }
        else
        {
            // If arguments are not in the correct range
            fprintf(stderr, "Error: Invalid number of arguments for encoding.\n");
            printf("encoding: ./lsb_steg -e <.bmp file> [output file]\n");
            return e_failure;
        }
    }
    // Check if the operation type is DECODE
    else if (check_operation_type(argv) == e_decode)
    {
        DecodeInfo decodeInfo;

        // Ensure correct number of arguments for decoding (3 or 4)
        if (argc >= 3 && argc <= 4)
        {
            // Validate input arguments for decoding
            if (read_and_validate_decode_args(argv, &decodeInfo) == e_failure)
            {
                fprintf(stderr, "Error: Validation of decoding arguments failed.\n");
                printf("Decoding: ./lsb_steg -d <.bmp file> [output file]\n");
                return e_failure;
            }

            // Perform the decoding process
            if (do_decoding(&decodeInfo) == e_success)
            {
                printf("✅ Decoding completed successfully.\n\n");
            }
            else
            {
                fprintf(stderr, "Error: Decoding failed.\n");
                return e_failure;
            }
        }
        else
        {
            // If arguments are not in the correct range
            fprintf(stderr, "Error: Invalid number of arguments for decoding.\n");
            printf("Decoding: ./lsb_steg -d <.bmp file> [output file]\n");
            return e_failure;
        }
    }
    else
    {
        // If operation is neither encode nor decode
        fprintf(stderr, "Error : Unsupported operation type.\n");
        return e_failure;
    }
    
    // If everything goes well
    return 0;
}

// Determine whether encoding or decoding operation is requested
OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
    {
        printf("Now you have selected encoding\n\n");
        return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        printf("Now you have selected decoding\n\n");
        return e_decode;
    }
    else
    {
        fprintf(stderr, "Unsupported operation: %s\n\n", argv[1]);
        return e_unsupported;
    }
}
