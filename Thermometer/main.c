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


//=============================================================================================
// LOCAK VARIABLES
//=============================================================================================

volatile unsigned int e_adcValue;       // Analog value * 10
volatile unsigned char e_displayBuf[4]; // Display buffer

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
    unsigned int adcBuf;
    unsigned char adcStr[16];
    unsigned char len;
    
    adcBuf = ADCL;
    adcBuf = (ADCH << 8) | adcBuf;
    
    e_adcValue = (adcBuf * 50) / 1023;
    sprintf(adcStr, "%d", e_adcValue);
    
    
    // Restarting AD conversion before exit
    StartConvAdc();
}


//---------------------------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------------------------

int main(void)
{
    unsigned char digit = 0;  // Digit position 0, 1, 2, 3
    unsigned char symbol;
	
    ADC_Init();
    USART_Init(BR_115200);

	/* Replace with your application code */
	while (1)
	{
        DisplaySymbol(digit, symbol);
        Delay(20);
	}
}

