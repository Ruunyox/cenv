#include <DHT.h>
#include <DHT_U.h>
#include <SoftwareSerial.h>

#define SNSR 2
#define DHTTYPE DHT22

DHT dht(SNSR,DHTTYPE);
double temp;
double rh;
void setup()  
{
  Serial.begin(9600);
}

void loop() 
{
  delay(2000);
  temp = dht.readTemperature();
  rh   = dht.readHumidity();
  if(Serial.available()>0){
    Serial.print("RH = ");
    Serial.print(rh);
    Serial.print("\t");
    Serial.print("Temp = ");
    Serial.print(temp);
    Serial.print("\n");
    Serial.read();
  }
}

