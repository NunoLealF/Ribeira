/* Ribeira | Written in 2022 by NunoLealF
   To the extent possible under law, NunoLealF has waived all copyright and related or neighboring rights to this
   software to the public domain worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with this software.
   If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. */

// WARNING: This is 16-bit C code. You should compile this, along with any other files from the second stage
// bootloader, with the -m16 (or equivalent) flag.

#include "Stdint.h"
#include "Error.h"
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

typedef volatile struct _BootTableType_ {

  uint32                  LowSignature;
  uint32                  HighSignature;
  MemoryMapEntryStruct    MemoryMap[128];
  uint32                  MemoryMapLastEntry;

} __attribute__((packed)) BootTableType;



/*  Crash(): Crash handler for the system.

    Input:        unsigned long ErrorCode            - This specifies the error code to crash with.

    This function is a crash handler for the system, which should be called whenever there is a need to crash the
    system. It gives out the error code, along with the associated message (defined in Error.h), and halts the
    system. It doesn't disable interrupts, so you can still use Ctrl+Alt+Delete to restart the system in this state.
*/

void Crash(unsigned long ErrorCode) {

  char ErrorCodeString[8];
  Itoa(ErrorCode, ErrorCodeString, 10);

  Print("\n\n\rUnable to continue booting (Error ", 0x0C);
  Print(ErrorCodeString, 0x07);
  Print("), halting the system. Reason given:\n\r", 0x0C);

  Print(ErrorMessage[ErrorCode], 0x07);

  Print("\n\n\rTo restart your system, press Ctrl+Alt+Delete.", 0x0F);

  for(;;);

}



/*  Bootloader(): The second-stage system bootloader.

    (No inputs or outputs)

    This function is the second-stage bootloader. It handles many tasks, such as initializing the BootTable,
    initializing the terminal, gathering information from the system (such as the memory map), among others.
*/

void Bootloader(void) {

  // Allocate up to 8KiB space for the BootTable struct at E000h in memory, up to FFFFh, and initialize the table.

  BootTableType *BootTable = (BootTableType*)0xE000;

  Memset((void*)BootTable, 0, 8192);

  BootTable->LowSignature  = 0x333C6557;
  BootTable->HighSignature = 0x31323665;

  // Initialize the Terminal table, which is used for storing terminal data, and clear out the terminal.
  // Assuming a VGA 80x25 text mode here.

  InitializeTerminal(80, 25, 2, 0xB8000);

  // Use the BIOS call int 15h e820h to get a memory map of the system, with up to 128 entries.

  Print("Getting the system memory map.\n\r", 0x0F);

  for (int i = 0; i < 128; i++) {

    uint32 MemoryMapReturnValue = GetMemoryMapEntry(&BootTable->MemoryMap[i], i);
    BootTable->MemoryMapLastEntry = i;

    if (MemoryMapReturnValue == 0) {

      break;

    } else if (MemoryMapReturnValue == uint_max) {

      Crash(1); break;

    }

  }

  // Warning: Literally all the code in this function and like half of the code otherwise in this file is incomplete

  int freeram = 0; // THIS MEASURES RAM UNDER 4GB AND NOT EVEN PROPERLY

  for (uint32 i = 0; i <= BootTable->MemoryMapLastEntry; i++) {
    if (BootTable->MemoryMap[i].Type != 1) continue;
    freeram += BootTable->MemoryMap[i].LowEntryLength;
  }

  freeram /= 1024;

  char buffer[32]; Memset(buffer, 0, 32);
  char thing[9]; Memcpy(thing, (void*)&BootTable->LowSignature, 8); thing[9] = '\0';

  Print("Test: Bootloader is at ", 0x0F);
  Itoa((unsigned long)&Bootloader, buffer, 16);
  Print(buffer, 0x07); Print(".\n\rBootTable->LowSignature is at ", 0x0F);
  Print(Itoa((unsigned long)&BootTable->LowSignature, buffer, 16), 0x07);
  Print(".\n\rBootTable->MemoryMapLastEntry is at ", 0x0F); Print(Itoa((unsigned long)&BootTable->MemoryMapLastEntry, buffer, 16), 0x07);
  Print(".\n\rTest 2: ", 0x0F); Print(Itoa(BootTable->LowSignature, buffer, 16), 0x07); Putchar(' ', 0x0F); Print(Itoa(BootTable->HighSignature, buffer, 16), 0x07);
  Print(" Ascii: ", 0x0F); Print(thing, 0x07); Print("\n\n\rRibeira bootloader. Licensed as CC0.\n\n\rTODO:\n\r - Add support for the detection of, and enabling of the A20 Line, before E820\n\r(Challenges: We've got no machines with A20 off by default to test this out)\n\n\r - Add support for VBE\n\r(Challenges: Not sure yet, but don't set any modes in this stage yet)\n\n\rCPUID is only for protected mode, it won't work in real mode, trust me!\n\r19:04 15 May 2022 UTC+1", 0x9F);

  Crash(0);

  for(;;);


  // Note: The bootloader is called Ribeira


}
