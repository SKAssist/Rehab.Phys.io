#include <DFRobot_BMX160.h>
#include <DFRobot_LIS2DW12.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoBLE.h>
DFRobot_BMX160 bmx160;

BLEService ServiceUUID("25AE1441-05D3-4C5B-8281-93D4E07420CF"); // create service
// create characteristic and allow remote device to read and write

// create characteristic and allow remote device to get notifications and read the value
BLEStringCharacteristic Inputanglecharacteristic ("25AE1443-05D3-4C5B-8281-93D4E07420CF", BLEWrite, 10);

BLEStringCharacteristic Startcharacteristic ("25AE1445-05D3-4C5B-8281-93D4E07420CF", BLEWrite, 10);
BLEStringCharacteristic Outputanglecharacteristic("25AE1446-05D3-4C5B-8281-93D4E07420CF",  BLERead | BLENotify, 30);
BLEStringCharacteristic Outputmaxanglecharacteristic("25AE1447-05D3-4C5B-8281-93D4E07420CF",  BLERead | BLENotify, 30);
BLEStringCharacteristic Outputtimecharacteristic("25AE1448-05D3-4C5B-8281-93D4E07420CF",  BLERead | BLENotify, 30);
BLEStringCharacteristic Outputrepscharacteristic("25AE1449-05D3-4C5B-8281-93D4E07420CF",  BLERead | BLENotify, 30);
BLEStringCharacteristic Outputscannedexercisecharacteristic("25AE1450-05D3-4C5B-8281-93D4E07420CF",  BLERead | BLENotify, 30);
BLEStringCharacteristic Outputsummarycharacteristic("25AE1451-05D3-4C5B-8281-93D4E07420CF",  BLERead | BLENotify, 100);
BLEStringCharacteristic emgxcharacteristic("25AE1452-05D3-4C5B-8281-93D4E07420CF",  BLERead | BLENotify, 30);
BLEStringCharacteristic emgycharacteristic("25AE1453-05D3-4C5B-8281-93D4E07420CF",  BLERead | BLENotify, 30);

unsigned long previousMillis = 0;
int maxReading = 0;
int rep=0;
int i=0;
String anglein= ""; 
DFRobot_LIS2DW12_I2C acce;
#if defined(ESP32) || defined(ESP8266)
#define LIS2DW12_CS  D3
#elif defined(__AVR__) || defined(ARDUINO_SAM_ZERO)
#define LIS2DW12_CS 3
#elif (defined NRF5)
#define LIS2DW12_CS 2  //The pin on the development board with the corresponding silkscreen printed as P2
#endif
#ifdef __AVR__
#include <avr/power.h> 
#endif
#define PIN   1   
#define NUMPIXELS 1 
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);



void setup(){
  Serial.begin(115200);
  delay(100);

  if (bmx160.begin() != true){
    Serial.println("init false");
    while(1);
  }
  while(!acce.begin()){
     Serial.println("Communication failed, check the connection and I2C address setting when using I2C communication.");
     delay(1000);
  }
  Serial.print("chip id : ");
  Serial.println(acce.getID(),HEX);
 
  acce.softReset();
 
  acce.continRefresh(true);
    acce.setDataRate(DFRobot_LIS2DW12::eRate_50hz);
  
  acce.setRange(DFRobot_LIS2DW12::e2_g);
  
  acce.setFilterPath(DFRobot_LIS2DW12::eLPF);
    acce.setFilterBandwidth(DFRobot_LIS2DW12::eRateDiv_4);
      acce.setPowerMode(DFRobot_LIS2DW12::eContLowPwrLowNoise2_14bit);

if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
     while (1);
  }

BLE.setLocalName("Physio");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(ServiceUUID);

  // add the characteristics to the service

  ServiceUUID.addCharacteristic(Inputanglecharacteristic);
  ServiceUUID.addCharacteristic(Startcharacteristic);
  ServiceUUID.addCharacteristic(Outputanglecharacteristic);
  ServiceUUID.addCharacteristic(Outputmaxanglecharacteristic);
  ServiceUUID.addCharacteristic(Outputtimecharacteristic);
  ServiceUUID.addCharacteristic(Outputrepscharacteristic);
  ServiceUUID.addCharacteristic(Outputscannedexercisecharacteristic);
  ServiceUUID.addCharacteristic(Outputsummarycharacteristic);
  ServiceUUID.addCharacteristic(emgxcharacteristic);
  ServiceUUID.addCharacteristic(emgycharacteristic);
BLE.addService(ServiceUUID);

  Outputanglecharacteristic.writeValue("");
  Outputmaxanglecharacteristic.writeValue("");
  Outputtimecharacteristic.writeValue("");
  Outputrepscharacteristic.writeValue("");
  Outputscannedexercisecharacteristic.writeValue("");
  Outputsummarycharacteristic.writeValue("");
  emgxcharacteristic.writeValue("");
  emgycharacteristic.writeValue("");
  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop()
{
  BLEDevice central = BLE.central();

if (central) {
  Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());


    // while the central is still connected to peripheral:
    while (central.connected()) {
    anglein = Inputanglecharacteristic.value();
    float angleinf = anglein.toFloat();
    Serial.println("The input angle is" + anglein);
    String startvalue = Startcharacteristic.value();
if(startvalue=="0"){
          unsigned long currentMillis = millis();
  if(currentMillis-previousMillis >=1000){
    previousMillis = currentMillis;
    i++;
    Serial.print("time: ");
    Serial.println(i);

    }
  sBmx160SensorData_t  Oaccel;
  bmx160.getAllData(NULL, NULL, &Oaccel);
  
  Serial.print("Accel ");
  Serial.print("X: "); Serial.print(Oaccel.x    ); Serial.print("  ");
  Serial.print("Y: "); Serial.print(Oaccel.y   ); Serial.print("  ");
  Serial.print("Z: "); Serial.print(Oaccel.z    ); Serial.print("  ");
  Serial.println("m/s^2");
float ybmx=Oaccel.y-2.25;
  Serial.println("");
   Serial.print("x: ");
   float xlis=9.8*(acce.readAccX())/1000;
   float ylis=9.8*(acce.readAccY())/1000;
   float zlis=9.8*(acce.readAccZ())/1000;
    Serial.print(xlis);
    Serial.print(" m/s2 \ty: ");

    Serial.print(ylis);
    Serial.print(" m/s2 \tz: ");
   
    Serial.print(zlis);
    Serial.println(" m/s2");
    float anglebmx=atan2(ybmx, Oaccel.z)*57.3;
    float anglelis= atan2 (ylis, zlis)*57.3;
 
   Serial.println (anglebmx-4);
      Serial.println (anglelis+5);
      float angle= (anglebmx-4-(anglelis+5))+110;
         Serial.println (angle);
         Outputanglecharacteristic.writeValue(String(angle));
         if (angle > maxReading){
  maxReading = angle;
 }
  Serial.println(maxReading); 
  if (angle<10){
    rep++;
    delay (500);}
    Serial.println("reps=");
    int norep=rep/2;
    Serial.print(norep);
 int a1 = 0.9*angleinf;
 int a2 = 0.8*angleinf;
 int a3 = 0.6*angleinf;
 int a4 = 0.4*angleinf;
 int a5 = -150;

    Serial.println("");
  pixels.clear();
  
if(angle>a1){
  pixels.setPixelColor(0, pixels.Color(0, 255, 0));
   pixels.show();   // Send the updated pixel colors to the hardware.  
}
else if(angle>a2 and angle<a1){
   pixels.setPixelColor(0, pixels.Color(153, 255, 51));
   pixels.show();   // Send the updated pixel colors to the hardware.  
  }
else if(angle>a3 and angle<a2){
   pixels.setPixelColor(0, pixels.Color(255, 255, 0));
   pixels.show();   // Send the updated pixel colors to the hardware.  
  }
else if(angle>a4 and angle<a3){
   pixels.setPixelColor(0, pixels.Color(255, 128, 0));
   pixels.show();   // Send the updated pixel colors to the hardware.  
  }
else if(angle>a5 and angle<a4){
   pixels.setPixelColor (0, pixels.Color(255, 0, 0));
   pixels.show();  
 }
String scanex = "Kneestretch";
        Outputmaxanglecharacteristic.writeValue(String(maxReading));
            Outputtimecharacteristic.writeValue(String(i));
            Outputrepscharacteristic.writeValue(String(norep));
            Outputscannedexercisecharacteristic.writeValue(scanex);
            emgxcharacteristic.writeValue((String)i);
            emgycharacteristic.writeValue((String)angle);           
//sending angle on y just for now
}
else if(startvalue=="1"){
   float norep=rep/2;
   
   float repspeed = norep/i;
       Serial.println(repspeed);

    String everything = ((String)"   Max Angle= " + maxReading + "\n"+"   Total Time = " + i+"\n"  + "     Total Reps=" + (int)norep + "\n"+"Avg. Rep Speed="+ repspeed );
    Outputsummarycharacteristic.writeValue(everything);
}
    }
    Serial.print(F("Disconnected fromcentral: "));
    Serial.println(central.address());
}
}
  
