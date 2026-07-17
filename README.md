# Image-Steganography-Using-LSB-C-Language

This project implements Image Steganography using the Least Significant Bit (LSB) technique to embed secret files inside 24-bit BMP images with minimal visual changes. The application provides a simple command-line interface (CLI) for both encoding and decoding while ensuring secure data extraction using a magic string.

✨ Features
Hide secret files inside 24-bit BMP images using LSB steganography.
Decode and recover the original hidden file.
Command-line interface for encoding and decoding.
Magic string verification for secure data extraction.
Automatic image capacity checking before encoding.
Supports different secret file extensions.
Modular and well-structured C implementation.


🛠 Technologies Used
Language: C
Compiler: GCC
Platform: Linux
Concepts: Bitwise Operations, File Handling, Memory Management, Command-Line Arguments


⚙️ Working
Encoding
Read the source BMP image.
Read the secret file.
Verify image capacity.
Embed the magic string, file extension, file size, and secret data into the image's least significant bits.
Generate the encoded (stego) image.
Decoding
Read the stego image.
Verify the magic string.
Extract the embedded metadata and secret data.
Reconstruct the original secret file.


▶️ Usage
Encode
./a.out -e input.bmp secret.txt output.bmp
Decode
./a.out -d output.bmp decoded_file

🚀 Future Improvements
Add password protection and encryption.
Support additional image formats such as PNG and JPEG.
Improve error handling and validation.
Develop a graphical user interface (GUI).

📚 Learning Outcomes
Understanding of image steganography using the LSB technique.
Practical experience with low-level programming in C.
Improved knowledge of bitwise manipulation and file operations.
Hands-on experience with command-line application development.
