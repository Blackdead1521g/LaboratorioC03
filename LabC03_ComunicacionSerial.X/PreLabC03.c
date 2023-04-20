/* Archivo: PostLab02.c
 * Dispositivo: PIC16F887
 * Autor: Kevin Alarcón
 * Compilador: XC8(v2.40), MPLABX V6.05
 * 
 * 
 * Programa: Utilizar los PWM del pic y crear un nuevo PWM para la intensidad de un led
 * Hardware: Potenciómetros en RA0, RA2 y RA5; 2 servo motores en RC1 y RC2, 1 led en RC3
 * 
 * Creado: 10 de abril, 2023
 * Última modificación: 13 de abril, 2023
 */
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//---------------------Librerías----------------------------------
#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pic16f887.h>

#define _XTAL_FREQ 1000000 //definimos la frecuencia del oscilador

//---------------------Variables---------------------------------
char oracion[15] = "HOLA MUNDO \r";
const char dato = 'A';

//-------------------Prototipos de funciones------------------
void setup(void);
void initUART(void);
void cadena(char txt[]);

void __interrupt() isr(void){
    if (PIR1bits.RCIF)
    {
        PORTD = RCREG;
    }
    return;
}

void main(void) {
    setup (); 
    initUART();
    
    while(1){ //loop forever
        __delay_ms(500);
        cadena(oracion);
        /*if (PIR1bits.TXIF){//Revisamos si está disponible el EUSART
            TXREG = dato;
        }*/
    }
}

//Función para enviar caracteres
void cadena(char txt[]){
    for (uint8_t i = 0; txt[i] != '\0'; i++){
        while (!PIR1bits.TXIF); //Si TXIF esta disponible
        TXREG = txt[i];
    }
    return;
}

void setup(void){
    //definir digitales
    ANSEL = 0;
    ANSELH = 0; 
    
    //Definimos puertos que serán salidas
    TRISA = 0;
    TRISB = 0;
    TRISD = 0;
    TRISE = 0;
    
    //Limpiamos los puertos
    PORTA = 0;
    PORTB = 0;
    PORTD = 0;
    PORTE = 0;
    
    //////////////oscilador
    OSCCONbits.IRCF = 0b100; ///1Mhz
    OSCCONbits.SCS = 1; //Utilizar oscilados interno*/
    
    /////////Banderas e interrupciones
    INTCONbits.PEIE = 1; //Habilitar interrupciones periféricas
    INTCONbits.GIE = 1; //Habilitar interrupciones globales
     __delay_ms(1); 
    return;
}

void initUART(void){
    //Paso 1
    SPBRG = 25; //SPBRGH:SPBRG = [(1MHz/9600)/64] - 1 = 25 -> real 9615.38
    SPBRGH = 0; //%error = (9615 - 9600)/9600 = 16
    TXSTAbits.SYNC = 0; //TXSTAbits -> modo asíncronico habilitado
    TXSTAbits.BRGH = 1;
    BAUDCTLbits.BRG16= 1;
    //Paso 2
    RCSTAbits.SPEN = 1; //RCSTAbits -> habilita el modulo UART
    RCSTAbits.RX9 = 0;
    RCSTAbits.CREN = 1;
    //Paso 3: 9 bits
    
    //Paso 4
    //12.1.1.6 Asynchronous Transmiss
    TXSTAbits.TXEN = 1; //TXSTAbits -> habilitar transimision & TXIF = 1
    //TXIF = 0; //PIRbits -> apagar bandera TX
    //12.1.2.8
   
    
    //Paso 5 INTERRUPCIONES
    PIR1bits.RCIF = 0;
    PIE1bits.RCIE = 1;
    return;
}
