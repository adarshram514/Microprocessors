



#include <Buzzer.h>
int buzzer = 7;
int led1 = 13;
int button = 3;


void setup() {
  // put your setup code here, to run once:
  pinMode(led1,OUTPUT);
pinMode(button,INPUT);
pinMode(buzzer, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  
if(button == HIGH){
  digitalWrite(led1,HIGH);
tone(buzzer,1000);
}
else 
digitalWrite(led1,LOW);
noTone(buzzer);

tone(buzzer,1000);
}
