/* Ribeira | Written in 2022 by NunoLealF
   To the extent possible under law, NunoLealF has waived all copyright and related or neighboring rights to this
   software to the public domain worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with this software.
   If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. */

#include "Stdint.h"
#include "Memory.h"

/*  TerminalStruct: This is a struct that defines the settings for the terminal. It is designed to work with a hardware
    text mode of any size, as long as it has a linear framebuffer.

    uint16 X                                         - The current row in the terminal. This is volatile.

    uint16 Y                                         - The current column in the terminal. This is volatile.

    uint16 Max_X, Max_Y                              - Number of rows and columns in the terminal. The last row would be
                                                     Max_X - 1., and the last column would be Max_Y - 1.

    uint16 TabSize                                   - How many spaces a tab occupies.

    uint32 Framebuffer                               - The location of the framebuffer, which must be linear. This is
                                                     not a pointer; it is only the memory address of the framebuffer.

    This struct contains the settings for a VGA text mode terminal. The default text mode is 80x25 with 8 or 16
    colors, and it is located at 0xB8000, so you'd fill it out as {0, 0, 80, 25, 2, 0xB8000}.

    This is not the best or the most efficient way to use the terminal, but this is only supposed to serve as a
    replacement until we switch to a graphics mode. It's possible to write to the terminal without relying upon
    this struct, but it's discouraged.

*/

typedef struct _TerminalStruct_ {

  uint16                  X;
  uint16                  Y;
  uint16                  Max_X;
  uint16                  Max_Y;
  uint16                  TabSize;
  uint32                  Framebuffer;

} TerminalStruct;

TerminalStruct Terminal;



/*  Strlen(): This function measures the length of a string.

    Input:        char* String                       - This specifies the string you want to measure the length of.

    Output:       int                                - This is the length of the given string.

    This function measures the length of a string. It does this by counting the amount of characters in the string
    until it encounters a null byte.

*/

unsigned short Strlen(char* String) {

  unsigned short Length = 0;
  while (String[Length] != '\0') Length++;
  return Length;

}



/*  InitializeTerminal(): This function initializes the terminal data, which is stored in the Terminal struct.

    Input:        uint16 Rows, Columns               - This should be the amount of rows and columns in the current
                                                     text mode.

    Input:        uint16 TabSize                     - This specifies how many spaces a tab should be.

    Input:        uint32 Framebuffer                 - This specifies where the terminal's framebuffer is.

    This function initializes the data in the Terminal struct, which other functions rely on to access the terminal,
    and it clears it out as well.

    It does this by resetting the X and Y parameters in the struct, and it assigns the other parameters directly to
    the Terminal struct.

    It's assumed that the terminal is a VGA text mode, and it assumes that the framebuffer is consisted by cells,
    where the first byte is an ASCII character, and the second byte is a color attribute.

*/


void InitializeTerminal(uint16 Rows, uint16 Columns, uint16 TabSize, uint32 Framebuffer) {

  Terminal.X = 0;
  Terminal.Y = 0;
  Terminal.Max_X = Rows;
  Terminal.Max_Y = Columns;
  Terminal.TabSize = TabSize;
  Terminal.Framebuffer = Framebuffer;


  for (int i = 0; i < (Rows * Columns); i++) {

    *((uint16*)Framebuffer + i) = 0;

  }

}



/*  Scroll(): Scrolls the terminal one line down.

    (No inputs or outputs)

    This function does two things; it pushes every line but the first line (discarding it) up to make room for another
    line, and it clears the last line.

    It does not modify anything in the Terminal struct (including X and Y), although it relies on it.

    This function should be called whenever there isn't enough room on the terminal to continue to the next line.
    As this is a static function, it is not accessible outside of this file. It is also inlined.

*/

inline static void Scroll(void) {

  Memmove((void*) Terminal.Framebuffer,
          (void*)(Terminal.Framebuffer + (Terminal.Max_X * 2)),
                 (Terminal.Max_X       * ((Terminal.Max_Y - 1) * 2)));


  for (uint16 i = 0; i < Terminal.Max_X; i++) {

    *((uint16*)Terminal.Framebuffer + ((Terminal.Max_Y - 1) * Terminal.Max_X) + i) = ' ' | 0 << 8;

  }

}



/*  Putchar(): Writes a character onto the terminal.

    Input:        char Character                     - This should be the character that you want to write onto the
                                                     terminal. While it's discouraged to use signed chars here, you
                                                     can use whatever signedness that your compiler has for char.

    Input:        uint8 Color                        - This is the color attribute of the character you want to write.
                                                     The highest four bits represent the background color, while the
                                                     lower four bits represent the foreground color. The highest bit
                                                     may be reserved as a 'blinking' attribute.

    This function filters and writes a character onto the terminal. Depending on the character, it may not write
    anything, and instead do something else: a null byte (\0) isn't processed at all, a newline (\n) moves the cursor
    to the next column, a carriage return (\r) moves the cursor to the start of the row, and a tab (\t) moves the
    cursor a certain amount of bytes ahead, depending on what is present in Terminal.TabSize.

    This function automatically goes to the next row and/or scrolls if needed.

    The most commonly used attribute is 0x0F, which is a white foreground (text) on a black background. You may use
    up to 16 colors for the foreground, and up to 8 or 16 background colors depending on the machine, although it's
    recommended to only use up to 8 colors for the background.

*/

void Putchar(char Character, uint8 Color) {

  switch(Character) {

    case '\0':
      break;

    case '\n':

      if (Terminal.Y >= Terminal.Max_Y) {

        Scroll();

      }

      Terminal.Y++;
      break;


    case '\r':

      Terminal.X = 0;
      break;


    case '\b':

      *((uint16*)Terminal.Framebuffer + (Terminal.X) + (Terminal.Y * 80)) = ' ' | Color << 8;

      if (Terminal.X == 0) {

        if (Terminal.Y == 0) {

          break;

        } else {

          Terminal.Y--;
          Terminal.X = Terminal.Max_X - 1;

          break;

        }

      } else {

        Terminal.X--;
        break;

      }

      Terminal.X--;
      break;


    case '\t':

      Terminal.X += (Terminal.TabSize - (Terminal.X % Terminal.TabSize));

      if (Terminal.X >= Terminal.Max_X) {

        Terminal.Y++;

        if (Terminal.Y >= Terminal.Max_Y) {

          Scroll();
          Terminal.Y = Terminal.Max_Y - 1;

        }

        Terminal.X = 0;

      }

      break;


    default:

      if (Terminal.X >= Terminal.Max_X) {

        Terminal.Y++;

        if (Terminal.Y >= Terminal.Max_Y) {

          Scroll();
          Terminal.Y = Terminal.Max_Y - 1;

        }

        Terminal.X = 0;

      }

      *((uint16*)Terminal.Framebuffer + Terminal.X + (Terminal.Y * 80)) = Character | Color << 8;

      Terminal.X++;
      break;

  }

}



/*  Print(): Writes a string onto the terminal.

    Input:        char* String                     - (Same as Putchar)

    Input:        uint8 Color                      - (Same as Putchar)

    This function is just a loop that calls Putchar() for every character in the given string. The same rules apply
    here as they do in Putchar().

*/

void Print(char* String, uint8 Color) {

  for (int i = 0; i < Strlen(String); i++) {

    Putchar(String[i], Color);

  }

}



/*  Itoa(): Converts an integer into a string.

    Input:        unsigned long Value              - This should be the value that you want to convert into a string.
                                                   This is taken as an unsigned value, and this function is not built
                                                   to handle negative numbers.

    Input/Output: char* Buffer                     - This is the buffer that you want Itoa to write the converted
                                                   number to and return. The size must be set by the caller, and the
                                                   function does not check if it is large enough, so be cautious.
                                                   Upon completion, for convenience, this function will return the
                                                   buffer back.

    Input:        unsigned short Base              - This is the 'base', or numbering system that you want to convert
                                                   your value to as a string. Binary is base 2, octal is base 8,
                                                   decimal is base 10, and hex is base 16. Follows the 0->Z order.

    This function converts an (unsigned) integer to a string. It takes in the value to be converted, a buffer to
    write the number as a string to (which is later returned), and the base (numbering system) to convert it to.
    This function can be fairly useful for many purposes, but one of the most important uses is to convert and
    display values for the end user.

    For example, an exception handler could report that a fault happened at 7E00h in memory, and use the Itoa
    function to convert 7E00h into the string "7E00", which could then be displayed to the end user. This conversion
    is necessary, as internally the memory address and/or number 7E00h is represented as the two bytes 00 and 7E,
    which is not the same as the string "7E00", or show up as '7E00' on screen.

    Another important thing to note is the base, or numbering system. Binary is base 2 (0-1), octal is base 8 (0-7),
    decimal is base 10 (0-9), and hexadecimal is base 16 (0-F). You can use a base between 2 and 36 with Itoa, with
    any number outside of that boundary being considered invalid and will only return an empty string.

*/


char* Itoa(unsigned long Value, char* Buffer, unsigned short Base) {

  if ((Base > 36) || (Base < 2)) {

    Buffer[0] = '\0';
    return Buffer;

  }

  unsigned int i = 0;

  do {

    unsigned long Digit = Value % Base;

    if (Digit < 10) {

      Buffer[i++] = Digit + '0';

    } else {

      Buffer[i++] = Digit - 10 + 'A';

    }

    Value /= Base;

  } while (Value > 0);

  Buffer[i] = '\0';

  unsigned int j = i - 1;

  for (unsigned int k = 0; k < (i / 2); k++) {

    char SwapBuffer = Buffer[k];
    Buffer[k] = Buffer[j - k];
    Buffer[j - k] = SwapBuffer;

  }

  return Buffer;

}
