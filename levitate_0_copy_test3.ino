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
#define hallB A6
#define trimmer A5
#define pwmOut 3
#define freqTestOut 4
#define led   5


// #define KP  6
// #define KD  10
// #define KI  1

#define KP  4
#define KD  10
#define KI  0


int time0=0;
int seconds = 0;
int minutes=0;
int hours=0;
int tick=0;

int16_t  counter1;
int16_t  hall1;     //adc values
int16_t  hall1B;
int16_t  trimmer1;  //adc values
int16_t  hallMeasError[256];
int16_t  hallLinearized;
int16_t  error;
int16_t  errorScaled;
int16_t  errorPrevious;
int16_t  proportional;
int16_t  derivative;
int16_t  integral;
int16_t  correctingValue;
int16_t hallMeasErrorTest;



void setup() {

sbi(ADCSRA,ADPS2) ; //configure adc to highspeed and lowish precision
cbi(ADCSRA,ADPS1) ;
cbi(ADCSRA,ADPS0) ;

pinMode(hall, INPUT);
pinMode(trimmer, INPUT);
pinMode(pwmOut, OUTPUT);
pinMode(freqTestOut, OUTPUT);
pinMode(led,OUTPUT);

//Timer 2 init (pwmOut)
timer2_init();
//Timer1 init( timebase)
timer1_init();

//Function prototypes

//Funktion prototypes

//Serial.begin(9600); default
Serial.begin(115200);
  delay(500);
  digitalWrite(led,0);
// measure sensor values for each pwm setpoint
  for(uint16_t  i=0; i<256; i++)
  {
    
      setPWM(pwmOut,i);
    

    delay(10);

    // Serial.print(i);
    // Serial.print(",");
    // Serial.print(hall1);
    // Serial.print(",");
    // Serial.println(hall1B);
    
    
    Serial.print(OCR2B);
    Serial.print(",");
    Serial.print(hall1);
    Serial.print(",");
    Serial.print(hall1B);
    Serial.print(",");

    hallMeasError[i]=(hall1-hall1B);
    delay(10);
    Serial.println(hallMeasError[i]);
    
  }
  delay(1000);

  digitalWrite(led,1);
  Serial.println("substracting hallMeasError and run pwm-ramp again");

  delay(1000);

  for(uint16_t n=0; n<256; n++)
  {
    setPWM(pwmOut,n);
    delay(10);

    
    
   
    Serial.print(n);
    Serial.print(",");

    hallLinearized = ((hall1-hall1B)-hallMeasError[n]);
    
    Serial.print(hallMeasError[n]);
    Serial.print(",");
    Serial.println(hallLinearized);
  }
  setPWM(pwmOut,23);
  Serial.println("pwmvalue");
  Serial.println(OCR2B);
}



void loop() {

  
  
  
//
  // put your main code here, to run repeatedly:

delay(100);            // waits for a second

//Serial.println(counter1);


Serial.print("HallLinearized:");
Serial.print(hallLinearized);
Serial.print(",");
Serial.print("Trimmer");
Serial.print(trimmer1);
Serial.print(",");
Serial.print("error");
Serial.println(error);

  // Serial.print(hall1);
  // Serial.print(",");
  // Serial.print(hall1B);
  // Serial.print(",");
  // Serial.println(trimmer1);

// Serial.print(0); // To freeze the lower limit
// Serial.print(" ");
// Serial.print(1000); // To freeze the upper limit
// Serial.print(" ");
// Serial.println(error); 
   
  // Serial.print(hall1);
  // Serial.print(",");
  // Serial.print(proportional);
  // Serial.print(",");
  // Serial.print(integral);
  // Serial.print(",");
  // Serial.println(derivative);
   

}

ISR(TIMER1_COMPA_vect) //Interrupt at frequency of 10kHz
{
  
  cli();
  errorPrevious = error;
  hall1=    analogRead(hall);
  hall1B=    analogRead(hallB);
  trimmer1= analogRead(trimmer);
  hallLinearized = ((hall1-hall1B)-hallMeasError[OCR2B]);
  
  error = trimmer1 - hallLinearized;
  
  //errorScaled = error + 635;// for readability on serial plotter

  ///P-Part
  proportional = error ;
  //D-Part
  derivative   = error-errorPrevious;// no need to divide by timestep cause timestep is constant
  //I-Part
  integral    += error;
  
  correctingValue = (KP * proportional + KD * derivative + KI * integral)/3;
  
  if(correctingValue< 0)
  {
    correctingValue= 0;
  }



  

//  setPWM(pwmOut, correctingValue);

  if(hall1 < trimmer1) //Comparator behaviour but it works
  {
   setPWM(pwmOut, 255);
 
  }
  else
  {
    setPWM(pwmOut, 0);
  }

 // setPWM(pwmOut,0);
  
  digitalWrite(freqTestOut, !digitalRead(freqTestOut)); // toggle a pin to check if isr frequnecy
  sei();
}



// There is a narrow spike in pwm out even if D=0 this is due to hw-limitations.
// The following function mitigates this problem by switching the pwm Pin do digital 0 or 1 for 0 or 255 pwm value
void setPWM(int pwmPin, int pwmVal)
{
  pinMode(pwmPin, OUTPUT);
  if (pwmVal == 0)
  {
    digitalWrite(pwmPin, LOW);
  }
  else if (pwmVal >= 255)
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
  
  OCR1A = 1600; //should equate to 10kHz (and it does :) )
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

