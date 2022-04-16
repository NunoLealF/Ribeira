#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

typedef volatile struct     _MemoryMapEntryStruct_ {

  uint32 LowBaseAddress;
  uint32 HighBaseAddress;
  uint32 LowEntryLength;
  uint32 HighEntryLength;
  uint32 Type;
  uint32 UnusedAcpi;

} __attribute__((packed))   MemoryMapEntryStruct;

int __attribute__((noinline)) GetMemoryMapEntry(MemoryMapEntryStruct* Entry, volatile uint32 EntryNum);

void* Memset (void* Address, uint8 Value, unsigned long Size);
void* Memcpy (void* DestinationAddress, void* SourceAddress, unsigned long Size);
int   Memcmp (void* Address2, void* Address1, unsigned long Size);

#endif
