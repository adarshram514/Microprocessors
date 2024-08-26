int Vrx = A1;
int Vry = A0;
const int sw = 2;


void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode(Vrx,OUTPUT);
pinMode(Vry,OUTPUT);
pinMode(sw,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  delay(3000);
int y= analogRead(A1);
if(A1>=1){
  Serial.print("Y-axis: ");
Serial.print(y);
  Serial.print(" ");
}
else if(A1<1){
Serial.print("Y-axis: ");
Serial.print(y);
Serial.print(" ");
}
}
