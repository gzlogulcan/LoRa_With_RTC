#include <Wire.h>
#define Led 13
#define DS3231_I2C_ADDRESS 0x68
#include <SPI.h>
#include <LoRa.h>
#include <OneWire.h> // OneWire kütüphanesini ekliyoruz.
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();
byte degree[8] =
{
0b00011,
0b00011,
0b00000,
0b00000,
0b00000,
0b00000,
0b00000,
0b00000
};

// Sıcaklık sensörünü bağladığımız dijital pini 3 olarak belirliyoruz.
int DS18S20_Pin = 3; 
// Sıcaklık Sensörü Giriş-Çıkışı
OneWire ds(DS18S20_Pin);  // 3. Dijital pinde.


//********** SAAT TANIMLANIYOR  *********

byte second, minute, hour,dayOfTheWeek, dayOfMonth, month, year; 
byte decToBcd(byte val){
  return ( (val / 10 * 16) + (val % 10) );
}
byte bcdToDec(byte val){
  return ( (val / 16 * 10) + (val % 16) );
}


void setup (){
  Serial.begin    (9600);
  Wire.begin          ();
  pinMode  (Led, OUTPUT);   
  digitalWrite(Led, LOW);

  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");
  Serial.println();

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  LoRa.setTxPower(20);
  
if (! sht31.begin(0x44)) { // Set to 0x45 for alternate i2c addr
Serial.println("Couldn't find SHT31");
while (1) delay(1);
}



}

void loop (){
   displayTime();
   if ( hour == 17 && minute == 44 ) {        // Saat 00.00sa
   void loop ();                   // Saat Ve Dakika Doğru İse Ledi Yak
    }                                           
  
   //if ( hour == 17 && minute == 45 ) {        // Saat 00.00sa
    //void loop ();                    // Saat Ve Dakika Doğru İse Ledi Söndür
    //}


    // temperature değişkenini sıcaklık değerini alma fonksiyonuna bağlıyoruz.
  float temperature = getTemp();
  // Sensörden gelen sıcaklık değerini Serial monitörde yazdırıyoruz.
  Serial.print("Toprak Sıcaklık Değeri: ");
  Serial.print(temperature);
  Serial.println("°C");
  // 1 saniye bekliyoruz. Monitörde saniyede 1 sıcaklık değeri yazmaya devam edecek.
  delay(1000);

float t = sht31.readTemperature();
float h = sht31.readHumidity();
 
if (! isnan(t)) { // check if 'is not a number'
Serial.print("Hava Sıcaklık Değeri:   ");
Serial.print(t);
Serial.println("°C"); 


} else {
Serial.println("Failed to read temperature");

}
 
if (! isnan(h)) { // check if 'is not a number'
Serial.print("Hava Nem Değeri:        "); 
Serial.print("%");
Serial.println(h);

} else {
Serial.println("Failed to read humidity");

}

delay(1000);
  int val;
  val = analogRead(0); //connect sensor to Analog 0
  Serial.print("Toprak Nem Değeri:      ");
  Serial.print("%");
  Serial.println(val/10.23); //print the value to serial port
  Serial.println("");
  
  delay(1000);
  // send packet
  LoRa.beginPacket();
  LoRa.println();
  LoRa.print("Toprak Sıcaklık Değeri: ");
  LoRa.print(temperature);
  LoRa.println(" °C");
  LoRa.print("Toprak Nem Değeri:      ");
  LoRa.print(val/10.23);
  LoRa.println(" %");
  LoRa.print("Hava Sıcaklık Değeri:   ");
  LoRa.print(t);
  LoRa.println(" °C");
  LoRa.print("Hava Nem Değeri:        ");
  LoRa.print(h);
  LoRa.println(" %");
    
  LoRa.endPacket();


  delay(1000);


  
  }  



  float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;

}






//****** DS3231 REAL TİME CLOCK  **********

  void save() {                               // Saati Ayarlamak İstiyorsanız İşlem Sonunda "save" Etmeniz Gerekmektedir...
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);                   
  Wire.write(decToBcd(second));     // set seconds
  Wire.write(decToBcd(minute));     // set minutes
  Wire.write(decToBcd(hour));       // set hours
  Wire.write(decToBcd(dayOfTheWeek));  // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month));      // set month
  Wire.write(decToBcd(year));       // set year (0 to 99)
  Wire.endTransmission();
}
//***********************

  void readDS3231time(  // DS3231'den Saat Okunuyor
  byte *second,
  byte *minute,
  byte *hour,
  byte *dayOfTheWeek,
  byte *dayOfMonth,
  byte *month,
  byte *year)
  
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  *second       = bcdToDec(Wire.read() & 0x7f);
  *minute       = bcdToDec(Wire.read());
  *hour         = bcdToDec(Wire.read() & 0x3f);
  *dayOfTheWeek = bcdToDec(Wire.read());
  *dayOfMonth   = bcdToDec(Wire.read());
  *month        = bcdToDec(Wire.read());
  *year         = bcdToDec(Wire.read());
 
    
    Serial.print("Tarih: ");
    Serial.print(*dayOfMonth  );
    Serial.print('/');
    Serial.print(*month);
    Serial.print('/');
    Serial.print(*year);
    Serial.println("\t");
    //Serial.println(*dayOfTheWeek);
    Serial.print("Saat:  ");
    Serial.print(*hour);
    Serial.print(':');
    Serial.print(*minute);
    Serial.print(':');
    Serial.print(*second);
    Serial.println();
    Serial.println("-----------------------");
    
    delay(1000);
}

//******* SAAT OKU ************

  void displayTime() {
  readDS3231time(&second, &minute, &hour, &dayOfTheWeek, &dayOfMonth, &month,
                 &year);
}
