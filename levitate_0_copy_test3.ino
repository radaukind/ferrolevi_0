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

int16_t  counter1;
uint16_t hall1;     //adc values
uint16_t trimmer1;  //adc values
int16_t      error;

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

//Function prototypes

//Funktion prototypes

//Serial.begin(9600); default
Serial.begin(115200);



}



void loop() {

  // put your main code here, to run repeatedly:

delay(10);            // waits for a second
counter1 +=1;
//Serial.println(counter1);


// Serial.print("Hallsensor:");
// Serial.println(hall1);
// Serial.print("Trimmer");
// Serial.println(trimmer1);

  Serial.print(hall1);
  Serial.print(",");
  Serial.print(trimmer1);
  Serial.print(",");
  Serial.println(error);

// Serial.print(0); // To freeze the lower limit
// Serial.print(" ");
// Serial.print(1000); // To freeze the upper limit
// Serial.print(" ");
// Serial.println(error); 
   
    

}

ISR(TIMER1_COMPA_vect) //Interrupt at frequency of 1 Hz
{
  int16_t correctingValue;
  cli();
  hall1=    analogRead(hall);
  trimmer1= analogRead(trimmer);
  
  error = trimmer1 - hall1;
  error = error + 635;// for readability on serial plotter

  correctingValue = error - 635;
  correctingValue = error*40 ;
  if(correctingValue< 0)
  {
    correctingValue= 0;
  }
  setPWM(pwmOut, correctingValue);

  // if(hall1 < trimmer1) //Comparator behaviour but it works
  // {
  //  setPWM(pwmOut, 255);
 
  // }
  // else
  // {
  //   setPWM(pwmOut, 0);
  // }
 
  sei();
}



// There is a narrow spike in pwm out even if D=0 this is due to hw-limitations.
// The following function mitigates this problem by switching the pwm Pin do digital 0 or 0 or 1 for 255 pwm value
void setPWM(int pwmPin, int pwmVal)
{
  pinMode(pwmPin, OUTPUT);
  if (pwmVal == 0)
  {
    digitalWrite(pwmPin, LOW);
  }
  else if (pwmVal >= 90)
  {
    digitalWrite(pwmPin, HIGH);
  }
  else
  {
    sbi(TCCR2A, COM2B1);
    OCR2B = pwmVal;
  }
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
  OCR2B = 0; //PWM: 255=100%
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

