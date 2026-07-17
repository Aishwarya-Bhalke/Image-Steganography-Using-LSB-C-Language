//NAME :AiSHWARYA BALAJI BHALKE
//BATCH ID : 25012A_096
//DATE OF SUBMISSION : 31 AUGUST 2025
//PROJECT NAME : LSB STEGANOGRAPHY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

// Function to read and validate command-line arguments for decoding
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check if the input stego image file is a BMP file
    if (strstr(argv[2], ".bmp"))
    {
        decInfo->stego_image_fname = argv[2];
        printf("Input image file is validated successfully.\n\n");
    }
    else
    {
        fprintf(stderr, "ERROR: %s is not a valid BMP file.\n\n", argv[2]);
        return e_failure;
    }
    // Check if output file basename is provided
    if (argv[3] == NULL)
    {
        decInfo->output_file_basename = "output"; // Default basename
        printf("Output file not mentioned. creating a deafult basename as output.\n\n");
    }
    else
    {
        // Output filename should not contain an extension
        if (strstr(argv[3], ".") != NULL)
        {
            printf("ERROR: Output file name should not contain extension\n\n");
            return e_failure;
        }
        decInfo->output_file_basename = argv[3];
    }
    return e_success;
}

// Main decoding function: drives the entire decoding process
Status do_decoding(DecodeInfo *decInfo)
{
    // Step 1: Open stego image file
    if (open_image_file(decInfo) != e_success)
        return e_failure;

    // Step 2: Skip BMP header
    if (skip_bmp_header(decInfo->fptr_stego_image) != e_success)
        return e_failure;
    printf("Skipped 54-byte BMP header successfully.\n\n");

    // Step 3: Decode magic string and validate
    if (decode_magic_string(decInfo) != e_success)
        return e_failure;

    // Step 4: Decode extension size
    if (decode_secret_file_extn_size(decInfo) != e_success)
        return e_failure;

    // Step 5: Decode extension and open output file
    if (decode_secret_file_extn(decInfo) != e_success)
        return e_failure;

    // Step 6: Decode size of the secret file content
    if (decode_secret_file_size(decInfo) != e_success)
        return e_failure;

    // Step 7: Decode secret file content and write to output
    if (decode_secret_file_data(decInfo) != e_success)
        return e_failure;

    return e_success;
}

// Function to open the stego image file for reading
Status open_image_file(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb"); // Open in binary read mode
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open stego image file %s\n\n", decInfo->stego_image_fname);
        return e_failure;
    }
    return e_success;
}

// Function to skip the BMP header (first 54 bytes)
Status skip_bmp_header(FILE *fptr_stego_image)
{
    fseek(fptr_stego_image, 54, SEEK_SET); // Move file pointer past header
    return e_success;
}

// Function to decode and validate the magic string from stego image
Status decode_magic_string(DecodeInfo *decInfo)
{
    char buffer[8];
    int magic_len = strlen(MAGIC_STRING);
    char magic_string[magic_len + 1];
    char user_magic_string[magic_len + 1];
    char ch;

    // Decode each character of the magic string
    for (int i = 0; i < magic_len; i++)
    {
        if (fread(buffer, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            fprintf(stderr, "ERROR: Unable to read from stego image.\n\n");
            return e_failure;
        }

        decode_byte_from_lsb(&ch, buffer); // Decode 1 byte
        magic_string[i] = ch;              // Store decoded character
    }

    magic_string[magic_len] = '\0'; // Null-terminate

    // Ask user for validation input
    printf("Enter the magic string to validate : ");
    scanf("%s", user_magic_string);
    printf("\n");
    // Compare decoded magic string with user input
    if (strcmp(magic_string, user_magic_string) != 0)
    {
        fprintf(stderr, "ERROR: Magic string mismatch. Decoding aborted.\n\n");
        return e_failure;
    }

    printf("Magic string verified successfully: %s\n\n", magic_string);
    return e_success;
}

// Function to decode the size of the secret file extension (stored in 32 LSBs)
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
    int extn_size;

    // Read 32 bytes from the image to decode extension size
    if (fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)
    {
        fprintf(stderr, "ERROR: Unable to read 32 bytes from stego image.\n\n");
        return e_failure;
    }

    decode_int_from_lsb(&extn_size, buffer); // Decode extension size

    decInfo->extn_size = extn_size;

    printf("Decoded secret file extension size: %d Bytes.\n\n", extn_size);
    return e_success;
}

// Function to decode the actual extension of the secret file
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[8];
    char extn[decInfo->extn_size + 1];

    // Decode each character of the extension from 8 LSBs
    for (int i = 0; i < decInfo->extn_size; i++)
    {
        if (fread(buffer, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            fprintf(stderr, "ERROR: Unable to read 8 bytes from stego image for extension character.\n\n");
            return e_failure;
        }
        decode_byte_from_lsb(&extn[i], buffer); // Decode one character
    }

    extn[decInfo->extn_size] = '\0'; // Null-terminate the extension

    printf("Decoded secret file extension: %s\n\n", extn);

    static char final_name[FILENAME_MAX] = "";
    strcpy(final_name, decInfo->output_file_basename); // Copy base name
    strcat(final_name, extn); // Append decoded extension

    decInfo->output_file_name = final_name; // Set final output filename

    // Open the output file for writing decoded data
    if (open_output_file(decInfo) == e_failure)
    {
        fprintf(stderr,"ERROR: Failed to open file\n\n");
        return e_failure;
    }
    return e_success;
}

// Function to open output file for writing the decoded secret data
Status open_output_file(DecodeInfo *decInfo)
{
    decInfo->fptr_output = fopen(decInfo->output_file_name, "w");
    if (decInfo->fptr_output == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open output file %s\n\n", decInfo->output_file_name);
        return e_failure;
    }
    return e_success;
}

// Function to decode the size of the secret file content
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    uint size = 0;

    // Read 32 bytes to get 4 bytes (uint) of secret file size
    if (fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)
    {
        fprintf(stderr, "ERROR: Unable to read 32 bytes from stego image for secret file size.\n\n");
        return e_failure;
    }

    decode_int_from_lsb((int *)&size, buffer); // Decode the size

    decInfo->size_secret_file = size;

    printf("Decoded secret file size: %u bytes.\n\n", size);
    return e_success;
}

// Function to decode actual secret file data and write to output file
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;

    // Loop through each byte of secret data
    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        // Read 8 bytes for 1 byte of data
        if (fread(buffer, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            fprintf(stderr, "ERROR: Failed to read 8 bytes from stego image while decoding secret file data.\n\n");
            return e_failure;
        }

        decode_byte_from_lsb(&ch, buffer); // Decode character

        // Write decoded character to output file
        if (fwrite(&ch, 1, 1, decInfo->fptr_output) != 1)
        {
            fprintf(stderr, "ERROR: Failed to write decoded byte to output file.\n\n");
            return e_failure;
        }
    }
    printf("Secret file data written to %s\n\n", decInfo->output_file_name);
    return e_success;
}

// Function to decode a single byte from the LSBs of 8 bytes in image buffer
void decode_byte_from_lsb(char *ch, const char *image_buffer)
{
    *ch = 0;
    for (int i = 0; i < 8; i++)  
    {
        *ch = *ch << 1;                  // Left shift by 1 to make space
        *ch = *ch | (image_buffer[i] & 1);   // Extract LSB and set it in *ch
    }
}

// Function to decode an integer (32 bits) from the LSBs of 32 bytes in image buffer
void decode_int_from_lsb(int *data, char *image_buffer)
{
    *data = 0;
    for (int i = 0; i < 32; i++)
    {
        *data = *data << 1;              // Left shift by 1 to make space
        *data |= (image_buffer[i] & 1);  // Extract LSB and set it in *data
    }
}
