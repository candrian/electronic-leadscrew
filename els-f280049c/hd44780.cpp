#include "hd44780.h"

LCD :: Lcd( void ) {

}

void LCD :: initHardware(uint8 displayAttribute) {

    // HD44780 is used in 4-bt mode. Only DB4 to DB7 are used for data transfer

    EALLOW; // Edit Allow - Enable register edit

     // First configure all pins as GPIOs to peripheral mux
     //GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 0;
     GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0;
     GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;
     GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;
     GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;
     GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;
     GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;
     GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 0;

     // Next configure any properties like internal pullups, open-drain, or an inverted input signal

     // Select GPIO direction mode
     //GpioCtrlRegs.GPBDIR.bit.GPIO58 = 1;
     GPIO_OUTPUT(LCD_RS);
     GPIO_OUTPUT(LCD_RW);
     GPIO_OUTPUT(LCD_E);

     GPIO_OUTPUT(LCD_DB4);
     GPIO_OUTPUT(LCD_DB5);
     GPIO_OUTPUT(LCD_DB6);
     GPIO_OUTPUT(LCD_DB7);

     EDIS; // Edit Disallow

     waitbusy();

     command(LCD_FUNCTION_4BIT_2LINES); // 4-bit interface, dual line,   5x7 dots

     command(LCD_DISP_OFF);             // Display off

     clear();                           // Display clear
     command(LCD_MODE_DEFAULT);         // Set entry mode
     command(displayAttribute);         // Display/cursor control

}

void LCD :: E_toggle(void) {
    E_high();
    DELAY_US(1);
    E_low();
}

void LCD :: write(uint8 data, uint8 rs) {
    uint8 dataBits ;

    // Write data (RS=1, RW=0)
    if (rs) {
        RS_high();
    } else {
    // Write instruction (RS=0, RW=0)
        RS_low();
    }
    RW_low();

    EALLOW;
    // Configure data pins as output
    GPIO_OUTPUT(LCD_DB4);
    GPIO_OUTPUT(LCD_DB5);
    GPIO_OUTPUT(LCD_DB6);
    GPIO_OUTPUT(LCD_DB7);
    EDIS;

    // Clear all pins
    GPIO_CLEAR(LCD_DB4);
    GPIO_CLEAR(LCD_DB5);
    GPIO_CLEAR(LCD_DB6);
    GPIO_CLEAR(LCD_DB7);

    // Output high nibble first
    // Set pins if bit is 1
    if(data & 0x80) GPIO_SET(LCD_DB7);
    if(data & 0x40) GPIO_SET(LCD_DB6);
    if(data & 0x20) GPIO_SET(LCD_DB5);
    if(data & 0x10) GPIO_SET(LCD_DB4);

    E_toggle();

    // Output low nibble

    // Clear all pins
    GPIO_CLEAR(LCD_DB4);
    GPIO_CLEAR(LCD_DB5);
    GPIO_CLEAR(LCD_DB6);
    GPIO_CLEAR(LCD_DB7);

    if(data & 0x08) GPIO_SET(LCD_DB7);
    if(data & 0x04) GPIO_SET(LCD_DB6);
    if(data & 0x02) GPIO_SET(LCD_DB5);
    if(data & 0x01) GPIO_SET(LCD_DB4);

    E_toggle();

    // All data pins high (inactive)
    GPIO_SET(LCD_DB7);
    GPIO_SET(LCD_DB6);
    GPIO_SET(LCD_DB5);
    GPIO_SET(LCD_DB4);

}

uint8 LCD :: read(uint8 rs) {
    uint8 data;

    if (rs) {
        RS_high();  // Read data (RS=1, RW=1)
    } else {
        RS_low();   // Read busy flag (RS=0, RW=1)
    }
    RW_high();      // Read mode (RW=1)

    EALLOW;
    // Configure data pins as input
    GPIO_INPUT(LCD_DB4);
    GPIO_INPUT(LCD_DB5);
    GPIO_INPUT(LCD_DB6);
    GPIO_INPUT(LCD_DB7);
    EDIS;

    // Read high nibble first
    E_high();
    DELAY_US(1);
    data = 0;
    if ( GPIO_GET(LCD_DB4) ) data |= 0x10;
    if ( GPIO_GET(LCD_DB5) ) data |= 0x20;
    if ( GPIO_GET(LCD_DB6) ) data |= 0x40;
    if ( GPIO_GET(LCD_DB7) ) data |= 0x80;
    E_low();

    DELAY_US(1);

    // Read low nibble
    E_high();
    DELAY_US(1);
    if ( GPIO_GET(LCD_DB4) ) data |= 0x01;
    if ( GPIO_GET(LCD_DB5) ) data |= 0x02;
    if ( GPIO_GET(LCD_DB6) ) data |= 0x04;
    if ( GPIO_GET(LCD_DB7) ) data |= 0x08;
    E_low();

    return data;
}

uint8 LCD :: waitbusy(void) {
    uint8 temp;

    // Wait until busy flag is cleared
    while ( (temp=read(0)) & (1<<LCD_BUSY))

    DELAY_US(2); // The address counter is updated 4us after the busy flag is cleared

    return (read(0));  // Now read & return the address
}

void LCD :: newline(uint8 position) {
    uint8 addressCounter;

    if ( position < (LCD_START_LINE2) )
        addressCounter = LCD_START_LINE2;
    else
        addressCounter = LCD_START_LINE1;

    command((1<<LCD_DDRAM)+addressCounter);

}

void LCD :: command(uint8 command) {
    waitbusy();
    write(command,TYPE_INSTRUCTION);
}

void LCD :: data(uint8 data) {
    waitbusy();
    write(data,TYPE_DATA);
}

void LCD :: gotoxy(uint8 x, uint8 y) {
    if ( y==0 ) {
        command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
    } else {
        command((1<<LCD_DDRAM)+LCD_START_LINE2+x);
    }
}

uint8 LCD :: getxy(void) {
    return waitbusy();
}

void LCD :: clear(void) {
    command(1<<LCD_CLR);
}

void LCD :: home(void) {
    command(1<<LCD_HOME);
}

void LCD :: printChar(char character) {
    uint8 position;

    position = waitbusy();
    if (character=='\n'){
        newline(position);
    }else{

#if LCD_WRAP_LINES==1
        if ( position == LCD_START_LINE1+LCD_DISP_LENGTH ) {
            write((1<<LCD_DDRAM)+LCD_START_LINE2,0);
        }else if ( position == LCD_START_LINE2+LCD_DISP_LENGTH ){
            write((1<<LCD_DDRAM)+LCD_START_LINE1,0);
        }
        waitbusy();
#endif

        write(character, 1);
    }

}

void LCD :: print(const char *s) {
    char character;
    while ( (character = *s++) ) printChar(character);
}
