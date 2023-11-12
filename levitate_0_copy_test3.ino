//>
//
//$ sudo chmod a+rw /dev/ttyUSB0
//
#define hall A6

#define led1 A4
#define led2 A3






int time0=0;
int seconds = 0;
int minutes=0;
int hours=0;
int tick=0;

void setup() {



pinMode(hall, INPUT);

pinMode(led1, OUTPUT);
pinMode(led2, OUTPUT);


//Timer 2 init (sensor feed)
timer2_init();
//Timer1 init( timebase)
timer1_init();



Serial.begin(9600);
// Relais Test

delay(1000); 
// Relais Test

}

int16_t counter1;
uint16_t hall1;

void loop() {
  // put your main code here, to run repeatedly:

delay(100);            // waits for a second
counter1 +=1;
//Serial.println(counter1);
hall1=analogRead(A6);
Serial.println(hall1);

 

   
    

}

void timer2_init()
{
  cli();
  TCCR2A = ((1 << WGM21) | (1 << COM2A0));// CTC normal port operation
  //TCCR2B = (1 << WGM22); this bit switches toggle on compare match on 
  TCCR2B = (4 << CS20); // prescaler
  TIMSK2 = 0;
  OCR2A = 0;
  sei();
}

void timer1_init()
{
  cli();
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  OCR1A = 15624;// entspricht 1 Hz  bei 1024 Presclr
  TCCR1B |= (1<<CS10) | (1<<CS12);// presclr = 1024

  
  TCCR1B |= (1<<WGM12); //CTC
  
  
  //initialize counter value to 0;
  TCNT1  = 0;
  
 
  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  sei();
}

ISR(TIMER1_COMPA_vect) //Interrupt at frequency of 1 Hz
{
 //Clock
 
  seconds +=1;

  if(seconds > 59)
  {
   minutes+=1;
   seconds=0;
  }
  if(minutes > 59)
  {
   hours+=1;
   minutes=0;
  }
  if(hours > 23)
  {
   hours=0;
  }
}
