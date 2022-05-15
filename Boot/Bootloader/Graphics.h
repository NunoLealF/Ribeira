/* Ribeira | Written in 2022 by NunoLealF
   To the extent possible under law, NunoLealF has waived all copyright and related or neighboring rights to this
   software to the public domain worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with this software.
   If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. */

// WARNING: This is 16-bit C code. You should compile this, along with any other files from the second stage
// bootloader, with the -m16 (or equivalent) flag.

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

typedef volatile struct _TerminalStruct_ {

  uint16                  X;
  uint16                  Y;
  uint16                  Max_X;
  uint16                  Max_Y;
  uint16                  TabSize;
  uint32                  Framebuffer;

} TerminalStruct;

extern TerminalStruct Terminal;

unsigned short Strlen(const char* String);

void InitializeTerminal(uint16 Rows, uint16 Columns, uint16 TabSize, uint32 Framebuffer);
void Putchar(const char Character, uint8 Color);
void Print(const char* String, uint8 Color);

char* Itoa(unsigned long Value, char* Buffer, unsigned short Base);

#endif
