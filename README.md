_TVI-Kbd-converter_

This is an arduino project to convert a PS/2 interface keyboard to attach
to a TeleVideo terminal. It is verified to work with a 965, and should work
with other terminals such as the 970, and TS-803 computer with no change.

With just a baud rate change to 1200 baud, it might also work with 925/950
based terminals with a 4-pin modular keyboard. Note that these all use 
12V power to the keyboard, so they should not be directly connected to the
VCC pin.

This uses the PS2Keyboard Arduino library, so download and add that to
the IDE first:
http://www.pjrc.com/teensy/td_libs_PS2Keyboard.html
https://github.com/PaulStoffregen/PS2Keyboard

See the schematic in TVI-Kbd-converter.sch / .png below.

Note that this requires a straight-through modular cable to connect to the
terminal, where most phone cables are cross over (they swap pin directions
on each end).  So you may need to crimp your own cable for this to work.

This project is distributed under the GNU GPL v3, see the file "LICENSE"
for details.
