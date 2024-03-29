/* Ribeira | Written in 2022 by NunoLealF
   To the extent possible under law, NunoLealF has waived all copyright and related or neighboring rights to this
   software to the public domain worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with this software.
   If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. */

// WARNING: This is 16-bit C code. You should compile this, along with any other files from the second stage
// bootloader, with the -m16 (or equivalent) flag.

#ifndef _STDINT_H_
#define _STDINT_H_

typedef signed char    int8;
typedef unsigned char  uint8;
typedef signed short   int16;
typedef unsigned short uint16;
typedef signed long    int32;
typedef unsigned long  uint32;
typedef int            bool;

#define int_max  0xFFFFFFF
#define uint_max 0xFFFFFFFF

#define true     0
#define false    1

#endif
