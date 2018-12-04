/*
 * Thermometer.c
 *
 * Created: 25.11.2018 12:43:52
 * Author : Kirill Ivanychev
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

//=============================================================================================
// MACROS
//=============================================================================================

#define StartConvAdc() ADCSRA |= (1<<ADSC)

//=============================================================================================
// CONSTANTS
//=============================================================================================

// USART baud rates

#define BR_1M       0
#define BR_500K     1
#define BR_250K     3
#define BR_115200   8
#define BR_57600    16
#define BR_38400    25
#define BR_19200    51
#define BR_9600     103

// Thermo sensor parameters
#define V_MIN   25
#define V_MAX   475
#define T_MIN   -500
#define T_MAX   1500

#define LED_DIGITS  4
#define DIGIT1      0
#define DIGIT2      1
#define DIGIT3      2
#define DIGIT4      3

#define DIGIT_DELAY 6000
#define TRANSISTOR_DELAY 2000

// Number of frames to display the measured value
#define FRAMES_CNT  200

//=============================================================================================
// LOCAL VARIABLES
//=============================================================================================

// LED indicators buffer
volatile unsigned char e_LedBuf[LED_DIGITS]; 

unsigned char e_LED_Symbols[][2] = 
{
    {'0', 0b11111100 },
    {'1', 0b01100000 },
    {'2', 0b11011010 },
    {'3', 0b11110010 },
    {'4', 0b01100110 },
    {'5', 0b10110110 },
    {'6', 0b10111110 },
    {'7', 0b11100000 },
    {'8', 0b11111110 },
    {'9', 0b11110110 },
    {'-', 0b00000010 },
    {'.', 0b00000001 },
    
    {'Z', 0b00000000 }      // Symbol table terminating code

};

//=============================================================================================
// FUNCTION PROTOTYPES
//=============================================================================================

void USART_SendChar(unsigned char ch);

//=============================================================================================
// DEVICE HW SPECIFIC FUNCTIONS
//=============================================================================================

//---------------------------------------------------------------------------------------------
// Function:    ADC_Init - Initializes ADC for Channel 0
//---------------------------------------------------------------------------------------------

void ADC_Init()
{
    // Initializing ADC:
    // Ref to Vcc: 5V -- is the maximum measured value
    // Left adjusted result,
    // Use channel AD0
    ADMUX = (1 << REFS0);

    // Turn on ADC, Single conversion mode, Enable ADC interrupts
    // Set conversion frequency to FCPU/128
    ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

}

//---------------------------------------------------------------------------------------------
// Function	IO_Init() -- Initialize IO ports
//---------------------------------------------------------------------------------------------

void IO_Init()
{
    // Transistors port: PB0..PB3 for output
    DDRB =  0b00001111;

    // Turn all indicators off (1 - off)
    PORTB = 0b00001111;
    
    // Indicator port: All for output
    DDRC = 0xFF;
    
    // Initial value for indicator
    PORTC = 0;
    
}


//---------------------------------------------------------------------------------------------
// Function:     USART_Init -- Initialize USART
// Parameters:   baudrate
//---------------------------------------------------------------------------------------------

void USART_Init(unsigned int baudrate)
{
    // Data frame format: 8 Data, 1 Stop, No Parity
    // USART0 Transmitter: On
    // USART0 Mode: Asynchronous
    UCSR0A = 0x00;

    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) + (1 << UCSZ00);

    UBRR0H =baudrate >> 8;
    UBRR0L = baudrate;
}

//---------------------------------------------------------------------------------------------
// Function:    Send symbol via USART
// Parameters:  ch -- the symbol
//---------------------------------------------------------------------------------------------

inline void USART_SendChar(unsigned char ch)
{
    // wait for data to be received
    while(!(UCSR0A & (1<<UDRE0)));
    // send data
    UDR0 = ch;

}

//---------------------------------------------------------------------------------------------
// Function:    Delay - time delay for
// Parameters:  
//---------------------------------------------------------------------------------------------

void Delay(unsigned int time)
{
    for(unsigned int t = 0; t < time; ++t)
    {
        ;
    }
}

//=============================================================================================
// THERMOMETER FUNCTIONS
//=============================================================================================

//---------------------------------------------------------------------------------------------
// Function:    USART_SendStr - Sends string via USART
// Parameters:  str -- pointer to the string to be sent
//---------------------------------------------------------------------------------------------

void USART_SendStr(char *str)
{
    static unsigned char ind;

    //    cli();
    for(ind = 0; str[ind] != '\0'; ++ind)
    {
        USART_SendChar(str[ind]);

        if(str[ind] == '\n')
        break;
    }
    //    sei();
}

//---------------------------------------------------------------------------------------------
// Function:    GetLedSymbol -- converts ASCII code to LED code
// Returns:     LED symbol code or the ASCII code itself
//---------------------------------------------------------------------------------------------

unsigned char GetLedSymbol(char s)
{
    unsigned char ledSymbol = (unsigned char)s;

    // Se    
    for(int i = 0; e_LED_Symbols[i][0] != 'Z'; ++i)
    {
        if(e_LED_Symbols[i][0] == s)
        {
            ledSymbol = e_LED_Symbols[i][1];
        }
    }    

    return ledSymbol;
}


//---------------------------------------------------------------------------------------------
// DisplaySymbol():     Initialize ADC for Channel 0
//                      symbol - LED symbol to display
//                      digit -  digit number (DIGIT1 ... DIGIT4)
//---------------------------------------------------------------------------------------------

void DisplaySymbol(unsigned char digit, unsigned char symbol)
{
    unsigned char transistor = (0xFF ^ (1 << digit));
    
    // Turn off all transistors
    PORTB = 0xFF;
    Delay(TRANSISTOR_DELAY);
    
    // Show symbol on the desired transistor
    PORTB = transistor;
    PORTC = symbol;
}


//=============================================================================================
// MAIN PROGRAM
//=============================================================================================

int main(void)
{
    unsigned char primaryLEDbuf[LED_DIGITS];
    
    IO_Init();
    ADC_Init();
    USART_Init(BR_115200);

    sei();
    StartConvAdc();

    USART_SendStr("Thermometer\r\n");

    // Checking pressed buttons
    while(1)
    {
        // Copy LED buffer to primary LED buffer
        cli();
        primaryLEDbuf[DIGIT1] = e_LedBuf[DIGIT1];
        primaryLEDbuf[DIGIT2] = e_LedBuf[DIGIT2];
        primaryLEDbuf[DIGIT3] = e_LedBuf[DIGIT3];
        primaryLEDbuf[DIGIT4] = e_LedBuf[DIGIT4];
        sei();
        
        for(int frame = 0; frame < FRAMES_CNT; ++frame)
        {
            DisplaySymbol(DIGIT1, primaryLEDbuf[DIGIT1]);
            Delay(DIGIT_DELAY);

            DisplaySymbol(DIGIT2, primaryLEDbuf[DIGIT2]);
            Delay(DIGIT_DELAY);
        
            DisplaySymbol(DIGIT3, primaryLEDbuf[DIGIT3]);
            Delay(DIGIT_DELAY);

            DisplaySymbol(DIGIT4, primaryLEDbuf[DIGIT4]);
            Delay(DIGIT_DELAY);
        }        
        
    }

    return 0;
}


//---------------------------------------------------------------------------------------------
// INTERRUPT HANDLERS
//---------------------------------------------------------------------------------------------

ISR(ADC_vect)
{
    long v;
    long t;
    long AdcBuf;
    short strLen;
    short pos;
    char strBuf[16];
    unsigned char *strPtr;      // Pointer to the LED string
    short resultDigitsCount;    // Number of digits in the sprintf text

    AdcBuf = ADCL;
    AdcBuf = (ADCH << 8) | AdcBuf;

    v = (AdcBuf * 500) / 1024;
    
    t = ((v - V_MIN) * (T_MAX - T_MIN)) / (V_MAX - V_MIN) + T_MIN;
    sprintf(strBuf, "%ld", t);
    
    // Count string len
    for(strLen = 0; strBuf[strLen] != 0; strLen++)
        ;
    
    // Clear LED buffer
    e_LedBuf[DIGIT1] = 0;
    e_LedBuf[DIGIT2] = 0;
    e_LedBuf[DIGIT3] = 0;
    e_LedBuf[DIGIT4] = 0;
    
    // Count digits
    resultDigitsCount = 0;
    for(pos = 0; pos < LED_DIGITS; ++pos)
    {
        if(strBuf[pos] >= '0' && strBuf[pos] <= '9')
        {
            resultDigitsCount++;
        }
    }
    
    //Convert string to LED buffer
    pos = LED_DIGITS - strLen;
    strPtr = (unsigned char *) &(strBuf[0]);
    
    for(; pos < LED_DIGITS ; ++pos) 
    {
        e_LedBuf[pos] = GetLedSymbol(*strPtr);
        strPtr++;
    }

    //Insert additional '0' symbol if there is only 1 digit
    if(resultDigitsCount == 1)
    {
        e_LedBuf[DIGIT1] = e_LedBuf[DIGIT2];
        e_LedBuf[DIGIT2] = e_LedBuf[DIGIT3];
        e_LedBuf[DIGIT3] = GetLedSymbol('0');
    }
    
    // Place '.' at digit 3
    e_LedBuf[DIGIT3] |= GetLedSymbol('.');
    
    StartConvAdc();
}

//---------------------------------------------------------------------------------------------
// TIMER 0 Interrupt handler
//---------------------------------------------------------------------------------------------

ISR(TIMER0_OVF_vect)
{

}
