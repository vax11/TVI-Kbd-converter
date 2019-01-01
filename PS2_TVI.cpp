/* PS2_TVI.cpp, an arduino program using the LGPL-licensed PS2Keyboard
 * Library by Christian Weichel <info@32leaves.net> & Paul Stoffregen 
 * <paul@pjrc.com> to connect a PS/2 protocol keyboard to a TeleVideo
 * terminal.
 *
 * Copyright (C) 2018 Patrick Finnegan <pat@vax11.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */




#include "PS2Keyboard.h"

#define PS2DATA_PIN 4
#define PS2CLOCK_PIN 3
#define RSTOUT_PIN 2
#define LED_PIN 13
#define HOSTBAUD (9600)

#define PREFIX_F0 1
#define PREFIX_E0 2
#define PREFIX_E1 4

#define MOD_LSHIFT 1
#define MOD_RSHIFT 2
#define MOD_LCTRL  4
#define MOD_RCTRL  8
#define MOD_LALT   16
#define MOD_RALT   32
#define MOD_CLOCK  64
#define MOD_NLOCK  128

#define TVI_ALOCK (0x10)
#define TVI_SHIFT (0x20)
#define TVI_CTRL  (0x40)
#define TVI_FUNCT (0x80)

#define KEY_F1	(0x80)
#define KEY_F2	(0x81)
#define KEY_F3	(0x82)
#define KEY_F4	(0x83)
#define KEY_F5	(0x84)
#define KEY_F6	(0x85)
#define KEY_F7	(0x86)
#define KEY_F8	(0x87)
#define KEY_F9	(0x88)
#define KEY_F10	(0x89)
#define KEY_F11	(0x8A)
#define KEY_F12	(0x8B)
#define KEY_F13	(0x8C)
#define KEY_F14	(0x8D)
#define KEY_F15	(0x8E)
#define KEY_F16	(0x8F)
#define KEY_KP_0	(0x90)
#define KEY_KP_1	(0x91)
#define KEY_KP_2	(0x92)
#define KEY_KP_3	(0x93)
#define KEY_KP_4	(0x94)
#define KEY_KP_5	(0x95)
#define KEY_KP_6	(0x96)
#define KEY_KP_7	(0x97)
#define KEY_KP_8	(0x98)
#define KEY_KP_9	(0x99)
#define KEY_KP_DOT	(0x9A)
#define KEY_KP_PLUS	(0x9B)
#define KEY_KP_DASH	(0x9C)
#define KEY_KP_STAR	(0x9D)
#define KEY_KP_SLASH	(0x9E)
#define KEY_KP_ENTER	(0x9F)
#define KEY_SLOCK	(0xA0)
#define KEY_BREAK	(0xA1)
#define KEY_PRTSC	(0xA2)
#define KEY_PAUSE	(0xA3)
#define KEY_SYSRQ	(0xA4)
#define KEY_ENTER	(0xA8)
#define KEY_BKSP	(0xA9)
#define KEY_TAB		(0xAA)
#define KEY_ESC		(0xAB)
#define KEY_E0_INS	(0xB0)
#define KEY_E0_END	(0xB1)
#define KEY_E0_DOWN	(0xB2)
#define KEY_E0_PGDN	(0xB3)
#define KEY_E0_LEFT	(0xB4)
#define KEY_E0_RIGHT	(0xB6)
#define KEY_E0_HOME	(0xB7)
#define KEY_E0_UP	(0xB8)
#define KEY_E0_PGUP	(0xB9)
#define KEY_E0_DEL	(0xBA)
#define SHIFT_OFFSET	(0x40)
#define NLOCK_OFFSET	(KEY_E0_INS-KEY_KP_0)

#define SCAN_LSHIFT	(0x12)
#define SCAN_RSHIFT	(0x59)
#define SCAN_ALT	(0x11)
#define SCAN_CTRL	(0x14)
#define SCAN_NLOCK	(0x77)
#define SCAN_CLOCK	(0x58)
#define SCAN_SYSRQ	(0x84)

#define SCAN_E0_END	(0x69)
#define SCAN_E0_LEFT	(0x6B)
#define SCAN_E0_HOME 	(0x6C)
#define SCAN_E0_INS 	(0x70)
#define SCAN_E0_DEL 	(0x71)
#define SCAN_E0_DOWN 	(0x72)
#define SCAN_E0_RIGHT 	(0x74)
#define SCAN_E0_UP 	(0x75)
#define SCAN_E0_PGDN 	(0x7A)
#define SCAN_E0_PGUP 	(0x7D)
#define SCAN_E0_KPSL 	(0x4A)
#define SCAN_E0_KPENT 	(0x5A)
#define SCAN_E0_PRTSC	(0x7C)
#define SCAN_E0_BREAK	(0x7E)

//https://wiki.osdev.org/PS/2_Keyboard#Scan_Code_Sets.2C_Scan_Codes_and_Key_Codes
const byte ps2_to_intermediate[] = {

	0, // 00h = err
	KEY_F9, // 01h = F9
	0, // 02h = 
	KEY_F5, // 03h = F5
	KEY_F3, // 04h = F3
	KEY_F1, // 05h = F1
	KEY_F2, // 06h = F2
	KEY_F12, // 07h = F12
	0, // 08h = 
	KEY_F10, // 09h = F10
	KEY_F8, // 0Ah = F8
	KEY_F6, // 0Bh = F6
	KEY_F4, // 0Ch = F4
	KEY_TAB, // 0Dh = TAB
	'`', // 0Eh = ` (back quote)
	0, // 0Fh = 
	0, // 10h = 
	0, // 11h = LALT
	0, // 12h = LSHIFT
	0, // 13h = 
	0, // 14h = LCTRL
	'q', // 15h = Q
	'1', // 16h = 1
	0, // 17h = 
	0, // 18h = 
	0, // 19h = 
	'z', // 1Ah = Z
	's', // 1Bh = S
	'a', // 1Ch = A
	'w', // 1Dh = W
	'2', // 1Eh = 2
	0, // 1Fh = 
	0, // 20h = 
	'c', // 21h = C
	'x', // 22h = X
	'd', // 23h = D
	'e', // 24h = E
	'4', // 25h = 4
	'3', // 26h = 3
	0, // 27h = 
	0, // 28h = 
	' ', // 29h = SPACE
	'v', // 2Ah = V
	'f', // 2Bh = F
	't', // 2Ch = T
	'r', // 2Dh = R
	'5', // 2Eh = 5
	0, // 2Fh = 
	0, // 30h = 
	'n', // 31h = N
	'b', // 32h = B
	'h', // 33h = H
	'g', // 34h = G
	'y', // 35h = Y
	'6', // 36h = 6
	0, // 37h = 
	0, // 38h = 
	0, // 39h = 
	'm', // 3Ah = M
	'j', // 3Bh = J
	'u', // 3Ch = U
	'7', // 3Dh = 7
	'8', // 3Eh = 8
	0, // 3Fh = 
	0, // 40h = 
	',', // 41h = , comma
	'k', // 42h = K
	'i', // 43h = I
	'o', // 44h = O
	'0', // 45h = 0 (zero)
	'9', // 46h = 9
	0, // 47h = 
	0, // 48h = 
	'.', // 49h = . dot
	'/', // 4Ah = /
	'l', // 4Bh = L
	';', // 4Ch = ;
	'p', // 4Dh = P
	'-', // 4Eh = -
	0, // 4Fh = 
	0, // 50h = 
	0, // 51h = 
	0x27, // 52h = ' (quote)
	0, // 53h = 
	'[', // 54h = [
	'=', // 55h = =
	0, // 56h = 
	0, // 57h = 
	0, // 58h = CAPS LOCK
	0, // 59h = RSHIFT
	KEY_ENTER, // 5Ah = ENTER
	']', // 5Bh = ]
	0, // 5Ch = 
	0x5C, // 5Dh = BKSLASH
	0, // 5Eh = 
	0, // 5Fh = 
	0, // 60h = 
	0, // 61h = 
	0, // 62h = 
	0, // 63h = 
	0, // 64h = 
	0, // 65h = 
	KEY_BKSP, // 66h = BKSP
	0, // 67h = 
	0, // 68h = 
	KEY_KP_1, // 69h = KP1
	0, // 6Ah = 
	KEY_KP_4, // 6Bh = KP4
	KEY_KP_7, // 6Ch = KP7
	0, // 6Dh = 
	0, // 6Eh = 
	0, // 6Fh = 
	KEY_KP_0, // 70h = KP 0
	KEY_KP_DOT, // 71h = KP .
	KEY_KP_2, // 72h = KP 2
	KEY_KP_5, // 73h = KP 5
	KEY_KP_6, // 74h = KP 6
	KEY_KP_8, // 75h = KP 8
	KEY_ESC, // 76h = ESC
	0, // 77h = NUM LOCK
	KEY_F11, // 78h = F11
	KEY_KP_PLUS, // 79h = KP +
	KEY_KP_3, // 7Ah = KP 3
	KEY_KP_DASH, // 7Bh = KP -
	KEY_KP_STAR, // 7Ch = KP *
	KEY_KP_9, // 7Dh = KP 9
	KEY_SLOCK, // 7Eh = SCROLL LOCK
	0, // 7Fh
	0, // 80h
	0, // 81h
	0, // 82h
	KEY_F7, // 83h = F7

};
#define NUM_PS2SCAN (sizeof(ps2_to_intermediate)/sizeof(ps2_to_intermediate[0]))

const byte intermediate_shift_xlat[256] = {

0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 
' ' , '!' , '"' , '#' , '$' , '%' , '&' , '"' , '(' , ')' , '*' , '+' , '<' , '_' , '>' , '?' , 
')' , '!' , '@' , '#' , '$' , '%' , '^' , '&' , '*' , '(' , ':' , ':' , '<' , '+' , '>' , '?' , 
'@' , 'A' , 'B' , 'C' , 'D' , 'E' , 'F' , 'G' , 'H' , 'I' , 'J' , 'K' , 'L' , 'M' , 'N' , 'O' , 
'P' , 'Q' , 'R' , 'S' , 'T' , 'U' , 'V' , 'W' , 'X' , 'Y' , 'Z' , '{' , '|' , '}' , '^' , '_' , 
'~' , 'A' , 'B' , 'C' , 'D' , 'E' , 'F' , 'G' , 'H' , 'I' , 'J' , 'K' , 'L' , 'M' , 'N' , 'O' , 
'P' , 'Q' , 'R' , 'S' , 'T' , 'U' , 'V' , 'E' , 'X' , 'Y' , 'Z' , '{' , '|' , '}' , '~' , 0x7F, 
0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 
0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 
0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 
0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, 
0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 
0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 
0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 
0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 

};
const byte intermediate_alock_xlat[256] = {

0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 
' ' , '!' , '"' , '#' , '$' , '%' , '&' , 0x27, '(' , ')' , '*' , '+' , ',' , '-' , '.' , '/' , 
'0' , '1' , '2' , '3' , '4' , '5' , '6' , '7' , '8' , '9' , ':' , ';' , '<' , '=' , '>' , '?' , 
'@' , 'A' , 'B' , 'C' , 'D' , 'E' , 'F' , 'G' , 'H' , 'I' , 'J' , 'K' , 'L' , 'M' , 'N' , 'O' , 
'P' , 'Q' , 'R' , 'S' , 'T' , 'U' , 'V' , 'W' , 'X' , 'Y' , 'Z' , '[' , '\\', ']' , '^' , '_' , 
'`' , 'A' , 'B' , 'C' , 'D' , 'E' , 'F' , 'G' , 'H' , 'I' , 'J' , 'K' , 'L' , 'M' , 'N' , 'O' , 
'P' , 'Q' , 'R' , 'S' , 'T' , 'U' , 'V' , 'E' , 'X' , 'Y' , 'Z' , '{' , '|' , '}' , '~' , 0x7F, 
0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 
0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 
0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 
0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 
0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 
0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 
0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 
0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, 

};

const byte intermediate_to_tvi[256] = {

// ^x in 00-1F
0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 
// Normal characters 20-7E. leave 7F in case we use it later.
' ' , '!' , '"' , '#' , '$' , '%' , '&' , 0x27, '(' , ')' , '*' , '+' , ',' , '-' , '.' , '/' , 
'0' , '1' , '2' , '3' , '4' , '5' , '6' , '7' , '8' , '9' , ':' , ';' , '<' , '=' , '>' , '?' , 
'@' , 'A' , 'B' , 'C' , 'D' , 'E' , 'F' , 'G' , 'H' , 'I' , 'J' , 'K' , 'L' , 'M' , 'N' , 'O' , 
'P' , 'Q' , 'R' , 'S' , 'T' , 'U' , 'V' , 'W' , 'X' , 'Y' , 'Z' , '[' , '\\', ']' , '^' , '_' , 
'`' , 'a' , 'b' , 'c' , 'd' , 'e' , 'f' , 'g' , 'h' , 'i' , 'j' , 'k' , 'l' , 'm' , 'n' , 'o' , 
'p' , 'q' , 'r' , 's' , 't' , 'u' , 'v' , 'w' , 'x' , 'y' , 'z' , '{' , '|' , '}' , '~' , 0x7F,
// 80-8F = unshifted F1-F16
0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 
// 90-9F = KP digits 0-9 . + - * / ENTER
//                                                            .    ,     -    ce   send   enter
0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xAE, 0xAC, 0xAD, 0xF8, 0xF2, 0xF4, 
// A0-AF = special keys
// SLOC BRK PRTS SYSRQ                          ENTR  BKSP  TAB   ESC
// NSCR BRK PRNT                                RETN  BKSP  TAB   ESC
0xFD, 0xFB, 0x92, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x8F, 0x89, 0xF0, 0x00, 0x00, 0x00, 0x00, 
// B0-BF = edit keys
// INS END  DOWN  PGDN  LEFT        RIGHT HOME  UP    PGUP  DEL
// CINS SEND DOWN PAGE  LEFT        RIGHT HOME  UP          DEL
0x94, 0xF2, 0x8A, 0x9A, 0x88, 0x00, 0x8C, 0x8E, 0x8B, 0x9A, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 
// C0-CF = shifted F1-F16
0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 
// D0-DF = shifted KP digits 0-9 . + - * / ENTER
//                                                            .    ,     -    ce   send   enter
0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xAE, 0xAC, 0xAD, 0xF9, 0xF3, 0xF5,
// E0-EF = shifted special keys
// SLOC BRL PRTS SYSRQ                          ENTR  BKSP  TAB   ESC
// SETU SBRK PRNT                               LF    CLRSP BTAB  LESC
0xFE, 0xFC, 0xA2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x9e, 0x91, 0xF1, 0x00, 0x00, 0x00, 0x00, 
// F0-FF = shifted edit keys
// INS END  DOWN  PGDN  LEFT        RIGHT HOME  UP    PGUP  DEL
// LINS SSEND SDOWN SPAGE SLEFT     SRGHT SHOME SUP   SPAGE LDEL
0x96, 0xF3, 0x82, 0xAA, 0x80, 0x00, 0x84, 0x86, 0x83, 0xAA, 0x97, 0x00, 0x00, 0x00, 0x00, 0x00, 

};

// These characters should have the shift status the opposite of what they are on a 
// PS/2 keyboard, due to the TVI keyboard layout -- these are intermediate codes
// {, ], Line Feed (0xE8), Back Tab (0xE9), Line Insert (0xF0), and Line Delete (0xFA)
const byte tviReverseShifted[] = {
	'{',']', 0xE8, 0xE9, 0xEA, 0xF0, 0xFA, 0 };

int keycode = 0;
int oldkeycode = 0;

// #define DEBUGCODE
#ifdef DEBUGCODE
byte debug=1;
#else
byte debug=0;
#endif

PS2Keyboard ps2;

// Returns TVI_SHIFT to xor with byte1, if the shift you get from the keyboard is not what tvi wants
byte reverseShift(byte c) {
	byte i=0;

	while (tviReverseShifted[i]) {
		if (tviReverseShifted[i] == c)
			return TVI_SHIFT;
		i++;
	}
	return 0;
}

void waitClk(int pin) {
	while (!digitalRead(pin)) delayMicroseconds(10);
	while (digitalRead(pin)) delayMicroseconds(10);
}

void waitClkLow(int pin) {
	while (digitalRead(pin)) 
		;
}
#ifdef DEBUGCODE
void serialWriteNum(int num) {

	char digits[9] = {0,0,0,0,0,0,0,0,0} , i=8;

	if (num<0) {
		Serial.write('-');
		num = -num;
	}
	do {
		i--;
		digits[i] = num%10 + '0';
		num /= 10;

	} while ((num > 0) && (i>0));
	Serial.write(&digits[i]);
	
}
#endif


// We don't actually use this routine right now, but it's a non-interrupt driven version
// of the PS2Keyboard routine to get scan codes
int readByte(int c, int d) {
	byte i, parity = 1, inval;
	int val=0;

	waitClkLow(c);			// Start bit must be 0
	inval = digitalRead(d);
	if (inval) {
		delay(1);
		return -256;		// Return -256 if bad start bit
	}
	for (i=0; i<10; i++) {
		waitClk(c);
		inval = digitalRead(d);
		val = (val>>1) | (inval<<7);
		parity ^= inval;
	}
	parity ^= inval;
	if (parity)
		return -(val & 0xFF);	// return negative of val if parity error
	if (!inval)
		return -257;		// return -257 if stop bit error
	return (val & 0xFF);		// If we're good just return the actual value

}

// Send a byte out the serial line, mostly to set the Lock LED status
void sendByte(int c, int d, byte data) {
	byte i;
	byte parity = 1;
	byte bit;

#	ifdef DEBUGCODE
	char debugmsg[14] = {'b',':',' ',' ',' ',' ',' ',' ',' ',' ',' ',13,10,0};
#	endif

	pinMode(c, OUTPUT);		// Send attention
	digitalWrite(c, LOW);
	delayMicroseconds(100);
	pinMode(d, OUTPUT);
	digitalWrite(d, LOW);
	pinMode(c, INPUT_PULLUP);
	waitClk(c);			// Start bit
	for (i=0;i<8;i++) {		// Data bits 0 - 7
		bit = data & 1;
		digitalWrite(d, bit);
#		ifdef DEBUGCODE
		debugmsg[i+2] = (bit) + 0x30;
#		endif
		parity = parity ^ bit;
		data = data >> 1;
		waitClk(c);
	}
	digitalWrite(d, parity);
#	ifdef DEBUGCODE
	debugmsg[10] = parity + 0x30;
#	endif
	waitClk(c);			// Parity bit
	pinMode(d, INPUT_PULLUP);
	waitClk(c);			// Stop bit
					// ACK response
	while (digitalRead(d)) delayMicroseconds(10);
	while (digitalRead(c)) delayMicroseconds(10);
	while (!digitalRead(c) || !digitalRead(d)) delayMicroseconds(10);
#	ifdef DEBUGCODE
	if (debug) {
		interrupts();
		Serial.write(debugmsg);
		noInterrupts();

	}
#	endif


}

// Send the keyboard LEDs
void sendLEDs(byte mod) {
	int leds=0;
	//int retval1, retval2;

	if (mod & MOD_CLOCK)
		leds = 4;
	if (mod & MOD_NLOCK)
		leds |= 2;

	noInterrupts();
	sendByte(PS2CLOCK_PIN, PS2DATA_PIN, 0xed);
	//retval1 = readByte(PS2CLOCK_PIN, PS2DATA_PIN);
	interrupts();
	delay(2);
	noInterrupts();
	sendByte(PS2CLOCK_PIN, PS2DATA_PIN, leds);
	//retval2 = readByte(PS2CLOCK_PIN, PS2DATA_PIN);
	interrupts();

}

void setup () {

	// PS2Keyboard doesn't init the keyboard, so do that ourselves
	pinMode(RSTOUT_PIN, OUTPUT);
	digitalWrite(RSTOUT_PIN, HIGH);

	sendByte(PS2CLOCK_PIN, PS2DATA_PIN, 0xff);

	// Initialize PS2Keyboard
	ps2.begin(PS2DATA_PIN, PS2CLOCK_PIN);
	
	// Initialize the serial line to the host/terminal
	Serial.begin(HOSTBAUD, SERIAL_8N1);
}
	

// Scan codes from http://www.vetra.com/scancodes.html et al

void loop () {
	// Track the modifier keys
	byte prefix = 0;
	byte modifier = MOD_NLOCK;
	byte oldmodifier = -1;
	
	byte scancode = 0;
	byte xlatcode0 = 0;
	byte xlatcode1 = 0;


#	ifdef DEBUGCODE
	char nums[6] = {0,0,'h',13,10,0};
	if (debug) Serial.write("Starting up...\r\n");
#	endif

	while (1) {
		scancode = ps2.readScanCode();

#		ifdef DEBUGCODE
		if (debug) {
			if (scancode) {
				nums[0] = '0' + scancode/16; if (nums[0]>'9') nums[0] += 7;
				nums[1] = '0' + scancode%16; if (nums[1]>'9') nums[1] += 7;
				Serial.write("Scan code: ");
				Serial.write((const char *)nums);
			}
		}
#		endif

		if (scancode) {
			keycode = 0;
			if (scancode == 0xE0) {
				prefix |= PREFIX_E0;
#				ifdef DEBUGCODE
				if (debug) Serial.write("Prefix E0\r\n");
#				endif
				continue;
			}
			else if (scancode == 0xF0) {
				prefix |= PREFIX_F0;
#				ifdef DEBUGCODE
				if (debug) Serial.write("Prefix F0\r\n");
#				endif
				continue;
			}
			else if (scancode == 0xE1) {
				prefix |= PREFIX_E1;
#				ifdef DEBUGCODE
				if (debug) Serial.write("Prefix E1\r\n");
#				endif
				continue;
			}
			else {
				// A real code
				// Handle modifier keys
				if ((scancode==SCAN_LSHIFT) && !(prefix & PREFIX_E0)) {
#					ifdef DEBUGCODE
					if (debug) Serial.write("Left shift\r\n");
#					endif
					if (prefix & PREFIX_F0)
						modifier &= ~(MOD_LSHIFT);
					else
						modifier |= MOD_LSHIFT;
				} else if ((scancode==SCAN_RSHIFT) && !(prefix & PREFIX_E0)) {
#					ifdef DEBUGCODE
					if (debug) Serial.write("Right shift\r\n");
#					endif
					if (prefix & PREFIX_F0)
						modifier &= ~(MOD_RSHIFT);
					else
						modifier |= MOD_RSHIFT;
				} else if ((scancode==SCAN_CTRL) && !(prefix & PREFIX_E1)) { // Skip this if it's the pause sequence
					if (prefix & PREFIX_E0) {
#						ifdef DEBUGCODE
						if (debug) Serial.write("Right ctrl\r\n");
#						endif
						if (prefix & PREFIX_F0)
							modifier &= ~(MOD_RCTRL);
						else
							modifier |= MOD_RCTRL;
					} else {
#						ifdef DEBUGCODE
						if (debug) Serial.write("Left ctrl\r\n");
#						endif
						if (prefix & PREFIX_F0)
							modifier &= ~(MOD_LCTRL);
						else
							modifier |= MOD_LCTRL;
					}
				} else if (scancode==SCAN_ALT) {
					if (prefix & PREFIX_E0) {
#						ifdef DEBUGCODE
						if (debug) Serial.write("Right alt\r\n");
#						endif
						if (prefix & PREFIX_F0)
							modifier &= ~(MOD_RALT);
						else
							modifier |= MOD_RALT;
					} else {
#						ifdef DEBUGCODE
						if (debug) Serial.write("Left alt\r\n");
#						endif
						if (prefix & PREFIX_F0)
							modifier &= ~(MOD_LALT);
						else
							modifier |= MOD_LALT;
					}
				} else if ((scancode==SCAN_CLOCK) && !(prefix)) {//&(PREFIX_F0|PREFIX_E0|PREFIX_E1))) {
#					ifdef DEBUGCODE
					if (debug) Serial.write("CAPS\r\n");
#					endif
					modifier ^= MOD_CLOCK;
				} else if ((scancode==SCAN_NLOCK) && !(prefix)) {//&(PREFIX_F0|PREFIX_E0|PREFIX_E1))) {
#					ifdef DEBUGCODE
					if (debug) Serial.write("NUM\r\n");
#					endif
					modifier ^= MOD_NLOCK;
				}
				// Handle E0 codes
				if (prefix & PREFIX_E0) {
					if (!(prefix & PREFIX_F0)) {
						switch (scancode) {
							case SCAN_E0_END:
								keycode = KEY_E0_END;
								break;
							case SCAN_E0_LEFT:
								keycode = KEY_E0_LEFT;
								break;
							case SCAN_E0_HOME:
								keycode = KEY_E0_HOME;
								break;
							case SCAN_E0_INS:
								keycode = KEY_E0_INS;
								break;
							case SCAN_E0_DEL:
								keycode = KEY_E0_DEL;
								break;
							case SCAN_E0_DOWN:
								keycode = KEY_E0_DOWN;
								break;
							case SCAN_E0_RIGHT:
								keycode = KEY_E0_RIGHT;
								break;
							case SCAN_E0_UP:
								keycode = KEY_E0_UP;
								break;
							case SCAN_E0_PGDN:
								keycode = KEY_E0_PGDN;
								break;
							case SCAN_E0_PGUP:
								keycode = KEY_E0_PGUP;
								break;
							case SCAN_E0_KPSL:
								keycode = KEY_KP_SLASH;
								break;
							case SCAN_E0_KPENT:
								keycode = KEY_KP_ENTER;
								break;
							case SCAN_E0_PRTSC:
								keycode = KEY_PRTSC;
								break;
							case SCAN_E0_BREAK:
								keycode = KEY_BREAK;
								break;
						}
						oldkeycode = keycode;
					} else {
						oldkeycode = 0;
						keycode = 0;
					}
				} else if (prefix & PREFIX_E1) { // Code to just handle the pause key
					if (scancode == SCAN_CTRL)
						continue;	// Ignore ctrl but don't clear prefixes
					if (scancode == SCAN_NLOCK) { // Pause
						if (prefix & PREFIX_F0)
							keycode = 0;
						else
							keycode = KEY_PAUSE;
						oldkeycode = 0;
					} else {
						keycode = 0;
					}
				
				
				} else if (scancode == SCAN_SYSRQ) {
					
					// Sys-Rq = reset system
					digitalWrite(RSTOUT_PIN, LOW);
					digitalWrite(LED_PIN, HIGH);
					delay(500);
					digitalWrite(RSTOUT_PIN, HIGH);
					digitalWrite(LED_PIN, LOW);
					
				} else {   // Handle normal codes

					if (scancode < NUM_PS2SCAN) 
						keycode=ps2_to_intermediate[scancode];
					else
						keycode=0;

#					ifdef DEBUGCODE
					if (debug) {
						nums[0] = '0' + scancode/16; if (nums[0]>'9') nums[0] += 7;
						nums[1] = '0' + scancode%16; if (nums[1]>'9') nums[1] += 7;
						Serial.write("Scancode (2): ");
						Serial.write((const char *)nums);
						nums[0] = '0' + keycode/16; if (nums[0]>'9') nums[0] += 7;
						nums[1] = '0' + keycode%16; if (nums[1]>'9') nums[1] += 7;
						Serial.write("Keycode: ");
						Serial.write((const char *)nums);
					}
#					endif

					if (!(modifier & MOD_NLOCK)) {
						// If numlock is off, change to edit keys
						if ((keycode >= KEY_KP_0) && (keycode <=KEY_KP_DOT)) {
							keycode += NLOCK_OFFSET;
						}
					}
					if (prefix & PREFIX_F0) {
						if (keycode == oldkeycode) {
							oldkeycode = 0;
						}
						keycode = 0;
					} else {
						oldkeycode = keycode;
					}
				}
			}
			if (keycode) {
				// Generate tvi code
				xlatcode0=0;
				xlatcode1=keycode;
				if (modifier & (MOD_LSHIFT|MOD_RSHIFT)) {
					xlatcode0 |= TVI_SHIFT;
					xlatcode1 = intermediate_shift_xlat[xlatcode1];

#					ifdef DEBUGCODE
					if (debug) {
						nums[0] = '0' + xlatcode1/16; if (nums[0]>'9') nums[0] += 7;
						nums[1] = '0' + xlatcode1%16; if (nums[1]>'9') nums[1] += 7;
						Serial.write("shift translate:");
						Serial.write((const char *)nums);
					}
#					endif

				}

				if (modifier & MOD_CLOCK) { // Handle Caps Lock / Alpha Lock
					// set bit, translate
					xlatcode0 |= TVI_ALOCK;
					xlatcode1 = intermediate_alock_xlat[xlatcode1];

#					ifdef DEBUGCODE
					if (debug) {
						nums[0] = '0' + xlatcode1/16; if (nums[0]>'9') nums[0] += 7;
						nums[1] = '0' + xlatcode1%16; if (nums[1]>'9') nums[1] += 7;
						Serial.write("alock translate:");
						Serial.write((const char *)nums);
					}
#					endif

				}

				if (modifier & (MOD_LALT|MOD_RALT)) { // Turn ALT into FUNCT
					xlatcode0 |= TVI_FUNCT;
				}

				if (modifier & (MOD_LCTRL|MOD_RCTRL)) { // Add CTRL after shift status
					xlatcode0 |= TVI_CTRL;
					if (xlatcode1 >= 0x40 && xlatcode1 <= 0x7F)
						xlatcode1 &= 0x1F; // Convert to control codes
				}

				// For codes that are should have shift reversed, do that.
				xlatcode0 ^= reverseShift(xlatcode1);
				xlatcode1 = intermediate_to_tvi[xlatcode1];

#				ifdef DEBUGCODE
				if (debug) {
					nums[0] = '0' + xlatcode1/16; if (nums[0]>'9') nums[0] += 7;
					nums[1] = '0' + xlatcode1%16; if (nums[1]>'9') nums[1] += 7;
					Serial.write("tvi translate:");
					Serial.write((const char *)nums);
				}
				if (!debug) {
#				endif

					Serial.write(xlatcode0);
					Serial.write(xlatcode1);

#				ifdef DEBUGCODE
				}
#				endif

			}
			prefix=0; // Reset prefixes if we sent out a character

		} else {

			// No keycode, send LEDs if numlock/capslock changed
			if ((oldmodifier ^ modifier) & (MOD_NLOCK|MOD_CLOCK)) {
				sendLEDs(modifier);
				oldmodifier = modifier;
			}
			
			// sleep 2ms
			delay(2);
		}

	}
}
	
