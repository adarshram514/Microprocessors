#include <SD.h>
#include <SPI.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static const unsigned char PROGMEM logo_bmp[] =
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};


DHT dht(26, DHT11); //This is for the temp/humidity sensor
const int CS_PIN = 5; //This is for the SD card
const int SENSOR_PIN = 27; // This if for the light sensor
const int SOUND_PIN = 14;
 
File myFile;
const char* fileName = "/test.txt"; // File name
 
void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);

  Serial.begin(115200);
  while(!Serial){}; // Wait for serial port to connect
 
  dht.begin();
  delay(1000);
  initializeCard();
 
  if(SD.exists(fileName)){
    Serial.println("\nFile exists. Will append to it.\n");
  } else {
    writeHeader(); // Only write header if file does not exist
  }
}
 
void initializeCard() {
  Serial.print("Beginning initialization of SD card: ");
   while(true) {
    if (SD.begin(CS_PIN)) {
      Serial.println("Initialization done.");
      break; // Exit the loop once the SD card initializes successfully
    } else {
      Serial.println("Initialization failed, retrying...");
      delay(1000); // Wait for a bit before retrying
    }
  }
}
 
void writeHeader() {
  myFile = SD.open(fileName, FILE_WRITE);
  if(myFile) {
    Serial.println("Writing header to " + String(fileName) + ": ");
    myFile.println("Light  Sound  Temp  Humidity");
    myFile.close();
  } else {
    Serial.println("Error opening " + String(fileName));
  }
}
 
void writeData() {
  int light = analogRead(SENSOR_PIN);
  float temp = dht.readTemperature(true);
  float humidity = dht.readHumidity();
  int sound = analogRead(SOUND_PIN);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.println(temp, 1); // Print temperature with one decimal place
  display.print("Humidity: ");
  display.println(humidity, 1); // Print humidity with one decimal place
  display.display();
 
  myFile = SD.open(fileName, FILE_WRITE);
  if(myFile) {
    myFile.seek(myFile.size());
    myFile.println(String(light) + "    " + String(sound) + "    " + temp + "    " + humidity); // Simplify to one line
    myFile.close();
    Serial.println(String(light) + "    " + String(sound) + "    " + temp + "    " + humidity);
  } else {
    Serial.println("Error opening " + String(fileName));
  }
}
 
void loop() {
  writeData();
  delay(2000);
}