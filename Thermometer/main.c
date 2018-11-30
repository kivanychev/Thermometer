/*
 * Thermometer.c
 *
 * Created: 25.11.2018 12:43:52
 * Author : ZorG
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>


//=============================================================================================
// CONSTANTS, MACROS
//=============================================================================================

#define StartConvAdc() ADCSRA |= (1<<ADSC)

#define LOWEST_VALUE

// Константы для работы с последовательным интерфейсом USART

#define BR_1M       0
#define BR_500K     1
#define BR_250K     3
#define BR_115200   8
#define BR_57600    16
#define BR_38400    25
#define BR_19200    51
#define BR_9600     103

#define LED_DIGITS  4

//=============================================================================================
// LOCAL VARIABLES
//=============================================================================================

volatile unsigned char e_displayBuf[4]; // Display buffer

unsigned char e_LED_Symbols[][2] = 
{
    {'0', 0b00000000 },
    {'1', 0b00000000 },
    {'2', 0b00000000 },
    {'3', 0b00000000 },
    {'4', 0b00000000 },
    {'5', 0b00000000 },
    {'6', 0b00000000 },
    {'7', 0b00000000 },
    {'8', 0b00000000 },
    {'9', 0b00000000 },
    {'-', 0b00000000 }

};


//=============================================================================================
// FUNCTION PROTOTYPES
//=============================================================================================



//=============================================================================================
// IMPLEMENTATION
//=============================================================================================

//---------------------------------------------------------------------------------------------
// Function:    Delay
// Parameters:  time in ms
//---------------------------------------------------------------------------------------------

void Delay(int time)
{
    // Start timer
    
    // Wait for CTC
    
}

//---------------------------------------------------------------------------------------------
// Function:    ADC_Init - Initializes ADC for Channel 0
//---------------------------------------------------------------------------------------------

void ADC_Init()
{
	// Initializing ADC:
	// Опорное напряжение подключить к Vcc
	// Лево-ориентированный результат,
	// включить канал AD0
	ADMUX = (1<<REFS0);

	// Turn on ADC, Single conversion mode, Enable ADC interrupts
	// Set conversion frequency to FCPU/128
	ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}

//---------------------------------------------------------------------------------------------
// Function:    USART_Init - Initializes USART for sending log messages
//---------------------------------------------------------------------------------------------

void USART_Init(unsigned int baudrate)
{
    // Data frame format: 8 Data, 1 Stop, No Parity
    // USART0 Transmitter: On
    // USART0 Mode: Asynchronous
    UCSRA = 0x00;

    UCSRB = (1 << TXEN);
    UCSRC = (1 << UCSZ1) + (1 << UCSZ0); // 8 bit data frame 

    UBRRH = baudrate >> 8;
    UBRRL = baudrate;
}

//---------------------------------------------------------------------------------------------
// Function:    Send symbol via USART
// Parameters:  ch -- the symbol
//---------------------------------------------------------------------------------------------

inline void USART_SendChar(unsigned char ch)
{
    // wait for data to be received
    while(!(UCSRA & (1<<UDRE)));
    // send data
    UDR = ch;
    
}

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
// Function	IO_Init() -- Initialize IO ports
//---------------------------------------------------------------------------------------------

IO_Init()
{
    
}


//---------------------------------------------------------------------------------------------
// DisplaySymbol():      Initialize ADC for Channel 0
//---------------------------------------------------------------------------------------------

DisplaySymbol(unsigned char digit, unsigned char symbol)
{
    
}


//---------------------------------------------------------------------------------------------
// ADC INTERRUPT HANDLER
//---------------------------------------------------------------------------------------------

ISR(ADC_vect)
{
    long v;
    long t;
    long AdcBuf;
    short strLen;
    unsigned char strBuf[16];

    AdcBuf = ADCL;
    AdcBuf = (ADCH << 8) | AdcBuf;

    v = (AdcBuf * 500) / 1024;
    
    t = ((v - V_MIN) * (T_MAX - T_MIN)) / (V_MAX - V_MIN) + T_MIN;
    sprintf(strBuf, "%d", t);

    // Count string len
    for(strLen = 0; strBuf[strLen] != 0; strLen++)
    ;
    
    // Clear LED buffer
    e_displayBuf[0] = 0;
    e_displayBuf[1] = 0;
    e_displayBuf[2] = 0;
    e_displayBuf[3] = 0;
    
    pos = LED_DIGITS - strLen;
    strPtr = &(strBuf[0]);
    
    //Put string
    for(; pos < LED_DIGITS ; ++pos) 
    {
        e_displayBuf[pos] = GetLedSymbol(*strPtr);
        strPtr++;
    }
    
    StartConvAdc();
}

//---------------------------------------------------------------------------------------------
// Function:    GetLedSymbol -- converts ASCII code to LED code
//---------------------------------------------------------------------------------------------
unsigned char GetLedSymbol(char s)
{
    unsigned char ledSymbol = (unsigned char)s;
    
    
    
    return ledSymbol;
}


//---------------------------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------------------------

int main(void)
{
    unsigned char digit = 0;  // Digit position 0, 1, 2, 3
    unsigned char symbol;
	
    IO_Init();
    ADC_Init();
    USART_Init(BR_115200);

	/* Replace with your application code */
	while (1)
	{
        DisplaySymbol(digit, symbol);
        Delay(20);
	}
}

