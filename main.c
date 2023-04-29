/* 
 * File:   main.c
 * Author: Pawankumar Lohar
 *
 */



// CONFIG
#pragma config FOSC = INTOSC    // Oscillator Selection bits (INTOSC oscillator: CLKIN function disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)
#pragma config LPBOR = OFF      // Brown-out Reset Selection bits (BOR disabled)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include<stdint.h>
/*
 * 
 */

#define _XTAL_FREQ 1000000

#define   LED    LATAbits.LATA1
#define   BUZZER LATAbits.LATA0

void timer_0_config(void);
void timer_2_config(void);
void clock_config(void);
void gpio_config(void);

void externalInterruptInit(void);
void hour_indication(uint8_t hour);
void start_sequence(void);

volatile uint16_t MsecCounter;
volatile uint16_t SecondCounter;
volatile uint8_t MinuteCounter;
volatile uint8_t HalfHourCounter;

volatile uint8_t HourCounter;
volatile uint8_t ShiftComplete;
volatile uint8_t ShiftStarted;
uint8_t TimerCounter;

void __interrupt() isr_routine(void) {

     if(PIR1bits.TMR2IF==1){
       
         PIR1bits.TMR2IF=0;
         MsecCounter++;
         
            if(MsecCounter == 1000){
                SecondCounter++;
                MsecCounter=0;
            }
                    if(SecondCounter == 60){
                        MinuteCounter++;
                        SecondCounter=0;
                    }
                        if(MinuteCounter == 30){
                            HalfHourCounter++;
                            hour_indication(HalfHourCounter);
                            MinuteCounter=0;   
                        }
                            
     }
    
    if(INTCONbits.TMR0IF==1){
        INTCONbits.TMR0IF=0;
        if(PORTAbits.RA2==0)
            TimerCounter++;
            if(TimerCounter==20){ //Equate with 20 because to get 5 Second 
                TimerCounter=0;
                ShiftStarted=1;    
            }
       } 

    
    if(INTCONbits.INTF==1){
        TimerCounter=0;
        INTCONbits.INTF=0;
        T2CONbits.TMR2ON=0;
        ShiftComplete = ShiftComplete==1 ? 0:255;
    }

}

void main() {
    
  clock_config();
  gpio_config();
  timer_0_config(); 
  externalInterruptInit() ;
  timer_2_config();
  
  while(1){
      
        if(ShiftStarted == 1){
            ShiftStarted=0;
            start_sequence();
            timer_2_config();
        }
            
            if (ShiftComplete==1){
                LED=1;
                BUZZER=1;
                __delay_ms(150);
                LED=0;
                BUZZER=0;
                __delay_ms(150);
        }
    }

}

void gpio_config(void){
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 1;
    LATAbits.LATA0 = 0;
    
    ANSELAbits.ANSA0 = 0;
    ANSELAbits.ANSA1 = 0;
    ANSELAbits.ANSA2 = 0;
    OPTION_REGbits.nWPUEN =0;
    WPUAbits.WPUA0 = 0;
    WPUAbits.WPUA1 = 1;
    WPUAbits.WPUA2 = 1;
    WPUAbits.WPUA3 = 0;
    
}

void timer_0_config(void){
    
       
    TMR0=0;
    
    INTCONbits.TMR0IF=0;
    INTCONbits.TMR0IE=1;
    OPTION_REGbits.T0CS=0;
    OPTION_REGbits.PSA=0;
    OPTION_REGbits.PS0=1;
    OPTION_REGbits.PS1=1;
    OPTION_REGbits.PS2=1;
    
}

void timer_2_config(void){
    
    INTCONbits.PEIE = 1;
    T2CONbits.TMR2ON=0;
    PIR1bits.TMR2IF=0;
    T2CONbits.T2CKPS0=0;
    
    T2CONbits.T2CKPS1=0;
    
    T2CONbits.TOUTPS3=0;
    T2CONbits.TOUTPS2=0;
    T2CONbits.TOUTPS1=0;
    T2CONbits.TOUTPS0=0;
    
    PIE1bits.TMR2IE =1;
    PR2=0xff;
    T2CONbits.TMR2ON=1;
    
}

void clock_config(void){
    
    OSCCONbits.IRCF2=0;
    OSCCONbits.IRCF1=1;
    OSCCONbits.IRCF0=1;
    //CLKRCONbits.CLKROE=1;
}

void externalInterruptInit(void){
    
    INTCONbits.PEIE = 1;
    INTCONbits.INTE = 1;
    OPTION_REGbits.INTEDG = 0;
    INTCONbits.INTF = 0;
    INTCONbits.GIE = 1;
}

void start_sequence(void){
    
    for(uint8_t i=0; i<20;i++){
        LED=1;
        BUZZER=1;
        __delay_ms(50);
        LED=0;
       BUZZER=0;
        __delay_ms(50);
    }
}

void hour_indication(uint8_t hour){
    
    
        if((hour%2)==0){
            
                LED=1;
                BUZZER=1;
                __delay_ms(500);
                LED=0;
                BUZZER=0;   
        }
     
        
        if(hour==19){
           ShiftComplete=1;
           HalfHourCounter=0;
        }
    
}