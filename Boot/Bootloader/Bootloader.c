#include "Stdint.h"
#include "Memory.h"
// Compile this with m16!!! Please


// There are still 2044 bytes left in this structure
typedef volatile struct _InfoTableType_ {

  MemoryMapEntryStruct    MemoryMap[128];
  uint32                  MemoryMapEntryCount;

} __attribute__((packed)) InfoTableType;

volatile struct _Terminal_ {

  uint16                  X;
  uint16                  Y;
  uint16                  Max_X;
  uint16                  Max_Y;

} Terminal;

int Strlen(char* String) {

  int Length = 0;
  while (String[Length] != '\0') Length++;
  return Length;

}

void Putchar(char Character, uint8 Color) {

  *((uint16*)0xB8000+(Terminal.X)+(Terminal.Y*80)) = Character | Color << 8;

  Terminal.X++;

  if (Terminal.X >= Terminal.Max_X) {

    Terminal.X = 0;
    Terminal.Y++;

  }

  if (Terminal.Y >= Terminal.Max_Y) {

    // Todo: Scrolling, also basically everything as well

  }

}

void Print(char* String, uint8 Color) {

  for (int i = 0; i < Strlen(String); i++) {

    Putchar(String[i], Color);

  }
}

void Bootloader(void) {

  // Allocate up to 5KiB space for the InfoTable struct at EA00h in memory, up to FE00h, and reset the table.

  InfoTableType *InfoTable = (InfoTableType*)0xEA00;
  Memset((void*)InfoTable, 0, 5120);

  // Initialize the Terminal table, which is used for storing terminal data, and clear out the terminal.
  // Assuming a 80x25 text mode here.

  Terminal.X = 0;
  Terminal.Y = 0;
  Terminal.Max_X = 80;
  Terminal.Max_Y = 25;

  Memset((void*)0xB8000, 0, (80*25*2));

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

  Print("Test message", 0x0F);

  for(;;);

}
