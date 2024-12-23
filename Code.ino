#include <DHT.h>

#define DHTPIN 10    
#define DHTTYPE DHT11 

const int Speed1 = 7;
const int Speed2 = 6;
const int Speed3 = 5;

const int pirPin1 = 12; 
const int pirPin2 = 11; 

const int FANONOFF = 8; 

const unsigned long PIRDuration = 300000; // 5 minutes in milliseconds
const unsigned long TEMPDuration = 60000; // 1 minutes in milliseconds

unsigned long previousReadPIR = 0; 
unsigned long previousReadTEMP = 0;

DHT dht(DHTPIN, DHTTYPE);

float temperatureSum = 0.0;
int temperatureCount = 0;
float previousAverageTemp = 0.0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  pinMode(pirPin1, INPUT);
  pinMode(pirPin2, INPUT);
  
  pinMode(FANONOFF, OUTPUT);
  pinMode(Speed1, OUTPUT);
  pinMode(Speed2, OUTPUT);
  pinMode(Speed3, OUTPUT);

  digitalWrite(FANONOFF, LOW);
  setFanSpeed(1);
}

void loop() {
  int pirState1 = digitalRead(pirPin1); 

  int pirState2 = digitalRead(pirPin2);

  unsigned long currentMillis = millis();
  
  // Read temperature and accumulate for averaging
  float temperature = dht.readTemperature();
  if (!isnan(temperature)) {
    temperatureSum += temperature;
    temperatureCount++;
  }

  // PIR sensor logic
  if (pirState1 == HIGH || pirState2 == HIGH) { 
    
    Serial.println("Motion detected!");
    if(pirState1==HIGH){
    Serial.println("PIR right detected!");
    }
    if(pirState2==HIGH){
    Serial.println("PIR left  detected!");
    }
    previousReadPIR = currentMillis;
    delay(1000);
  } else {
    if (currentMillis - previousReadPIR < PIRDuration) {
      digitalWrite(FANONOFF, LOW);
      Serial.println("KEEP ON!");
    } else {
      digitalWrite(FANONOFF, HIGH);
      Serial.println("FAN OFF!");
      setFanSpeed(0);
      // Avoid infinite loop, instead reset the previous read time
      while(true){}
      previousReadPIR = currentMillis - PIRDuration;
    }
  }

  // Temperature adjustment logic
  if (currentMillis - previousReadTEMP >= TEMPDuration) {
    previousReadTEMP = currentMillis;
    if (temperatureCount > 0) {
      float averageTemperature = temperatureSum / temperatureCount;
      Serial.print("Average Temperature: ");
      Serial.print(averageTemperature);
      Serial.println(" °C");
      previousAverageTemp = averageTemperature;
      adjustFanSpeed(previousAverageTemp);
      // Reset temperature accumulation
      temperatureSum = 0.0;
      temperatureCount = 0;
    }
  }
}

void setFanSpeed(int speed) {
  digitalWrite(Speed1, speed == 1 ? HIGH : LOW);
  digitalWrite(Speed2, speed == 2 ? HIGH : LOW);
  digitalWrite(Speed3, speed == 3 ? HIGH : LOW);
}

void adjustFanSpeed(float temperature) {
  if (temperature < 25.0) {
    setFanSpeed(1);
  } else if (temperature < 30.0) {
    setFanSpeed(2);
  } else {
    setFanSpeed(3);
  }
}
