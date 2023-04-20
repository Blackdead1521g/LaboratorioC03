/* 
 * File:   UART.c
 * Author: USUARIO FINAL
 *
 * Created on 16 de abril de 2023, 08:22 PM
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

/*******************************LIBRERIAS***********************************/
#include <stdio.h>
#include <xc.h>
#include <stdint.h>
#include <stdlib.h>
#include <pic16f887.h>


/******************************DEFINIR CONSTANTES***************************/
#define _XTAL_FREQ 8000000


/******************************Variables Globlales**************************/
uint8_t num;
char opcion;
uint8_t Vpot;
char menu[] =  "\rMenu: \r 1)Leer potenciometro \r 2)enviar Ascii \r";
/******************************PROTOTIPOS***********************************/
void setup(void);
void initUART(void);
void cadena(char txt[]);
void POT(void);
void TXT(void);
/******************************INTERRUPCIONES*******************************/

void __interrupt() isr(void)
{
    if(PIR1bits.RCIF){
        opcion = RCREG;

        return;
    }
   
     if(ADIF) //activa bandera de conversion
    { 
        if (ADCON0bits.CHS == 0b0000)   //si estamos en el canal principal
            Vpot = ADRESH;
            
       ADIF = 0; //apagamos la bandera  
    }
    
}

/******************************CODIGO PRINCIPAL*****************************/

void main(void){
    setup();
    initUART();
   // cadena(menu);
  
    while(1){
        cadena(menu);
        opcion = '\0';
        
       while(opcion == '\0'); //espera a que la persona ingrese una opcion

                switch(opcion){
            case '1':   //valor potenciometro
                POT();
                break;
                
            case '2':   //ascii
                TXT();
                break;
                
            default:    //opcion no existente
                cadena("No existe esa opcion, presione enter para continuar \r");
                while(RCREG != 13); //espera el enter
             
        }   
       
        __delay_ms(100);
  
    }
       
    return;
}
/*******************************Funciones***********************************/
void setup(void)
{/*IO*/
    ANSEL = 0x1;     //RA1 = analogico
    ANSELH = 0;
    TRISA = 0x1;    //RA1 = entrada
    TRISB = 0;
    TRISD = 0;
    TRISE = 0;
    
    //limpieza puertos
    PORTA = 0;
    PORTB = 0;
    PORTD = 0;
    

/*Reloj*/
    OSCCON = 0X75; //8 MHZ
    
/*interrupciones*/
    GIE = 1;    //Global enable
    PEIE = 1;   //periferic enable
    ADIE = 1;   //ADC enable
    
    ADIF = 0;
    
/*ADC config*/
    ADCON0bits.ADON = 1; //activamos el ADC
    
    ADCON0bits.CHS = 0b0000; //AN0
      __delay_ms(10); //delay
    
    ADCON1bits.ADFM = 0;    //just izq
    
    ADCON1bits.VCFG1 = 0;      //VSS
    ADCON1bits.VCFG0 = 0;      //VDD
    
    ADCON0bits.ADCS1 = 1;
    ADCON0bits.ADCS0 = 0;       //FOSC/32
    
    return;
    
}

void initUART(void){
    //paso 1
    SPBRG = 12; //SPBRGH:SPBRG  = [(8Mhz/9600)/64]-1 = 12 ? real 9615.38
    SPBRGH = 0; //%error = (9615.38-9600)/9600 * 100 = 0.16%
    BRGH = 0;   
    BRG16 = 0;
    
    //paso 2
    CREN = 1;
    SYNC = 0;   // TXSTAbits ? habilitar transmision & TXIF = 1
    SPEN = 1;   //RCSTAbits ? apagar bandera TX
    TXSTAbits.TXEN = 1; //permite transmitir
    
    //paso 3: habilitar los 9 bits
    RCSTAbits.RX9 = 0;
    
    //paso 4
    TXEN = 1;
    TXIF = 0;
    //C
    
    //paso 5: interrupciones
    PIE1bits.RCIE = 1;
    PIR1bits.RCIF = 0;

    
    
    //paso 6: cargar 9no bit
    //paso 7: cargar 
    
    
}

void cadena(char txt[]){
    int i;  //variable iteracion
    
    for(i = 0; txt[i] != '\0'; i++){
        
        while(!PIR1bits.TXIF); // Esperamos a que el registro de transmisión esté vacío
        
        TXREG = txt[i]; // envia caracter por caracter a la terminal
    }
    return;
}

void POT (void)
{
        __delay_ms(10);
        ADCON0bits.GO = 1; //empieza la conversion
        while( ADCON0bits.GO == 1);
        char CVPOT[4];
        sprintf(CVPOT, "%d\r", Vpot); //pasa a caracter el valor que leyo
        
        cadena("El valor del potenciometro es: ");
        cadena(CVPOT);  //muestra el resultado
   
         return;      
}

void TXT(void){
    cadena("Ingrese su caracter: ");
                opcion = '\0';
                while(opcion == '\0'); //esperar a que presione
                PORTB = opcion; 
                TXREG = opcion;
                TXREG = '\r';
                return;
}

