//>
//
//$ sudo chmod a+rw /dev/ttyUSB0
//

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define hall A7
#define trimmer A6
#define pwmOut 3






int time0=0;
int seconds = 0;
int minutes=0;
int hours=0;
int tick=0;

void setup() {

sbi(ADCSRA,ADPS2) ; //configure adc to highspeed and lowish precision
cbi(ADCSRA,ADPS1) ;
cbi(ADCSRA,ADPS0) ;

pinMode(hall, INPUT);
pinMode(trimmer, INPUT);
pinMode(pwmOut, OUTPUT);



//Timer 2 init (pwmOut)
timer2_init();
//Timer1 init( timebase)
timer1_init();



Serial.begin(9600);


delay(1000); 


}

int16_t counter1;
uint16_t hall1;
uint16_t trimmer1;

void loop() {
  // put your main code here, to run repeatedly:

delay(1000);            // waits for a second
counter1 +=1;
//Serial.println(counter1);


Serial.print("Hallsensor:");
Serial.println(hall1);
Serial.print("Trimmer");
Serial.println(trimmer1);

 

   
    

}

// void timer2_init() //funktionierende reserve
// {
//   cli();
//   TCCR2A = ((1 << WGM21) | (1 << COM2A0));// CTC normal port operation
//   //TCCR2B = (1 << WGM22); this bit switches toggle on compare match on 
//   TCCR2B = (4 << CS20); // prescaler
//   TIMSK2 = 0;
//   OCR2A = 0;
//   sei();
// }
void timer2_init()
{
 
  //Timer 2 fast PWM Tickle
  TCCR2A = (1<<COM2B1) + (1<<WGM21) + (1<<WGM20); // Set OC2B at bottom, clear OC2B at compare match
  TCCR2B = (1<<CS21)+(1<<WGM22); // prescaler = 8
  OCR2A = 255;
  OCR2B = 200; //PWM: 255=100%
  DDRD |= (1<<PD3);

}

void timer1_init()
{
  cli();
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  
  //OCR1A = 15624;// entspricht 1 Hz  bei 1024 Presclr
  //TCCR1B |= (1<<CS10) | (1<<CS12);// presclr = 1024
  
  OCR1A = 1600; //should equate to 10kHz
  TCCR1B |= (1<<CS10) ;// presclr = 1
  
  TCCR1B |= (1<<WGM12); //CTC
  
  
  //initialize counter value to 0;
  TCNT1  = 0;
  
 
  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  sei();
}
ISR(TIMER2_COMPA_vect)
{

}
ISR(TIMER1_COMPA_vect) //Interrupt at frequency of 1 Hz
{
  cli();
  hall1=    analogRead(hall);
  trimmer1= analogRead(trimmer);
  if(hall1 < trimmer1)
  {
    OCR2B = 255;
  }
  else
  {
    OCR2B = 0;
  }
  sei();
}
