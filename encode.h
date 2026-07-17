//NAME : AISHWARYA B BHALKE
//BATCH ID : 25012A_096
//DATE OF SUBMISSION : 31 AUGUST 2025
//PROJECT NAME : LSB STEGANOGRAPHY

#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types like Status and OperationType

/* 
 * Structure to store information required for
 * encoding a secret file into a source BMP image.
 * Includes info about secret file, image, and output file.
 */

#define MAX_FILE_SUFFIX 4  // Max length of secret file extension (e.g., "txt")

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;        // Name of source BMP image
    FILE *fptr_src_image;         // File pointer for source image
    uint image_capacity;          // Total number of bytes available for encoding

    /* Secret File Info */
    char *secret_fname;           // Name of the secret file to encode
    FILE *fptr_secret;            // File pointer for secret file
    char extn_secret_file[MAX_FILE_SUFFIX];  // Extension of secret file (e.g., "txt")
    long size_secret_file;        // Size of the secret file in bytes

    /* Stego Image Info */
    char *stego_image_fname;      // Name of output stego image (encoded)
    FILE *fptr_stego_image;       // File pointer for stego image

} EncodeInfo;


// Encoding function prototypes 

// Determines if the operation is encoding or decoding based on argv[]
OperationType check_operation_type(char *argv[]);

// Reads and validates command-line arguments for encoding 
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

// Main function that controls the encoding steps
Status do_encoding(EncodeInfo *encInfo);

// Opens required files: source image, secret file, stego image 
Status open_files(EncodeInfo *encInfo);

// Checks if the image has enough capacity to encode all required data 
Status check_capacity(EncodeInfo *encInfo);

// Calculates total image size from BMP (excluding headers)
uint get_image_size_for_bmp(FILE *fptr_image);

// Calculates file size of any file
uint get_file_size(FILE *fptr);

// Copies BMP header (usually first 54 bytes) from source to stego image 
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

// Encodes the predefined magic string (e.g., "#*") into the stego image 
Status encode_magic_string(const char *magic_string, FILE *fptr_src, FILE *fptr_dest);

// Encodes the secret file extension (e.g., "txt") into the stego image 
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

// Encodes the size of the secret file extension into the image (e.g., 3 for "txt")
Status encode_secret_file_extn_size(int extn_size, EncodeInfo *encInfo);

// Encodes the size of the secret file (number of bytes) into the stego image 
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

// Encodes the actual contents of the secret file into the stego image 
Status encode_secret_file_data(EncodeInfo *encInfo);

// Encodes a given buffer of data into the image by modifying its LSBs 
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

// Encodes a single byte of data (8 bits) into the LSBs of 8 image bytes 
Status encode_byte_to_lsb(char data, char *image_buffer);

// Copies any remaining data from the source image to stego image after encoding 
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

// Encodes an integer (typically 4 bytes) into LSBs of 32 image bytes
Status encode_size_to_lsb(int data, char *image_buffer);

#endif // ENCODE_H
