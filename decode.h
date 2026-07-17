//NAME : AISHWARYA B BHLAKE
//BATCH ID : 25012A_096
//DATE OF SUBMISSION : 31 AUGUST 2025
//PROJECT NAME : LSB STEGANOGRAPHY
#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "common.h"
#include "types.h"

// Maximum allowed length for secret file extension (e.g., .txt, .png, etc.)
#define MAX_DECODE_FILE_SUFFIX 5

// Structure to hold all necessary information for decoding
typedef struct _DecodeInfo
{
    char *stego_image_fname;           // Name of the stego image file (input)
    FILE *fptr_stego_image;            // File pointer to stego image
    int size_secret_file;              // Size of the secret file data in bytes
    int extn_size;                     // Size of the secret file extension (e.g., 4 for ".txt")
    char *output_file_name;            // Final decoded output file name (e.g., output.txt)
    char *output_file_basename;        // User-specified base name (e.g., "output" → output.txt)
    FILE *fptr_output;                 // File pointer to output file
    char extn[MAX_DECODE_FILE_SUFFIX]; // Buffer to temporarily store file extension
} DecodeInfo;

// Function Prototypes

// Opens the decoded output file for writing
Status open_output_file(DecodeInfo *decInfo);

// Validates command-line arguments and sets DecodeInfo fields
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

// Main controller function to orchestrate decoding process
Status do_decoding(DecodeInfo *decInfo);

// Opens the stego image file for reading
Status open_image_file(DecodeInfo *decInfo);

// Skips the BMP header (54 bytes)
Status skip_bmp_header(FILE *fptr_stego_image);

// Decodes and verifies the magic string (e.g., "#*") from image
Status decode_magic_string(DecodeInfo *decInfo);

// Decodes the size of the secret file extension from image
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

// Decodes the file extension (e.g., ".txt") from image
Status decode_secret_file_extn(DecodeInfo *decInfo);

// Decodes the size of the secret file in bytes
Status decode_secret_file_size(DecodeInfo *decInfo);

// Decodes the secret file data and writes it to the output file
Status decode_secret_file_data(DecodeInfo *decInfo);

// Decodes a single byte from 8 LSBs in image buffer
void decode_byte_from_lsb(char *ch, const char *image_buffer);

// Decodes an integer (4 bytes) from 32 LSBs (4 * 8) in image buffer
void decode_int_from_lsb(int *data, char *image_buffer);

#endif
