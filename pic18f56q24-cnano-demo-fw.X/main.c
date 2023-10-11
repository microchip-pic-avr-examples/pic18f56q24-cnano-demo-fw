/*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
 */

/*
© [2022] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
 */

#include "mcc_generated_files/system/system.h"
#include <string.h>
#include <stdio.h>

#define clrscr() printf("\e[1;1H\e[2J")
#define BLINK_DELAY         500
#define RAMP_DELAY          300
#define DV_START_VALUE      0x03
#define DV_STOP_VALUE       0xFC
typedef uint8_t flash_data_t;
typedef uint24_t flash_address_t;


uint16_t ADC_MEAS;
int24_t Temp_in_C;
int16_t H_gain;
uint16_t H_ADC;
int16_t H_offset;
int16_t H_FVR;

/*
    Main application
 */

typedef enum {
    BLINKY, BUTTON, RAMP, TEMP, SECRET
} programs_t;

static uint8_t Unblocking_Read(void);
static void Print_Menu(void);
static void CLI_Execute_Command(uint8_t);
static void CLI_Run(void);
static void DEMO_Run(void);
static void Blink_LED(void);
static void Button_LED(void);
static void TEMP_sensor(void);
static void Send_DataVisualizer_Data(char txData);
static void Send_DataVisualizer_Ramp(void);
flash_data_t FLASH_Read(flash_address_t address);
int16_t NVM_READ(uint24_t WORD_ADDR);

static programs_t current_program = BLINKY;

int main(void) {
    SYSTEM_Initialize();
    clrscr();
    Print_Menu();
    __delay_ms(50);

    while (1) {
        CLI_Run();
        DEMO_Run();
    }
}

int16_t NVM_READ(uint24_t WORD_ADDR) {


    NVMADR = WORD_ADDR;
    NVMCON1bits.CMD = 0x00;
    NVMCON0bits.GO = 1;
    while (NVMCON0bits.GO);

    return NVMDAT;
}


static uint8_t Unblocking_Read(void) {
    if (UART2_IsRxReady() != 0) /* boolean */ {
        return UART2_Read();
    } else {
        return '\0';
    }
}

static void Print_Menu(void) {
    __delay_us(200);
    printf("\r\n");
    printf("********************************************************************************************************\r\n");
    printf("*                       Out of the box application for PIC18F56Q24 Curiosity Nano                      *\r\n");
    printf("********************************************************************************************************\r\n");
    printf("*        MMMMMMMMMNddhysyyyysyyhdmMMNNNMMMMM                                                           *\r\n");
    printf("*        MMMMMMmhssssssssssssssssshMNmmMMMMM                                                           *\r\n");
    printf("*        MMMMdyssssssssssssssssssssyNMMMMMMM                                                           *\r\n");
    printf("*        MMmyssssso-`.ossssssso-.-osymMMMMMM                                                           *\r\n");
    printf("*        Mdssssss/`   `/sssss:    `+ssdMMMMM       __  __ _                     _     _                *\r\n");
    printf("*        mssssss/       -sss-       /ssyNMMM      |  \\/  (_)                   | |   (_)               *\r\n");
    printf("*        yssssoso-       .oso.       -ssymMM      | \\  / |_  ___ _ __ ___   ___| |__  _ _ __           *\r\n");
    printf("*        ysso- -ss/`      `/ss-       .ossdM      | |\\/| | |/ __| '__/ _ \\ / __| '_ \\| | '_ \\          *\r\n");
    printf("*        ys+`   .os+`       :ss/`      `/ssh      | |  | | | (__| | | (_) | (__| | | | | |_) |         *\r\n");
    printf("*        y:      `+so-       .os+`       :sh      |_|  |_|_|\\___|_|  \\___/ \\___|_| |_|_| .__/          *\r\n");
    printf("*        m`      -osss/       -sso-       /M                                           | |             *\r\n");
    printf("*        Mh`   `/ssssss+`    :sssss/     -NM                                           |_|             *\r\n");
    printf("*        MMm: -osssssssso-`.+sssssss+`  +NMM                                                           *\r\n");
    printf("*        MMMMhysssssssssssssssssssssso+mMMMM                                                           *\r\n");
    printf("*        MMMMMMNdysssssssssssssssssydNMMMMMM                                                           *\r\n");
    printf("*        MMMMMMMMMNmdhyyysssyyhhdmMMMMMMMMMM                                                           *\r\n");
    printf("********************************************************************************************************\r\n");
    printf("*                                                                                                      *\r\n");
    printf("* -Press a key corresponding to a program:                                                             *\r\n");
    printf("*  '1' - Default program; Blinking LED                                                                 *\r\n");
    printf("*  '2' - LED turns on while button is pressed                                                          *\r\n");
    printf("*  '3' - An ascending ramp visible in MPLAB Data Visualizer sent through UART while button is pressed  *\r\n");
    printf("*  '4' - Temperature sensor measurement taken with every button press                                  *\r\n");
    printf("*                                                                                                      *\r\n");
    printf("* - Press any other key to view the menu again.                                                        *\r\n");
    printf("*                                                                                                      *\r\n");
    printf("********************************************************************************************************\r\n");
}

static void CLI_Execute_Command(uint8_t command) {
    switch (command) {
        case '1':
            current_program = BLINKY;
            printf("APPLICATION blink LED\r\n");
            break;
        case '2':
            LED_SetHigh();
            current_program = BUTTON;
            printf("APPLICATION press button to turn on LED\r\n");
            break;
        case '3':
            LED_SetHigh();
            current_program = RAMP;
            printf("APPLICATION press button to send a ramp through UART\r\n");
            printf("Please refer to README if your having troubles setting the DVRT.\r\n");
            break;
        case '4':
            current_program = TEMP;
            printf("APPLICATION TEMP sensor \r\n");
            printf("TEMP sensor measurement is indicative of the DIE and not ambient temp.\r\n");
            break;
        default:

            Print_Menu();
            break;
    }
}

static void CLI_Run(void) {
    uint8_t ch = 0;

    ch = Unblocking_Read();

    if ((ch != '\n') && (ch != '\r') && (ch != '\0')) {
        CLI_Execute_Command(ch);
    }
}

static void DEMO_Run(void) {
    if (current_program == BLINKY) {
        Blink_LED();
    } else if (current_program == BUTTON) {
        Button_LED();
    } else if (current_program == RAMP) {
        Send_DataVisualizer_Ramp();
    } else if (current_program == TEMP) {
        TEMP_sensor();
    } else if (current_program == SECRET) {
    }
}

static void Blink_LED(void) {
    LED_Toggle();
    __delay_ms(BLINK_DELAY);
}

static void Button_LED(void) {
    if (Button_GetValue() == 0) {
        LED_SetLow();
    } else {
        LED_SetHigh();
    }
}

static void Send_DataVisualizer_Data(char txData) {
    while (!(UART2_IsTxReady()));
    UART2_Write(txData);
}

static void Send_DataVisualizer_Ramp(void) {
    static uint8_t counter = 0;

    if (!Button_GetValue() == 1) {
        Send_DataVisualizer_Data(DV_START_VALUE);
        Send_DataVisualizer_Data(counter++);
        Send_DataVisualizer_Data(DV_STOP_VALUE);
        __delay_ms(RAMP_DELAY);
    }
}

static void TEMP_sensor(void) {

    //Setup ADC and Temp Sensor
    

    H_gain = NVM_READ(DIA_TSHR1);
    H_offset = NVM_READ(DIA_TSHR3);
    H_ADC = NVM_READ(DIA_TSHR2);
    
    
    while (1) {

        if (PORTFbits.RF3 == 0) {
  
            ADC_MEAS = ADCC_GetSingleConversion(channel_Temp); // Store the ADC Result

            //DATASHEET SOLUTION
            Temp_in_C = (int24_t) (ADC_MEAS) * H_gain;
            Temp_in_C = (Temp_in_C / 256);
            Temp_in_C = (Temp_in_C + H_offset);
            Temp_in_C = (Temp_in_C / 10);
            
          
            printf("Temp of the die in C: %d \r\n", Temp_in_C);

            __delay_ms(500);

            break;
        }
        break;
    }

}

/**
    End of File
 *  */