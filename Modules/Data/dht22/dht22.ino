#include <dht.h>
dht DHT;
//Constants
#define DHT22_PIN 8     

//Variables
float hum;  
float temp; 

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    int chk = DHT.read22(DHT22_PIN);
    p
    hum = DHT.humidity;
    temp= DHT.temperature;
    
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    delay(2000); 
}