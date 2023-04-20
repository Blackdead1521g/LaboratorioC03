/* Archivo: PostLab03.c
 * Dispositivo: PIC16F887
 * Autor: Kevin Alarc�n
 * Compilador: XC8(v2.40), MPLABX V6.05
 * 
 * 
 * Programa: Crear un men� en la terminal que pueda presentar el valor de un potenciometro 
 * Hardware: Potenci�metro en RA0 y una terminal conectada
 * 
 * Creado: 17 de abril, 2023
 * �ltima modificaci�n: 20 de abril, 2023
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
char menu[] =  "Menu: \r 1)Leer potenciometro \r 2)enviar Ascii \r";
/******************************PROTOTIPOS***********************************/
void setup(void);
void initUART(void);
void cadena(char txt[]);
void POT(void);
void TXT(void);
/******************************INTERRUPCIONES*******************************/

void __interrupt() isr(void)
{
    if(PIR1bits.RCIF){ //Verificamos que la bandera del EUSART est� llena (ya recibi� un valor)
        opcion = RCREG; //Ingresamos el dato recibido desde la hiperterminal en la variable opcion
        //RCREG es el registro que contiene el valor que ingresamos en la hiperterminal
        switch(opcion){ //Hacemos una comprobaci�n de la variable opcion
            case '1': //Si eligieron la opcion 1
                POT(); //Mandamos a llamar a nuestras funciones
                cadena(menu); //Muetras el menu de nuevo
                break; 
            case '2':  //Si eligieron la opcion 2
                TXT();  //Mandamos a llamar a nuestras funciones
                cadena(menu); //Muetras el menu de nuevo
                break;
            default: //Si ingresa un valor diferente
                
                cadena("No existe esa opcion \r"); //
                while(RCREG != 13); //No sale del bucle hasta que opriman enter en la hiperterminal
                cadena(menu); //Muetras el menu de nuevo
        }   
        return;
    }
   
     if(ADIF) //Si se activa la bandera de interrupcion del ADC
    { 
        if (ADCON0bits.CHS == 0b0000)   //Si est� en ADC AN0
            Vpot = ADRESH; //Le ingresamos el valor del potenciometro en RA0 a la variable Vpot
            
       ADIF = 0; //Apagamos la bandera del ADC
    }
    
}

/******************************CODIGO PRINCIPAL*****************************/

void main(void){
    setup();
    initUART();
    cadena(menu);
  
    while(1){

        while(!PIR1bits.RCIF); //Espera a que la persona ingrese una opcion

        __delay_ms(500);
  
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
    PORTE = 0;
    

/*Reloj*/
    OSCCON = 0X75; //8 MHZ
    
/*interrupciones*/
    GIE = 1;    //Global enable
    PEIE = 1;   //periferic enable
    ADIE = 1;   //ADC enable
    ADIF = 0; //Limpiamos bandera del ADC
    
    //Configuraci�n ADC
    ADCON0bits.CHS = 0b0000; //Elegimos canal RA0 como inicial
    ADCON1bits.VCFG0 = 0; //Voltaje referenciado de 0V
    ADCON1bits.VCFG1 = 0; //Voltaje referenciado de 5V
    ADCON0bits.ADCS = 0b10; // Fosc/32
     
    ADCON1bits.ADFM = 0; //Justificado a la izquierda
    ADCON0bits.ADON = 1;//Encendemos el m�dulo del ADC
     __delay_ms(1); 
    
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
    TXEN = 1; //TXSTAbits -> habilitar transimision & TXIF = 1
    TXIF = 0; //PIRbits -> apagar bandera TX
    //C
    
    //paso 5: interrupciones
    PIE1bits.RCIE = 1; //Habilitamos interrupci�n de RCIF
    PIR1bits.RCIF = 0; //Limpiamos bandera del RCIF

    
    
    //paso 6: cargar 9no bit
    //paso 7: cargar 
    
    
}

void cadena(char txt[]){
    int i;  //variable iteracion
    
    for(i = 0; txt[i] != '\0'; i++){
        
        while(!PIR1bits.TXIF); // Esperamos a que el registro de transmisi�n est� vac�o
        //TXREG es el registro de la terminal, es decir, lo que posea ese registro se va a presentar en la terminal
        TXREG = txt[i]; // Envia caracter por caracter a la terminal
    }
    return;
}

void POT (void)
{       ADON = 1; //se enciende el ADC
        __delay_ms(10);
        ADCON0bits.GO = 1; //Empieza la conversion
       
        char CVPOT[4]; //Creamos un arreglo 
        sprintf(CVPOT, "%d\r", Vpot); //Pasa a el valor del potenciometro a caracter
        
        cadena("El valor del potenciometro es:\r"); //Presentamos el valor del potenciometro
        cadena(CVPOT);  //muestra el resultado
        ADON = 0;       //se apaga el ADC
         return;      
}

void TXT(void)
{
    while(!PIR1bits.RCIF); //Esperar a que presione un valor en la terminal
    char caracter = RCREG; //Le ingresamos el valor ingresado de la terminal a la variable caracter 
    TXREG = caracter; //Ese mismo valor luego se lo ingresamos el registro TXREG que presenta dicho valor en la terminal
    TXREG ='\r'; //Damos un salto en la terminal
    PORTB = caracter; //Le ingresamos al puerto B el valor de la terminal para poder observar en binario el valor decimal correspondiente 
                      //al caracter ingresado
}
