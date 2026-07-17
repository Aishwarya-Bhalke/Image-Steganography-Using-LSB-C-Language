//NAME : AISHWARYA B BHALKE
//BATCH ID : 25012A_096
//DATE OF SUBMISSION : 31 AUGUST 2025
//PROJECT NAME : LSB STEGANOGRAPHY

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "common.h"
#include "types.h"

// Function to get BMP image size in bytes (width × height × 3)
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    // Seek to  18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);
    printf("\n");
    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

// Get size of any file in bytes
uint get_file_size(FILE *fptr)
{
    if(fptr==NULL)
    return e_failure;
    fseek(fptr, 0, SEEK_END);
    uint size = ftell(fptr);
    rewind(fptr);
    return size;
}

// Read and validate input arguments for encoding operation
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    printf("Validating encode arguments.\n\n");
    // Validate source image is a BMP
    if (strstr(argv[2], ".bmp"))
    {
        encInfo->src_image_fname = argv[2];
        printf("Source image file validated : %s.\n\n",argv[2]);
    }
    else
    {
        fprintf(stderr, "ERROR: %s is not a valid BMP file.\n\n", argv[2]);
        return e_failure;
    }
    // Validate secret file has valid extension (.txt, .c, .cpp, .sh)
    if (strstr(argv[3], ".txt") || strstr(argv[3], ".c") || 
        strstr(argv[3], ".cpp") || strstr(argv[3], ".sh"))
    {
        encInfo->secret_fname = argv[3];
        printf("Secret file validated : %s.\n\n",argv[3]);

        // Extract file extension
        
        char *extn = strchr(argv[3], '.');
        if (!extn || strlen(extn) > MAX_FILE_SUFFIX)
        {
            fprintf(stderr, "ERROR: Invalid or too long extension in secret file.\n\n");
            return e_failure;
        }

        // Store extension in encode info
        strcpy(encInfo->extn_secret_file, extn);
    }
    else
    {
        fprintf(stderr, "ERROR: %s is not a supported secret file. Use .txt, .c, .cpp, or .sh\n\n", argv[3]);
        return e_failure;
    }
    // Output file: stego image
    if (!argv[4])
    {
        // Use default name if not provided
        encInfo->stego_image_fname = "stego.bmp";
        printf("Output file not mentioned. Creating %s as default.\n\n", encInfo->stego_image_fname);
    }
    else
    {
        char *ext = strrchr(argv[4], '.');
        if (ext != NULL && strcmp(ext, ".bmp") == 0)
        {
            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            fprintf(stderr, "ERROR: %s is not a valid output BMP file.\n\n", argv[4]);
            return e_failure;
        }
    }
   //encode validated successfully
    printf("All encode arguments validated successfully.\n\n");
    printf("-------Started encoding---------\n\n");
    return e_success;
}

// Function to open source image, secret file, and stego image for writing
Status open_files(EncodeInfo *encInfo)
{
    // Open source BMP image in binary read mode
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n\n", encInfo->src_image_fname);
        return e_failure;
    }
    // Open secret file in read mode
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n\n", encInfo->secret_fname);
        return e_failure;
    }
    // Open destination BMP file for stego image in binary write mode
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n\n", encInfo->stego_image_fname);
        return e_failure;
    }
    printf("Open file is successfull\n\n");
    return e_success;
}

// Check if the image has enough capacity to encode all required data
Status check_capacity(EncodeInfo *encInfo)
{
    // Get capacity of image in bytes (R+G+B)
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    // Get size of secret file
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    // Get extension (e.g., ".txt")
    char *extn = encInfo->extn_secret_file;

    // Calculate total bytes needed to encode
    int total_bytes_to_encode = 54 + ((
        strlen(MAGIC_STRING) +   // 2 bytes for magic string
        sizeof(int) +            // 4 bytes: extension size
        strlen(extn) +           // 4 bytes: extension itself
        sizeof(int) +            // 4 bytes: size of secret file
        encInfo->size_secret_file // N bytes: actual file content
    )*8);

    // Check if capacity is sufficient
    if (encInfo->image_capacity >= total_bytes_to_encode)
    {
        printf("Sufficient capacity in the image for encoding.\n\n");
        printf("Check capacity is successfull !!!\n\n");
        return e_success;
    }
    else
    {
        fprintf(stderr, "ERROR: Insufficient image capacity.\nRequired: %d bytes, Available: %d bytes\n\n",
                total_bytes_to_encode, encInfo->image_capacity);
        return e_failure;
    }
}

// Copy first 54 bytes (BMP header) from source image to stego image
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];
    rewind(fptr_src_image);

    // Read 54-byte BMP header
    size_t read_bytes = fread(buffer, 1, 54, fptr_src_image);

    // Write header to stego image
    size_t written_bytes = fwrite(buffer, 1, 54, fptr_dest_image);

    // Ensure all 54 bytes are copied
    if (read_bytes != 54 || written_bytes != 54)
    {
        fprintf(stderr, "Error copying BMP header. Read: %zu, Written: %zu\n\n", read_bytes, written_bytes);
        return e_failure;
    }

    printf("BMP header copied successfully: %zu bytes.\n\n", written_bytes);
    return e_success;
}

// Encodes the magic string into the stego image using LSB technique
Status encode_magic_string(const char *magic_str, FILE *src_image, FILE *stego_image)
{
Status status = encode_data_to_image ((char*)magic_str,strlen(magic_str), src_image, stego_image);
    if (status == e_success)
        printf("Magic string encoded up to byte offset: %ld in stego image.\n\n", ftell(stego_image));
    else
        fprintf(stderr, "Magic string encoding failed.\n\n");

    return status;
}

// Encodes the size of the secret file extension (e.g., 4 for ".txt") into the image
Status encode_secret_file_extn_size(int extn_size, EncodeInfo *encInfo)
{
    char image_Buffer[32];

    // Read 32 bytes from source image
    int byteread=fread(image_Buffer, 1, 32, encInfo->fptr_src_image);
    if(byteread!=32)
    {
        fprintf(stderr, "Error reading from source image at byte \n\n");
        return e_failure;
    }

    // Encode the extension size (integer) into 32 bits
    encode_size_to_lsb(extn_size, image_Buffer);

    // Write back modified 32 bytes to destination file
    int bytewrite=fwrite(image_Buffer, 1, 32, encInfo->fptr_stego_image);
    if(bytewrite!=32)
    {
        return e_failure;
    }

    printf("Extension size encoded successfully\n\n");

    return e_success;
}

// Encodes the file extension (e.g., ".txt") of the secret file into the image
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    for (int i = 0; i < strlen(file_extn); i++)
    {
        char image_Buffer[8];

        // Read 8 bytes from image
        fread(image_Buffer, 1, 8, encInfo->fptr_src_image);

        // Encode one character of extension
        encode_byte_to_lsb(file_extn[i], image_Buffer);

        // Write back the modified buffer to destination
        fwrite(image_Buffer, 1, 8, encInfo->fptr_stego_image);
    }

    printf("Secret file extension encoded successfully \n\n");
    return e_success;
}

// Encodes the size of the secret file (in bytes) into the image
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char image_Buffer[32];

    // Read 32 bytes from source image
    fread(image_Buffer, 1, 32, encInfo->fptr_src_image);

    // Encode the file size
    encode_size_to_lsb(file_size, image_Buffer);

    // Write back the encoded buffer
    fwrite(image_Buffer, 1, 32, encInfo->fptr_stego_image);

    printf("Secret file size encoded successfully\n\n");
    return e_success;
}

// Encodes the actual content of the secret file into the image
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch, image_Buffer[8];
    long bytes_encoded = 0;

    // Read secret file byte by byte
    while (fread(&ch, 1, 1, encInfo->fptr_secret))
    {
        // Read 8 bytes from source image
        fread(image_Buffer, 1, 8, encInfo->fptr_src_image);

        // Encode the byte into image LSBs
        encode_byte_to_lsb(ch, image_Buffer);

        // Write modified buffer to stego image
        fwrite(image_Buffer, 1, 8, encInfo->fptr_stego_image);
        bytes_encoded = bytes_encoded + 8;
    }

    printf("Secret data encoded  from source image.\n\n");
    printf("Encoding secret file data successfully\n\n");

    return e_success;
}

// Copies the remaining part of the BMP image that was not used for encoding
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    long bytes_copied = 0;

    // Copy rest of the image byte-by-byte
    while (fread(&ch, 1, 1, fptr_src))
    {
        fwrite(&ch, 1, 1, fptr_dest);
        bytes_copied++;
    }

    printf("Remaining image data copied successfully\n\n");
    return e_success;
}

// Encodes a sequence of bytes (data) into the image LSBs
Status encode_data_to_image(char *data, int size, FILE *src_image, FILE *stego_image)
{
    char buffer[8];

    for (int i = 0; i < size; i++)
    {
        // Read 8 bytes from source image
        fread(buffer, 1, 8, src_image);

        // Encode one byte of data into the 8 image bytes
        encode_byte_to_lsb(data[i], buffer);

        // Write the modified bytes to stego image
        fwrite(buffer, 1, 8, stego_image);
    }
    return e_success;
}

// Encodes a single byte of data into the least significant bit of 8 image bytes
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        // Extract bit from data
        int bit = (data >> (7 - i)) & 1;

        // Set the LSB of image_buffer[i] to the bit
        image_buffer[i] = (image_buffer[i] & ~1) | bit;
    }
    return e_success;
}


// Encodes a 32-bit integer (like file size or extension size) into the LSBs of 32 image bytes
Status encode_size_to_lsb(int data, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        int bit = (data >> (31 - i)) & 1;
        image_buffer[i] = (image_buffer[i] & ~1) | bit;
    }
    return e_success;
}
// Perform the full encoding workflow
Status do_encoding(EncodeInfo *encInfo)
{
    int extn_size;

    // Step 1: Open all required files
    if (open_files(encInfo) != e_success)
        return e_failure;

    // Step 2: Check if the image has enough space to store data
    if (check_capacity(encInfo) != e_success)
        return e_failure;

    // Step 3: Copy BMP header (first 54 bytes)
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
        return e_failure;

    // Get size of the file extension string
    extn_size = strlen(encInfo->extn_secret_file);

    // Step 4: Encode magic string
    if (encode_magic_string(MAGIC_STRING, encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
        return e_failure;

    // Step 5: Encode extension size, extension, file size, and then the secret file data
    if(encode_secret_file_extn_size(extn_size, encInfo)!= e_success)
       return e_failure;

    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) != e_success)
       return e_failure;

    if(encode_secret_file_size(encInfo->size_secret_file, encInfo)!=e_success)
       return e_failure;

    if(encode_secret_file_data(encInfo)!= e_success)
       return e_failure;


    // Step 6: Copy remaining image data from source to stego
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)!= e_success)
        return e_failure;
    
    return e_success;
}






