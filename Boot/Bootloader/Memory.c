#include "Stdint.h"

/*  Our bootloader can access BIOS functions, one of the most important of which is int 15h eax e820h. This function
    allows you to obtain the system's memory map, which is a map of the RAM in your system. It shows what areas can
    and can not be used. This function is also generally present on all, or at least most i686/P6 systems. */



/*  MemoryMapEntryStruct: This is a struct that defines an int 15h, eax e820h memory map entry. You can also convert
    other memory map entry types (for example, from int 15h, ax e801h) to this type of entry.

    LowBaseAddress, HighBaseAddress           - This is the physical base address of the memory map entry. It's
                                              64 bits wide, but in 16-bit real mode we can only handle variables
                                              up to 32 bits (as long as we're on a 386), so it's split into two.

    LowEntryLength, HighEntryLength           - This is the length of the memory map entry in bytes. For example,
                                              an entry that starts at 10000h with a length of 20000h goes from
                                              10000h to 30000h in memory. This is also split into two.

    Type                                      - This is the type of the memory map entry. With the e820h BIOS
                                              function, type 1 is usable, type 2 and 4 is reserved, type 3 is
                                              reclaimable memory from ACPI, type 5 is bad memory, type 6 is
                                              disabled memory, type 7 is persistent memory, and any other type
                                              is undefined, or varies between OEMs. You should only use type 1.

    UnusedAcpi                                - This is unused on most systems, but some systems with ACPI use
                                              this field. It contains attributes specific to ACPI that give more
                                              info about the entry. However, this isn't used, as it doesn't give
                                              us any relevant info.

    It's set as volatile and packed, to keep GCC from interfering with it. Packed means that the contents of the
    table should be packed closely together, without padding, and volatile alerts the compiler to the fact that
    the content in the table may change.                                                                              */

typedef volatile struct _MemoryMapEntryStruct_ {

  uint32 LowBaseAddress;
  uint32 HighBaseAddress;
  uint32 LowEntryLength;
  uint32 HighEntryLength;
  uint32 Type;
  uint32 UnusedAcpi;

} __attribute__((packed)) MemoryMapEntryStruct;



/*  GetMemoryMapEntry(): This function calls the BIOS function int 15h eax e820h to obtain a memory map entry.

    Input:        struct MemoryMapEntryStruct - This is a pointer to the memory map entry you want to write. For
                                              example, if you had a single entry you wanted to fill out named
                                              "MapEntry", you'd pass &MapEntry, or MapEntry[n] if it was an array
                                              of entries.

    Input/Output: uint32 EntryNum           - This is the entry number or count that you're currently at. If you're
                                              running this for the first time, start at 0. After execution, this
                                              changes to the same value as the output. If this returns 0, stop:
                                              that means it's finished. If this returns FFFFFFFFh, that means it's
                                              not supported.

    This function calls the BIOS function int 15h, eax e820h, which returns a memory map in the format of the
    MemoryMapEntryStruct table. You call this function with the

    The BIOS function takes a few operands; eax must be E820h, ebx must be the entry number, es:di must be the address
    of the buffer where the table is stored, ecx must be size of that buffer, and edx must be 534D4150h,
    which is ASCII for 'SMAP'.

    Then, when the BIOS call is finished, if it was done correctly, eax should contain the 'SMAP' signature, and
    ebx should contain the next entry number to get an entry for, or 0 if this was the last entry. Between calls,
    es:di is not incremented; you have to manually do that yourself. If FFFFFFFFh (uint_max) is returned, that means
    e820 is not supported.                                                                                            */

uint32 __attribute__((noinline)) GetMemoryMapEntry(MemoryMapEntryStruct* Entry, volatile uint32 EntryNum) {

  static uint32 IntCall = 0x0000E820;
  static uint32 Signature = 0x534D4150;
  static uint32 EntrySize = 24;

  asm volatile( "int $0x15" :
                "=a"  (Signature),  "=b"  (EntryNum) :
                "a"   (IntCall),    "b"   (EntryNum), "c" (EntrySize), "d" (Signature), "D" (Entry));

  if (Signature != 0x534D4150) {
    return uint_max;
  } else {
    return EntryNum;
  }

}



/*  Memset(): This function writes over an area of memory.

    Input/Output: void* Address               - This specifies the base memory address you want to start writing to.

    Input:        uint8 Value               - This specifies the value you want to write to your memory area with.

    Input:        unsigned long Size          - This specifies how many bytes after the initial memory address you also
                                              want to write to; essentially, this is the size of the memory area
                                              you want to write to.

    This function writes to an area in memory. It writes a Size amount of bytes from Address with Value.
    An example of using this could be a buffer at memory location 5000h that's 32 bytes long, and that needs to be
    cleared out; you could just call Memset(0x5000, 0, 32).                                                           */

void* Memset(void* Address, uint8 Value, unsigned long Size) {

  for (unsigned long i = 0; i < Size; i++) {

    ((uint8*)Address)[i] = Value;
  }

  return Address;

}



/*  Memcpy(): This function copies an area of memory to another memory location.

    Input/Output: void* DestinationAddress    - This specifies the destination address that you want to copy to.

    Input:        void* SourceAddress         - This specifies the source address that you want to copy from.

    Input:        unsigned long Size          - This specifies how many bytes after the source address you also want
                                              to copy from; essentially, this is the size of the memory area you want
                                              to copy.

    This function copies from one area in memory to another area; it copies a Size amount of bytes from SourceAddress
    to DestinationAddress. This can be used, for example, to copy a framebuffer to VRAM; imagine that you have a
    128KiB image that's completely loaded into memory, and you want to copy it to the start of your VESA framebuffer.
    You could simply call something like Memset(&VesaFramebuffer, &Image, 131072).                                    */

void* Memcpy(void* DestinationAddress, void* SourceAddress, unsigned long Size) {

  unsigned long i;
  for (i = 0; i < Size; i++) {

    ((uint8*)DestinationAddress)[i] = ((uint8*)SourceAddress)[i];
  }

  return DestinationAddress;

}



/*  Memcmp(): This function compares two areas in memory.

    Input:        void* Address2              - This is the base address of the area of memory that will be compared
                                              with the area of memory at Address1.

    Input:        void* Address1              - This is the base address of the area of memory that will be compared
                                              with the area of memory at Address2.

    Input:        unsigned long Size          - This is the size, in bytes, of the memory areas that will be compared.

    Output:       int                         - This is the return value. It returns 0 if the chosen memory areas are
                                              identical; otherwise, it returns -+1.

    This function compares two areas in memory; it compares a Size amount of bytes from Address1 to a Size amount of
    bytes from Address2, and returns 0 if the two are equal, and -+1 if they are not. This can be used, for example,
    to compare two tables of the same type, and see if the content in them is equal, by calling something like
    Memcmp(&Table2, &Table1, (sizeof(TableType))).                                                                    */

int Memcmp(void* Address2, void* Address1, unsigned long Size) {

  unsigned long i;
  for (i = 0; i < Size; i++) {

    if (((uint8*)Address1) < ((uint8*)Address2)) return 1;
    if (((uint8*)Address2) < ((uint8*)Address1)) return -1;

  }

  return 0;

}
