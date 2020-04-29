// HD44780 support
//
// MIT License
//
// Copyright (c) 2020
// Haris Andrianakis
// Website: www.candrian.gr
// haris@candrian.gr
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __HD44780_H
#define __HD44780_H

#include "F28x_Project.h"
#include "Configuration.h"

typedef unsigned char uint8;

#define LCD_RS  GPIO10
#define LCD_RW  GPIO11
#define LCD_E   GPIO8

#define LCD_DB4  GPIO9
#define LCD_DB5  GPIO4
#define LCD_DB6  GPIO5
#define LCD_DB7  GPIO30

#define GPIO_SET(pin) GpioDataRegs.GPASET.bit.pin = 1
#define GPIO_CLEAR(pin) GpioDataRegs.GPACLEAR.bit.pin = 1
#define GPIO_GET(pin) GpioDataRegs.GPADAT.bit.pin
#define GPIO_INPUT(pin) GpioCtrlRegs.GPADIR.bit.pin = 0;
#define GPIO_OUTPUT(pin) GpioCtrlRegs.GPADIR.bit.pin = 1;

#define E_delay()   DELAY_US(1)
#define E_high()    GPIO_SET(LCD_E)
#define E_low()     GPIO_CLEAR(LCD_E)
#define RW_high()   GPIO_SET(LCD_RW)
#define RW_low()    GPIO_CLEAR(LCD_RW)
#define RS_high()   GPIO_SET(LCD_RS)
#define RS_low()    GPIO_CLEAR(LCD_RS)

#define TYPE_INSTRUCTION    0
#define TYPE_DATA           1
#define LCD_LINES           2
#define LCD_DISP_LENGTH    16
#define LCD_LINE_LENGTH  0x40     // Internal line length of the display
#define LCD_START_LINE1  0x00     // DDRAM address of first char of line 1
#define LCD_START_LINE2  0x40     // DDRAM address of first char of line 2
#define LCD_WRAP_LINES      0     // 0: no wrap, 1: wrap at end of visible line


// Instruction register bit positions, see HD44780U data sheet
#define LCD_CLR               0      // DB0: clear display
#define LCD_HOME              1      // DB1: return to home position
#define LCD_ENTRY_MODE        2      // DB2: set entry mode
#define LCD_ENTRY_INC         1      // DB1: 1=increment, 0=decrement
#define LCD_ENTRY_SHIFT       0      // DB2: 1=display shift on
#define LCD_ON                3      // DB3: turn lcd/cursor on
#define LCD_ON_DISPLAY        2      // DB2: turn display on
#define LCD_ON_CURSOR         1      // DB1: turn cursor on
#define LCD_ON_BLINK          0      // DB0: blinking cursor ?
#define LCD_MOVE              4      // DB4: move cursor/display
#define LCD_MOVE_DISP         3      // DB3: move display (0-> cursor) ?
#define LCD_MOVE_RIGHT        2      // DB2: move right (0-> left) ?
#define LCD_FUNCTION          5      // DB5: function set
#define LCD_FUNCTION_8BIT     4      // DB4: set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_2LINES   3      // DB3: two lines (0->one line)
#define LCD_FUNCTION_10DOTS   2      // DB2: 5x10 font (0->5x7 font)
#define LCD_CGRAM             6      // DB6: set CG RAM address
#define LCD_DDRAM             7      // DB7: set DD RAM address
#define LCD_BUSY              7      // DB7: LCD is busy

// Set entry mode: display shift on/off, dec/inc cursor move direction
#define LCD_ENTRY_DEC            0x04   // display shift off, dec cursor move dir
#define LCD_ENTRY_DEC_SHIFT      0x05   // display shift on,  dec cursor move dir
#define LCD_ENTRY_INC_           0x06   // display shift off, inc cursor move dir
#define LCD_ENTRY_INC_SHIFT      0x07   // display shift on,  inc cursor move dir

// Display on/off, cursor on/off, blinking char at cursor position
#define LCD_DISP_OFF             0x08   // display off
#define LCD_DISP_ON              0x0C   // display on, cursor off
#define LCD_DISP_ON_BLINK        0x0D   // display on, cursor off, blink char
#define LCD_DISP_ON_CURSOR       0x0E   // display on, cursor on
#define LCD_DISP_ON_CURSOR_BLINK 0x0F   // display on, cursor on, blink char

// Move cursor/shift display
#define LCD_MOVE_CURSOR_LEFT     0x10   // move cursor left  (decrement)
#define LCD_MOVE_CURSOR_RIGHT    0x14   // move cursor right (increment)
#define LCD_MOVE_DISP_LEFT       0x18   // shift display left
#define LCD_MOVE_DISP_RIGHT      0x1C   // shift display right

// Function set: set interface data length and number of display lines
#define LCD_FUNCTION_4BIT_1LINE  0x20   // 4-bit interface, single line, 5x7 dots
#define LCD_FUNCTION_4BIT_2LINES 0x28   // 4-bit interface, dual line,   5x7 dots
#define LCD_FUNCTION_8BIT_1LINE  0x30   // 8-bit interface, single line, 5x7 dots
#define LCD_FUNCTION_8BIT_2LINES 0x38   // 8-bit interface, dual line,   5x7 dots

#define LCD_MODE_DEFAULT     ((1<<LCD_ENTRY_MODE) | (1<<LCD_ENTRY_INC) )

class LCD
{
private:

    void E_toggle(void);
    uint8 waitbusy(void);
    void command(uint8 command);
    uint8 read(uint8 rs);
    void write(uint8 data, uint8 rs);
    void data(uint8 data);
    uint8 getxy(void);

public:
    Lcd(void);
    void newline(uint8 position);
    void gotoxy(uint8 x, uint8 y);
    void clear(void);
    void home(void);
    void printChar(char character);
    void print(const char *s);
    void initHardware(uint8 displayAttribute);

};

#endif
