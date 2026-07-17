//NAME : AISHWARYA B BHALKE 
//BATCH ID : 25012A_096
//DATE OF SUBMISSION : 31 AUGUST 2025
//PROJECT NAME : LSB STEGANOGRAPHY

#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    e_success,
    e_failure
} Status;

typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif
