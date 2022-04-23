# This is the Makefile for the bootloader.
# To execute this from the main folder, use "make -C Boot/".

# You must use nasm and a gcc cross compiler designed for the i686-elf-gcc target.
# This makefile requires nasm, i686-elf-gcc and objcopy to be in your path to compile and build this.
# You also need dd and rm, but if your system does not have these commands, you can replace them with your own versions.

AS = nasm
CC = i686-elf-gcc


# Our bootloader has two sections; the Bootsector (the 1st stage bootloader), which is compiled with nasm, and the
# 'Bootloader' (the 2nd stage bootloader) in C99 standard C with GNU extensions ('gnu99'). The 1st stage bootloader is
# located in the boot sector, which is the first (512-byte) sector of the medium you're trying to load the bootloader
# off of. It contains a skeleton for a future BPB implementation, and loads the 2nd stage bootloader.

# The 2nd stage bootloader depends on the 'Bootloader.ld' linker file to successfully compile it. We compile our 2nd
# stage bootloader and any associated files with the following arguments:

# -ffunction-sections						- This splits up every function into a separate section in the object file, which
#																is needed so that we can place the main function at the beginning.
#
# -ffreestanding								- This limits the available functions to only what is available in a freestanding
#																environment, which is necessary as our bootloader can't rely upon an OS for some things.
#
# -fno-builtin									- Under a normal environment, gcc replaces implementations of common functions like itoa
#																with its own versions, which is not preferable in a freestanding environment.
#
# -std=gnu99										- Compile this with the C99 standard, with GNU extensions ('gnu99'). This version
#																of C works well and is compatible with pretty much anything.
#
# -m16													- The 2nd stage bootloader runs in 16-bit real mode, so we have to make sure we're
#																compiling for a 16-bit environment.
#
# -Wall, -Wextra, pedantic			- This enables all warnings. By default, gcc doesn't warn you about everything, and
#																enabling all warnings helps you keep track of any possible mistakes.
#
# -funsigned-char								- Changes the default char type to unsigned, so that strings can use extended ASCII
#																characters.

LDFLAGS = -T Bootloader/Bootloader.ld
CFLAGS = -ffunction-sections -ffreestanding -fno-builtin -std=gnu99 -m16 -Wall -Wextra -pedantic -funsigned-char


# The .PHONY directive is used on targets that don't output anything. For example, running 'make all' builds our
# bootloader, but it doesn't output any specific files; it just goes through a lot of targets; the target that builds
# the final output isn't 'all', it's 'Boot.bin'. If Make sees that something is already there when executing a target,
# it skips it (for example, for the target 'example.o', if it sees example.o is already there, it skips compiling it),
# and this can cause problems for targets that don't output anything. These are called 'phony targets'.

.PHONY: All Clean CleanObj CleanBin Run all clean run


# People will typically run 'make all', 'make clean', etc. in the command line, but Make is case sensitive and those
# targets are not all lowercase.

all: All
allrun: AllRun
clean: Clean
run: Run


# This target compiles the bootsector with nasm, and outputs it as a flat binary file in the Bootsector folder.
# Our 1st stage bootloader loads our 2nd stage bootloader into memory, at the memory location 7E00h.

Bootsector/Bootsector.bin:
	@echo "Building $@"
	@$(AS) Bootsector/Bootsector.asm -f bin -o Bootsector/Bootsector.bin


# The following targets compile the source files from the 2nd stage bootloader into object files. By this stage, they
# aren't linked yet, that'll happen later.

Bootloader/Bootloader.o:
	@echo "Building $@"
	@$(CC) $(CFLAGS) -c Bootloader/Bootloader.c -o Bootloader/Bootloader.o

Bootloader/Memory.o:
	@echo "Building $@"
	@$(CC) $(CFLAGS) -c Bootloader/Memory.c -o Bootloader/Memory.o

Bootloader/Graphics.o:
	@echo "Building $@"
	@$(CC) $(CFLAGS) -c Bootloader/Graphics.c -o Bootloader/Graphics.o

# This target compiles all the object files from the 2nd stage bootloader into one flat binary file. It references a
# linker file, which puts the main function at the start (which is needed for a flat binary file), and also affirms
# that the start of execution is at 7E00h in memory, which is where our 2nd stage bootloader is loaded. First, we
# link it into an ELF object file, and then we transform that into a flat binary file with objcopy. There is a method
# to do this in gcc, but it might be unstable.

Bootloader/Bootloader.bin: Bootloader/Bootloader.o Bootloader/Memory.o Bootloader/Graphics.o
	@echo "Building $@"
	@$(CC) $(LDFLAGS) -o Bootloader/Bootloader.elf -ffreestanding -nostdlib $^ -lgcc
	@objcopy -O binary Bootloader/Bootloader.elf Bootloader/Bootloader.bin


# This target creates a 32 KiB image that contains both our 1st and 2nd stage bootloader. You can burn this image onto
# any bootable medium. It writes Bootsector.bin (the bootsector, or our 1st stage bootloader) into the first sector,
# and Bootloader.bin (our 2nd stage bootloader) into the next 47 sectors. Bootloader.bin must not be larger than 48
# sectors, or 24KiB. The last 16 sectors (8KiB) are reserved for storage for things like the E820 memory map, which
# is loaded by the rest of the bootloader into memory.
# This function uses dd, so it may not work on Windows.

Boot.bin: Bootsector/Bootsector.bin Bootloader/Bootloader.bin
	@echo "Building $@"
	@dd if=/dev/zero of=Boot.bin bs=512 count=64 status=none
	@dd if=Bootsector/Bootsector.bin of=Boot.bin conv=notrunc bs=512 count=1 status=none
	@dd if=Bootloader/Bootloader.bin of=Boot.bin conv=notrunc bs=512 count=57 seek=1 status=none


# The All target cleans out the binary files from the previous build, and rebuilds the bootloader. The AllRun target
# does the same as the All target, but it also runs it with Qemu.

All: CleanBin Bootsector/Bootsector.bin Bootloader/Bootloader.o Bootloader/Memory.o Bootloader/Graphics.o Bootloader/Bootloader.bin Boot.bin CleanObj
AllRun: All Run


# The Clean target cleans both the object and binary files left out by the build process.

Clean: CleanObj CleanBin


# The CleanObj target cleans all the object (*.o) and ELF object (*.elf) files from the folders that 'produce' them.
# This function uses rm, so it may not work on Windows.

CleanObj:
	@echo "Deleting all *.o and *.elf files."
	@-rm -f Bootloader/*.o
	@-rm -f Bootloader/*.elf


# The CleanBin target cleans all the binary (*.bin) files from the folders that 'produce' them.
# This function uses rm, so it may not work on Windows.

CleanBin:
	@echo "Deleting all *.bin files."
	@-rm -f Boot.bin
	@-rm -f Bootsector/*.bin
	@-rm -f Bootloader/*.bin


# The Run target runs the bootloader file (Boot.bin) with Qemu, configured to emulate a Pentium II machine with 32 MB
# of RAM, with the bootloader running off of a hard drive. On some systems, this may not be supported, so you can run
# it without the CPU or RAM limitation.

Run:
	@echo "Running with Qemu."
	@qemu-system-i386 -cpu pentium2 -m 32 -drive file=Boot.bin,format=raw
