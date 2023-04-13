

#define _XTAL_FREQ 20000000

#define Trigger RB1 //34 is TriggeR
#define Echo RB2//35 is Echo 
#define lcd PORTD
#define rs RC0
#define en RC1

#include <xc.h>

#pragma config FOSC = HS   
#pragma config WDTE = OFF  
#pragma config PWRTE = OFF 
#pragma config BOREN = ON 
#pragma config LVP = OFF   
#pragma config CPD = OFF   
#pragma config WRT = OFF  
#pragma config CP = OFF

                      

//LCD Functions Developed by Circuit Digest.

int on_time ;//= 150; //On-Time for the PWM signal

int count; //count gets incremented for every timer overlap

int pot_value;

int d4,d5,d6;

int angle;

int i=0;
unsigned char f=0;
int A1,A2,A3;

   

/***************Function Prototyping*******************/
void lcdinit();
void lcdcmd(unsigned char);
void pulse();
void lcddat(unsigned char);
void lcddisplay(char*);
void servo();

unsigned int counter = 0;

/****************Function Declaration******************/

void __interrupt() timer_isr()

{  

    if(TMR0IF==1) // Timer has overflown

    {

        TMR0 = 252;     /*Load the timer Value, (Note: Timervalue is 101 instaed of 100 as the

                         TImer0 needs two instruction Cycles to start incrementing TMR0 */

        TMR0IF=0;       // Clear timer interrupt flag

        count++;

    } 

    

    if (count >= on_time)

    {

        RC4=1;  // complement the value for blinking the LEDs

    }

    

    if (count >= 200)

    {

        RC4=0;

        count=0;
    }

}
void lcdinit(){
    lcd = 0x00;
    lcdcmd(0x38);
    lcdcmd(0x01);
    lcdcmd(0x0c);
    lcdcmd(0x06);
}

void pulse(){
    en = 1;
    __delay_ms(5);
    en = 0;
}

void lcdcmd(unsigned char cmd){
    lcd = cmd;
    rs = 0;
    pulse();
}

void lcddat(unsigned char cmd){
    lcd = cmd;
    rs = 1;
    pulse();
}

void lcdstring(char *s){
    while (*s != '\0'){
        lcddat(*s++);
    }
}

int time_taken;

int distance;

char t1,t2,t3,t4,t5;

char d1,d2,d3;
void intial_inter(){
    
    TRISC4=0;
    OPTION_REG = 0b00000100;  // Timer0 with external freq and 32 as prescaler

    TMR0=252;       // Load the time value for 1us delayValue can be between 0-256 only

    TMR0IE=1;       //Enable timer interrupt bit in PIE1 register

    GIE=1;          //Enable Global Interrupt

    PEIE=1;         //Enable the Peripheral Interrupt
}


void dista_cal(){
        TMR1H =0; TMR1L =0; //clear the timer bits

        

        Trigger = 1; 

        __delay_us(10);           

        Trigger = 0;  

        

        while (Echo==0);

            TMR1ON = 1;

        while (Echo==1);

            TMR1ON = 0;

        

        time_taken = (TMR1L | (TMR1H<<8)); 

        distance= ((0.0272*time_taken)/2)+2;

            

        time_taken = time_taken * 0.8;
      
       
        
        if(distance>=3&&distance<50){
            f=1;
            
        d1 = (distance/100)%10;

        d2 = (distance/10)%10;

        d3 = (distance/1)%10;
        lcdcmd(0x01);
        lcdcmd(0X80);
        lcdstring(" Object Detected ");
        __delay_ms(100);
	lcdcmd(0x01);
	
	A1=angle/100;
        int p= angle%100;
        A2=p/10;
        int n= p%10;
        A3= n/1;
        
        lcdcmd(0X80);
        lcdstring("distance:");
        lcddat(d1+'0');
        lcddat(d2+'0');
        lcddat(d3+'0'); 
        __delay_ms(100);
	
	lcdcmd(0xC0);
        lcdstring("ANGLE: ");
        lcddat(A1+0x30);
        lcddat(A2+0x30);
        lcddat(A3+0x30);
        __delay_ms(200);
        }
        else{
            
        f=0;
        lcdcmd(0X01);    
        lcdcmd(0X80);
        lcdstring(" No Object ");
        __delay_ms(100);
        }
}

void servo(){
  while(i<=1023){
        dista_cal();
        if(f==1){
            break;
        }
        pot_value = i*0.039;
        on_time = (170-pot_value);
        angle= i*0.175953;
        __delay_us(0.5);                
        i=i+50;
        }
        while(i>=0){
        dista_cal();
        if(f==1){
            break;
        }
        pot_value = i*0.039;
        on_time = (170-pot_value);
        angle= i*0.175953;
        __delay_us(0.5);
        i=i-50;
        }
}
int main()

{
    intial_inter();
    TRISD = 0x00; //PORTD declared as output for interfacing LCD
  
    TRISC0 = 0;
    TRISC1 = 0;

    TRISB1 = 0; //Trigger pin of US sensor is sent as output pin
    TRISB2 = 1; //Echo pin of US sensor is set as input pin       

   

    

    T1CON=0x20;

    lcdinit();

    

    lcdcmd(0x81);

    lcdstring("Ultrasonic Radar");

    lcdcmd(0xC4);

    lcdstring("System");

    __delay_ms(500);

    lcdcmd(0X01);

    lcdcmd(0x82);

    lcdstring("SEARCHING...");
    __delay_ms(1000);
    

    while(1)

    { 

        
        servo();
       
        
    }

}


 