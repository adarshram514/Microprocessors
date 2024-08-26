#include <SD.h>
#include <SPI.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <ESP32Firebase.h>
 
#define WIFI_SSID  "Eddie"
#define WIFI_PASSWORD "Mopp3tt!"
#define API_KEY "AIzaSyCs7OKUVsWg3hRFSYSiHQGc5xeTp1RyTv8"
#define DATABASE_URL "https://urban-hotspots-1-default-rtdb.firebaseio.com/"
 
Firebase firebase(DATABASE_URL);
 
unsigned long sendDataPrevMillis = 0;
 
 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
 
#define OLED_RESET 4
 
#define RXD2 16
#define TXD2 17
 
HardwareSerial neogps(1);
TinyGPSPlus gps;
 
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

unsigned long sendDataPrevMillis = 0;

// Constants for sound sensor sensitivity and reference voltage
const float SOUND_SENSITIVITY = 0.1; // Example sensitivity, adjust based on sensor specifications
const float REFERENCE_VOLTAGE = 5.0; // Example reference voltage, adjust based on sensor specifications

// Function to convert sound intensity (analog reading) to decibels
float convertToDecibels(float soundVoltage) {
  // Convert analog voltage to sound pressure level (SPL) using sensor characteristics
  // Example conversion factor: 3.0 mV/dB (adjust based on sensor specifications)
  float voltageInMillivolts = soundVoltage * (5000.0 / 1023.0); // Convert to millivolts
  float soundPressureLevel = voltageInMillivolts / SOUND_SENSITIVITY; // Convert to sound pressure level
  // Convert SPL to decibels (dB)
  return 20 * log10(soundPressureLevel / REFERENCE_VOLTAGE);
}

// Function to convert light frequency (Hz) to Lux
float convertToLux(float lightHz) {
  // Calibration curve or formula to convert frequency to Lux
  // This needs to be determined based on sensor characteristics
  // For LM393 module, you need to establish a calibration curve
  // relating frequency (Hz) to Lux
  // For simplicity, let's assume a linear relationship for demonstration
  float slope = 10.0; // Example slope, adjust based on calibration
  float intercept = 50.0; // Example intercept, adjust based on calibration
  return slope * lightHz + intercept;
}
 
void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
 
  Serial.begin(115200);
  while(!Serial){}; // Wait for serial port to connect
 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi: ");
  while(WiFi.status() != WL_CONNECTED){
    Serial.println("."); delay(300);
  }
  Serial.println("WiFi Connected");
 
 
  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);
 
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
  if (myFile) {
    myFile.println("Lat,Long, Time, Light, Temp, Humidity, Sound");
    myFile.close();
  } else {
    Serial.println("Error opening " + String(fileName));
  }
}
 
void loop() {
  // Check if enough time has passed to send data again
  if (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0) {
    sendDataPrevMillis = millis(); // Update the last sent time
 
    int light = analogRead(SENSOR_PIN); // Read digital signal from LM393
    float lightHz = light / 1000.0; // Convert to frequency in Hz (for Lux conversion)
    float temperature = dht.readTemperature(true);
    float humidity = dht.readHumidity();
    float soundVoltage = analogRead(SOUND_PIN) * (5.0 / 1023.0); // Convert analog to voltage (assuming 5V reference)
    float sounddB = convertToDecibels(soundVoltage); // Convert sound intensity to decibels
 
    boolean newData = false;
    for (unsigned long start = millis(); millis() - start < 1000;) {
      while (neogps.available()) {
        if (gps.encode(neogps.read())) {
          newData = true;
        }
      }
    }

    float latitude = gps.location.isValid() ? gps.location.lat() : 0;
    float longitude = gps.location.isValid() ? gps.location.lng() : 0;

    // Get current time in Central Time (CT)
    unsigned long currentTime = now(); // Get current time since start in seconds
    int ctHours = hour(currentTime - 6 * 3600); // Adjust UTC to Central Time (UTC-6)
    int ctMinutes = minute(currentTime - 6 * 3600);
    int ctSeconds = second(currentTime - 6 * 3600);
 
    // Begin by clearing the display
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0,0);     // Start at top-left corner
 
  // Print time since start
  display.print("Time: ");
  display.println(millis() / 1000);
 
  // Print temperature
  display.print("Temp: ");
  display.print(temperature, 1);
  display.println(" F");
 
  // Print humidity
  display.print("Humidity: ");
  display.print(humidity, 1);
  display.println("%");
 
  // Display the content
  display.display();
 
    float latitude = gps.location.isValid() ? gps.location.lat() : 0;
    float longitude = gps.location.isValid() ? gps.location.lng() : 0;
 
   // Firebase updates for temperature, humidity, latitude, and longitude
    firebase.setFloat("Environment/Temperature", temperature);
    firebase.setFloat("Environment/Humidity", humidity);
    firebase.setFloat("GPS/Latitude", latitude);
    firebase.setFloat("GPS/Longitude", longitude);
   
    // Continue with SD card logging and display updates here...
    myFile = SD.open(fileName, FILE_WRITE);
    if (myFile && newData) {
      myFile.seek(myFile.size());
      myFile.print(millis());
      if (gps.location.isValid()) {
        myFile.print(",");
        myFile.print(gps.location.lat(), 6);
        myFile.print(",");
        myFile.println(gps.location.lng(), 6);
      } else {
        myFile.println(",No GPS Data");
      }
      myFile.print(",");
      myFile.print(lightLux);
      Serial.println(lightLux);
      myFile.print(",");
      myFile.print(temperature, 2);
      myFile.print(",");
      myFile.print(humidity, 2);
      myFile.print(",");
      myFile.print(sounddB);
      Serial.println(sounddB);
      myFile.close();
    } else if (!myFile) {
      Serial.println("Error opening " + String(fileName));
    }
  }
}