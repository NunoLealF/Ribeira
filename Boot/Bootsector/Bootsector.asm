;  Ribeira | Written in 2022 by NunoLealF
;  To the extent possible under law, NunoLealF has waived all copyright and related or neighboring rights to this
;  software to the public domain worldwide. This software is distributed without any warranty.
;
;  You should have received a copy of the CC0 Public Domain Dedication along with this software.
;  If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

[ORG 0x7c00]
[BITS 16]

; The BIOS Parameter Block, or as it's more widely known, the BPB, is an area in the bootsector that's present
; on FAT-based filesystems, and it describes the partition it's on. It's usually, at most, 79 bytes long, but
; you're supposed to jump 118 bytes forward anyway, which is exactly what I expect from Microsoft engineers

; This BPB is designed to work with FAT32. The standard says that there should be a short jump 76h (118) bytes
; forward, a nop instruction, and then the content of your BPB.

; Normally, you should be able to ignore this, but some BIOSes don't boot a disk without it having a valid BPB,
; because non-FAT filesystems don't actually exist, and are just a myth written by Linus Torvalds

jmp short SetCS
nop



BPB:
  times 0x75 db 0



SetCS:
; Sets cs to 0000, as some BIOSes have it set at 07c0:0000.

  jmp 0:Start



Start:
; This function initializes ds and es, and then sets the stack location to 0000:7b00h, where it has about
; 30KB of space downwards up until 600h, which should be safe.

; After that, it executes BIOS function int 13h ah 02h, which reads sectors from the disk to the memory location
; at es:bx (which in this case is 0000:7e00h), which we'll use to load our 2nd stage bootloader.

; Then, we check if it loaded correctly; if it did, we'll jump to the new code. Otherwise, we'll just halt.
; This function also requires dl, but that's already set up correctly by our BIOS. This is why you also shouldn't
; overwrite or otherwise trash dl.

  mov ax, 0
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, 0x7B00

  mov ah, 0x02
  mov al, 0x35
  mov bx, 0x7E00
  mov ch, 0x00
  mov cl, 0x02
  mov dh, 0x00

  int 0x13

  jnc 0x7E00
  jc _DiskLoadFail



_DiskLoadFail:
; This function disables interrupts and halts the system, if it can't load the rest of the bootloader.

  cli
  hlt
  jmp _DiskLoadFail



; This is a note to the assembler to tell it to zero out the rest of our bootsector, up to the 510th byte.
times 510-($-$$) db 0



; At the end of every bootsector is a signature that the firmware always checks for, which are the two bytes aa55h.
; Without those bytes, it won't load our bootsector. They're there so that the firmware knows it's not just loading
; random code

db 0x55
db 0xAA
