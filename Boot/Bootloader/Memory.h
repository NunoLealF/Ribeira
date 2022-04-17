/* Ribeira | Written in 2022 by NunoLealF
   To the extent possible under law, NunoLealF has waived all copyright and related or neighboring rights to this
   software to the public domain worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with this software.
   If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. */

#ifndef _MEMORY_H_
#define _MEMORY_H_

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
void* Memcpy (void* restrict DestinationAddress, const void* restrict SourceAddress, unsigned long Size);
void* Memmove(void* restrict DestinationAddress, const void* restrict SourceAddress, unsigned long Size);
int   Memcmp (void* Address2, void* Address1, unsigned long Size);

#endif
