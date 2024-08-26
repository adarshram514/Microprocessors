#include <Keypad.h>
#include <Servo.h>
  


//#define Keymap

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

  byte rowPins[ROWS] = {22, 23, 24, 25 };
  byte colPins[COLS] = {26, 27, 28, 29}; 

 Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS );
  
Servo first_servo;
void setup() {
  // put your setup code here, to run once:
  
Serial.begin(9600);
first_servo.attach(49);
//pinMode(first_servo, OUTPUT);

#ifdef EE3420_MEGA_AUX_H
  ee3420_mega_aux_setup();
#endif

#ifdef EE3420_MEGA_ADC_H
  ee3420_mega_adc_setup();
#endif
}
int limit = 1300;
void loop() {
  // put your main code here, to run repeatedly:

  char key = keypad.getKey();
Serial.print(key);
  

    if(key == '1') { //increase the pulse width by 1 
      limit = limit +1;
    first_servo.writeMicroseconds(limit);
    Serial.println("  1301 ");
    
    
    }
    if(key == '2') { //increase the pulse width by 10
      limit = limit +10;
      first_servo.writeMicroseconds(limit);
      Serial.println(" 1310");
    }
    if(key == '3') { //increase the pulse width by 100
      limit = limit +100;
      first_servo.writeMicroseconds(limit);
      Serial.println(" 1400 " );
    }
    if(key == '4') { //decrease the pulse width by 1
      limit = limit - 1;
      first_servo.writeMicroseconds(limit);
      Serial.println(" 1299" );
    }
    if(key == '5') { //decrease the pulse width by 10
      limit = limit -10;
      first_servo.writeMicroseconds(limit);
      Serial.println(" 1290" );
    }
    if(key == '6') { //decrease the pulse width by 100
      limit = limit -100;
      first_servo.writeMicroseconds(limit);
      Serial.println(" 1200 ");
      
      }
    if(key == '7') { //set the pulse width to 1000
     limit = 1000;
     first_servo.writeMicroseconds(limit);
     Serial.println( " 1000");
    }
    if(key == '8') { //set the pulse width to 1500
     limit = 1500;
     first_servo.writeMicroseconds(limit);
     Serial.println(" 1500");
    }
    if(key == '9') { //set the pulse width to 2000
      limit = 2000;
     first_servo.writeMicroseconds(limit);
     Serial.println( " 2000" );
    }
    if(key == '*'){
      Serial.println(" terminating program" );
      exit(0);
    }


}
