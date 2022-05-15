;  Ribeira | Written in 2022 by NunoLealF
;  To the extent possible under law, NunoLealF has waived all copyright and related or neighboring rights to this
;  software to the public domain worldwide. This software is distributed without any warranty.
;
;  You should have received a copy of the CC0 Public Domain Dedication along with this software.
;  If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

[ORG 0x7c00]
[BITS 16]


; These two instructions jump over the area reserved for the BIOS Parameter Block, which is explained later on.
; The standard is to do a short jump 118 (76h) bytes forward, and add a nop instruction. This is also known as
; 'EB 76 90'. The jump should arrive at SetCS.

jmp short 120
nop



;   BPB: This area contains important data regarding a FAT-style filesystem.
;
;   (No inputs or outputs)
;
;   The BIOS Parameter Block, or as it's more widely known, the BPB, is an area in the first sector of a
;   FAT-style partition. It contains important data about the partition, such as the name, the number of
;   bytes per sector, among other things. Most BPBs are under 80 bytes long, but the area reserved for the
;   BPB is 120 bytes long, including the short jump and nop at the start, which are 3 bytes.
;
;   The BPB here in the bootsector is designed to work with FAT32, to maintain compatibility with EFI
;   System Partitions (ESPs), which also use FAT32. It's also important to mention that the BPB includes a
;   bootable flag, at 0x42, and some BIOSes will not boot if this flag is not set, even with the bootable
;   flag the end of the bootsector.

BPB:

  times 0x3F db 0
  db 40
  times 0x35 db 0



;   SetCS: Does a long jump to the Start label, setting CS to 0.
;
;   (No inputs or outputs)
;
;   Some BIOSes jump into their bootloader with CS:IP set to 07C0:0000h. Depending on what you're doing,
;   this may not be an issue, but for a bootloader it is, as it's assumed that CS:IP is set to 0000:7C00h.
;   This label does a long jump to the Start label, setting CS:IP to 0000:7C00h.

SetCS:

  jmp 0:Start



;   Start: The beginning of the first-stage bootloader. Loads the next stage into memory.
;
;   Inputs:        uint32 <CS:IP>                    - CS:IP must be 0000:7C00h.
;
;   This label marks the start of the first-stage bootloader. In this environment, we are in 16-bit real mode,
;   with CS:IP set to 0000:7C00h, with DL set to the current drive number by our BIOS. All we'll do here is
;   set up the stack at 7B00h in memory (where it has nearly 30KiB of space), and load the second stage bootloader,
;   and if the latter fails, give out an error message.
;
;   The second stage bootloader, which is 48 sectors, or 24KiB long, will be loaded at 0x7E00 in memory, going
;   up to 0xDDFF in RAM. Finally, the BootTable, which contains the information gathered by the second stage
;   bootloader, is stored from 0xE000 to 0xFFFF, having roughly 8KiB of space.
;
;   It's loaded in with the BIOS function int 13h ah 02h, which loads AL sectors from the disk at DL, the head at
;   DH, the cylinder at CH and the sector at CL, into the memory location in ES:BX. Keep in mind that the sector
;   number starts at 1, while the disk, head and cylinder numbers start at 0.
;
;   DL is preserved throughout the whole file. This is because our BIOS sets DL to the current drive number, which
;   is the same drive number as the rest of our bootloader. For this reason, it's quite important.

Start:

  mov ax, 0
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, 0x7B00

  mov ah, 0x02
  mov al, 0x30
  mov bx, 0x7E00
  mov ch, 0x00
  mov cl, 0x02
  mov dh, 0x00

  int 0x13

  jc DiskLoadFail
  jnc 0x7E00



;   DiskLoadFail (and _Halt): Crash handler, if loading the rest of the bootloader failed.
;
;   (No inputs or outputs)
;
;   This function serves as a crash handler, for situations where loading the rest of the bootloader ended up
;   failing. Using the Print function, it prints out an error message, an error code of 1, and then it halts
;   the system.

DiskLoadFail:

  mov bx, 0
  mov cl, 0x0C
  mov si, ErrorMsg1 ; 1st line
  call Print

  mov bx, (34 * 2)
  mov cl, 0x0F
  mov si, ErrorCode ; 1st line
  call Print

  mov bx, (35 * 2)
  mov cl, 0x0C
  mov si, ErrorMsg2 ; 1st line
  call Print

  mov bx, (80 * 2)
  mov cl, 0x07
  mov si, ErrorMsg3 ; 2nd line
  call Print

  mov bx, ((80 * 3) * 2)
  mov cl, 0x0F
  mov si, ErrorMsg4 ; 3rd line
  call Print

  jmp _Halt

_Halt:

  hlt
  jmp _Halt



;   Print (and _PrintReturn): Prints a string to the terminal.
;
;   Input:        uint8 <BX>                         - The offset in memory that you want to write to the screen with.
;                                                    For example, if you wanted to write to the 2nd line, you'd
;                                                    want to start at an offset of 160, as each character is 2 bytes.
;
;   Input:        uint8 <CL>                         - This is the color attribute of the string you want to write.
;                                                    The highest four bits represent the background color, while the
;                                                    lower four bits represent the foreground color. The highest bit
;                                                    may be reserved as a 'blinking' attribute.
;
;   Input:        uint32* <DS:SI>                    - A pointer to the string you want to write to the screen.
;
;   This function writes a string onto the terminal. It takes in a pointer to the string, the offset in memory
;   to write that string with, and the color code you want to write the string with. It does not support scrolling,
;   newlines, or any other special feature.

Print:

  lodsb

  cmp al, 0
  jz _PrintReturn

  mov dx, 0xB800
  mov es, dx

  mov [es:bx], al
  mov [es:(bx+1)], cl

  add bx, 2

  jmp Print

_PrintReturn:

  ret


; These strings are to be displayed if loading the second-stage bootloader fails.

ErrorMsg1 db 'Unable to continue booting (Error ', 0
ErrorCode db '1', 0
ErrorMsg2 db '), halting the system. Reason given:', 0
ErrorMsg3 db 'Failed to load the second-stage bootloader. Try rebooting the system.', 0
ErrorMsg4 db 'To restart your system, press Ctrl+Alt+Delete.', 0

; This is a note to the assembler to tell it to zero out the rest of our bootsector, up to the 510th byte.
times 510-($-$$) db 0


; At the end of every bootsector is a signature that the firmware always checks for, which are the two bytes aa55h.
; Without those bytes, it won't load our bootsector. They're there so that the firmware knows it's not just loading
; random noise.

db 0x55
db 0xAA
