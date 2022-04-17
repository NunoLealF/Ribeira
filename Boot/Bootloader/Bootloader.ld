/* Ribeira | Written in 2022 by NunoLealF
   To the extent possible under law, NunoLealF has waived all copyright and related or neighboring rights to this
   software to the public domain worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with this software.
   If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. */

ENTRY(Bootloader)

SECTIONS
{
  . = 0x7E00;
  .text :
  {
    *(.text.Bootloader)
    *(.text*);
  }
}
