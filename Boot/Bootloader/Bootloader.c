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



// There are still 2044 bytes left in this structure
typedef volatile struct _InfoTableType_ {

  MemoryMapEntryStruct    MemoryMap[128];
  uint32                  MemoryMapEntryCount;

} __attribute__((packed)) InfoTableType;



void Bootloader(void) {

  // Allocate up to 5KiB space for the InfoTable struct at EA00h in memory, up to FE00h, and reset the table.

  InfoTableType *InfoTable = (InfoTableType*)0xEA00;
  Memset((void*)InfoTable, 0, 5120);

  // Initialize the Terminal table, which is used for storing terminal data, and clear out the terminal.
  // Assuming a VGA 80x25 text mode here.

  InitializeTerminal(80, 25, 2, 0xB8000);

  // Use the BIOS call int 15h e820h to get a memory map of the system, with up to 128 entries.

  for (uint32 i = 0; i < 128; i++) {
    i = GetMemoryMapEntry(&InfoTable->MemoryMap[i], i);
    if (i == uint_max) break; // Todo: Crash
    InfoTable->MemoryMapEntryCount = i;
    if (i == 0) break;

  }

  // Clear nonexistent entries (where the size is 0).

  for (uint32 i = 0; i <= InfoTable->MemoryMapEntryCount; i++) {
    if ((InfoTable->MemoryMap[i].LowEntryLength = 0) && (InfoTable->MemoryMap[i].HighEntryLength = 0)) {

        Memset((void*)&InfoTable->MemoryMap[i], 0, 24);
      }

  }

  // Join any adjacent entries with the same size.

  for (uint32 i = 0; i <= InfoTable->MemoryMapEntryCount; i++) {

    if (InfoTable->MemoryMap[i].Type != InfoTable->MemoryMap[(i+1)].Type) continue;

    uint32 MemoryMapCurrentLowEnd = InfoTable->MemoryMap[i].LowBaseAddress + InfoTable->MemoryMap[i].LowEntryLength;
    uint32 MemoryMapCurrentHighEnd = InfoTable->MemoryMap[i].HighBaseAddress + InfoTable->MemoryMap[i].HighEntryLength;
    uint32 MemoryMapNextLowStart = InfoTable->MemoryMap[(i+1)].LowBaseAddress;
    uint32 MemoryMapNextHighStart = InfoTable->MemoryMap[(i+1)].HighBaseAddress;

    if (InfoTable->MemoryMap[i].HighBaseAddress > 0) {

    } else {

    }

  }

  Print("You are in the 2nd stage bootloader.\n\r\n\rNewline (without carriage return):\n", 0x0F); Print("*", 0x09);
  Print("\n\rNewline and carriage return:\n\r", 0x0F); Print("*", 0x09);
  Print("\n\r\n\rTab size is 2.\n\r0 tabs\n\r\t1 tab\n\r\t\t2 tabs\n\r\t\t\t3 tabs\n\r", 0x0F);
  Print("\n\rSupports ", 0x0F); Print("pretty ", 0x1F); Print("colours", 0x3F); Print(", and ", 0x0F);
  Print("high bit colours", 0x9F); Print(" as well.", 0x0F);
  Print("\n\r\n\rTODO: Add a crash screen, make it so that it crashes when it can't call E820 or when it doesn't have enough RAM, and finish filtering out the E820 results.     Don't bother with making sure your code only runs with -m16, it's impossible.", 0x03);
  Print("\n\r15:47 17th April 2022 UTC+1", 0x30);
  for(;;);

  // Note: The bootloader should be named Levada, and the project/OS itself should be named Ribeira
  // Delete this if you ever make it public

}
