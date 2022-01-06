#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>

#include <SoftwareSerial.h>

#include <TinyGPS.h>
TinyGPS gps;  //Creates a new instance of the TinyGPS object// Includes the PulseSensorPlayground Library.


#include <DHT.h>
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

//  Variables
const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int LED13 = 13;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.
                               // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                               // Otherwise leave the default "550" value.
PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"

#define pirPin 2

int calibrationTime = 30;
long unsigned int lowIn;
long unsigned int pause = 5000;
boolean lockLow = true;
boolean takeLowTime;
int PIRValue = 0;

int IRpin = A1;

void setup() {

  // put your main code here, to run repeatedly:
  Serial.begin(9600);  
  pinMode(pirPin, INPUT);// For Serial Monitor
  dht.begin();

  // Configure the PulseSensor object, by assigning our variables to it. 
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);   

  // Double-check the "pulseSensor" object was created and "began" seeing a signal. 
   if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }

  //GPS
  
  Serial.print("Simple TinyGPS library v. "); Serial.println(TinyGPS::library_version());
  Serial.println("Testing GPS");
  Serial.println();
}

void Temp()
{

  delay(2000);
  Serial.println("=================================");
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);

}

void Heart(){

    // Pulse

  int myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int".
                                               // "myBPM" hold this BPM value now. 


    if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened". 
 Serial.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
 Serial.print("BPM: ");                        // Print phrase "BPM: " 
 Serial.println(myBPM);                        // Print the value inside of myBPM. 
}

  delay(20);                    // considered best practice in a simple sketch.
  
  }

void GPS()
{
      //   GPS

  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      //Serial.print(c);
      if (gps.encode(c)) 
        newData = true;  
    }
  }

    if (newData)      //If newData is true
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);   
    Serial.print("Latitude = ");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" Longitude = ");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);

  }
 
  Serial.println(failed);
 // if (chars == 0)
   // Serial.println("** No characters received from GPS: check wiring **");
   
  
}

void PIRSensor()
{
     if(digitalRead(pirPin) == HIGH)
     {       
         if(lockLow)
         {    
             PIRValue = 1;
             lockLow = false;
             Serial.println("Motion detected.");
             
             
             delay(50);
         }         
         takeLowTime = true;
       }

       if(digitalRead(pirPin) == LOW)
       {       
           
           if(takeLowTime){lowIn = millis();takeLowTime = false;}
           if(!lockLow && millis() - lowIn > pause)
           {
               PIRValue = 0; 
               lockLow = true;                        
               Serial.println("Motion ended.");
               
               delay(50);
           }
       }
}

void IR(){
  float volts = analogRead(IRpin)*0.0048828125;   
  float distance = 65*pow(volts, -1.10);
  
 Serial.print("IR: ");                     
 Serial.println(distance);
  
}
void loop() {
  Temp();
  Heart();
  GPS();
  PIRSensor();
  IR();
}
