/* Ribeira | Written in 2022 by NunoLealF
   To the extent possible under law, NunoLealF has waived all copyright and related or neighboring rights to this
   software to the public domain worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with this software.
   If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. */

// WARNING! You must compile this and any other files from this 2nd stage bootloader with -m16!

#include "Stdint.h"
#include "Memory.h"
#include "Graphics.h"

#ifndef __i686__
#error  "You must compile this on a cross-compiler with an i686 target."
#endif

// 8192 bytes
// L/H Signature: 8 bytes    (8184 bytes remaining), 8
// Memorymap:     3072 bytes (5112 bytes remaining), 3080
// Memorymap-ec:  4 bytes (5108 bytes remaining), 3084

// ! KEEP IN MIND !
// LOWSIGNATURE = 0x333C6557
// HIGHSIGNATURE = 0x31323665

typedef volatile struct _InfoTableType_ {

  uint32                  LowSignature;
  uint32                  HighSignature;
  MemoryMapEntryStruct    MemoryMap[128];
  uint32                  MemoryMapLastEntry;

} __attribute__((packed)) InfoTableType;



void Crash(char* Why) {

  Print("\n\n\rUnable to continue booting. Reason given:\n\r", 0x0C);
  Print(Why, 0x07);
  Print("\n\n\rTo restart your system, press Ctrl+Alt+Delete.", 0x0F);

  for(;;);

}



void Bootloader(void) {

  // Allocate up to 8KiB space for the InfoTable struct at E000h in memory, up to FFFFh, and initialize the table.

  InfoTableType *InfoTable = (InfoTableType*)0xE000;

  Memset((void*)InfoTable, 0, 8192);

  InfoTable->LowSignature  = 0x333C6557;
  InfoTable->HighSignature = 0x31323665;

  // Initialize the Terminal table, which is used for storing terminal data, and clear out the terminal.
  // Assuming a VGA 80x25 text mode here.

  InitializeTerminal(80, 25, 2, 0xB8000);

  // Use the BIOS call int 15h e820h to get a memory map of the system, with up to 128 entries.

  Print("Getting the system memory map.\n\r", 0x0F);

  for (int i = 0; i < 128; i++) {

    uint32 MemoryMapReturnValue = GetMemoryMapEntry(&InfoTable->MemoryMap[i], i);
    InfoTable->MemoryMapLastEntry = i;

    if (MemoryMapReturnValue == 0) {

      break;

    } else if (MemoryMapReturnValue == uint_max) {

      Crash("Unable to get a memory map using the E820 BIOS function.\n\r"
            "Your system you should probably use a form of struct inheritence (c -style) to make this a little easier for everyone to figure outlikely doesn't meet the minimum requirements."); break;

    }

  }

  // Warning: Literally all the code in this function and like half of the code otherwise in this file is incomplete

  int freeram = 0;

  for (uint32 i = 0; i <= InfoTable->MemoryMapLastEntry; i++) {
    if (InfoTable->MemoryMap[i].Type != 1) continue;
    freeram += InfoTable->MemoryMap[i].LowEntryLength;
  }

  freeram /= 1024;

  char buffer[32]; Memset(buffer, 0, 32);
  char thing[9]; Memcpy(thing, (void*)&InfoTable->LowSignature, 8); thing[9] = '\0';

  Print("Test: Bootloader is at ", 0x0F);
  Itoa((unsigned long)&Bootloader, buffer, 16);
  Print(buffer, 0x07); Print(".\n\rInfoTable->LowSignature is at ", 0x0F);
  Print(Itoa((unsigned long)&InfoTable->LowSignature, buffer, 16), 0x07);
  Print(".\n\rInfoTable->MemoryMapLastEntry is at ", 0x0F); Print(Itoa((unsigned long)&InfoTable->MemoryMapLastEntry, buffer, 16), 0x07);
  Print(".\n\rTest 2: ", 0x0F); Print(Itoa(InfoTable->LowSignature, buffer, 16), 0x07); Putchar(' ', 0x0F); Print(Itoa(InfoTable->HighSignature, buffer, 16), 0x07);
  Print(" Ascii: ", 0x0F); Print(thing, 0x07); Print("\n\n\rRibeira project, Levada bootloader. Licensed as CC0.\n\r12:21 23 April 2022 UTC+1", 0x30);


  for(;;);


  // Note: The bootloader should be named Levada, and the project/OS itself should be named Ribeira
  // Delete this if you ever make it public

}
