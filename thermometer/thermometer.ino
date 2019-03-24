/*

  Thermometer based on 1n4148 silicon diode used as temperature sensor.The thermometer is based on the diode characteristic that the increase of the temperature its forward voltage (VF) is lowered by 2,2mV / ° C.
Fixing the value of Vf = VF0 at ambient temperature t0, the temperature value
t is calculated with the following formula:

t= t0 - [vf(t)- vf0]* K

with K = 1 / 2,2mV

vf0 = 573.44

The value of Vf (t) = dtemp -vf0 is obtained by averaging values of 1024 by acquiring as many vf values

The result of t is shown on the serial monitor
*/
#include <OneWire.h>
#include <DallasTemperature.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 5     // Digital pin connected to the DHT sensor 
#define DHTTYPE DHT11
DHT_Unified dht_11(DHTPIN, DHTTYPE);
uint32_t delayMS;

#define ONE_WIRE_BUS 13
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

float _avg;
int i;

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT_PULLUP);            // set the  pin IN with npull up to bias the diode
  DS18B20.begin();

  dht_11.begin();
  sensor_t dhtsensor;
  dht_11.temperature().getSensor(&dhtsensor);
  delayMS = dhtsensor.min_delay / 1000;
}

void loop() {
  for (_avg = 0,i = 0; i < 256; i++,delay(5)) {
    _avg += analogRead(A0) * (5000.0 / 1023.0) - 573.44;
  }
  float t = 20.3 - (int(_avg) >> 8) * 0.45454545;    // Volt to Celcius (1/2.2 = 0.45454545)
  Serial.print("1N4148 = " );
  Serial.print(t);
  Serial.print(F("°C, "));

  DS18B20.requestTemperatures(); 
  Serial.print("DS18B20  = " );
  Serial.print(DS18B20.getTempCByIndex(0));
  Serial.print(F("°C, "));
  
  delay(delayMS);
  sensors_event_t event;
  dht_11.temperature().getEvent(&event);
  if (!isnan(event.temperature)) {
    Serial.print(F("DHT-11 = : "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }

}
