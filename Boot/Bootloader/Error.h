/* Ribeira | Written in 2022 by NunoLealF
   To the extent possible under law, NunoLealF has waived all copyright and related or neighboring rights to this
   software to the public domain worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with this software.
   If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. */

// WARNING: This is 16-bit C code. You should compile this, along with any other files from the second stage
// bootloader, with the -m16 (or equivalent) flag.

#ifndef _ERROR_H_
#define _ERROR_H_

const char* ErrorMessage[] = {

  "No reason given, or invalid error code.", // 0

  "This error code should not happen here.", // 1

  "Failed to retrieve the system's memory map with the BIOS function \n\r" // 2
  "int 15h, ax e820h. This may happen if your machine is very old. \n\r" // 2
  "Make sure that your system meets the minimum requirements.", // 2

};

#endif
