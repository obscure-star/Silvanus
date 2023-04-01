// Code for all sensors to work

// for Power (sleep)
// To send data to LoRa


#include "LowPower.h"
// 1 hour = 3600 seconds
const uint16_t delayOneHour = 3600;
const uint16_t delay1Mins = 60;
const uint16_t delay5Mins = 300;
const uint16_t delay10Mins = 600;
const uint16_t delay15Mins = 900;
const uint16_t delay30Mins = 1800;
const uint16_t delay10Seconds = 10;
#define SLEEP_COUNT       10
#define READING_MAX_COUNT   10



// for oxygen sensor --------------------------------------------------------------------------------------
#include "DFRobot_OxygenSensor.h"

#define COLLECT_NUMBER    10             // collect number, the collection range is 1-100.
#define Oxygen_IICAddress ADDRESS_3
/*   iic slave Address, The default is ADDRESS_3.
       ADDRESS_0               0x70      // iic device address.
       ADDRESS_1               0x71
       ADDRESS_2               0x72
       ADDRESS_3               0x73
*/
DFRobot_OxygenSensor Oxygen;
// array to store oxygen data
float oxygen_avg = 0;
// --------------------------------------------------------------------------------------

// for temp sensor --------------------------------------------------------------------------------------
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;
float temperature_avg = 0;
float humidity_avg = 0;
// --------------------------------------------------------------------------------------

// for soil moisture sensor --------------------------------------------------------------------------------------
int soil_pin = A0; // AOUT pin on sensor
float slope = 2.48; // slope from linear fit
float intercept = -0.72; // intercept from linear fit
float soilmoisturevalue = 0.00;
float threshold = 0.00; // Not needed

float moisture_avg = 0;
// --------------------------------------------------------------------------------------

// for sleep algorithm --------------------------------------------------------------------------------------
int reading_count = 0;
bool ready_to_sleep = false;
// --------------------------------------------------------------------------------------
void setup() 
{
  
  Serial.begin(9600);
  
  // for oxygen sensor --------------------------------------------------------------------------------------
  while(!Oxygen.begin(Oxygen_IICAddress)) {
    Serial.println("I2c device number error !");
    delay(1000);
  }
 
  
  Serial.println("I2c connect success !");
  // --------------------------------------------------------------------------------------

  // for temp sensor --------------------------------------------------------------------------------------
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }

  Serial.println("AHT10 or AHT20 found");
  // --------------------------------------------------------------------------------------

  // for soil moisture sensor --------------------------------------------------------------------------------------
  analogReference(EXTERNAL); // set the analog reference to 3.3V
  
  // --------------------------------------------------------------------------------------
}

void loop()
{
  bool oxygen_threshold = false;
  bool temp_threshold = false;
  bool humidity_threshold = false;
  bool moisture_threshold = false;
  int alarm = 0;
  
  // for sleep algorithm - first half -------------------------------------------------------------------------------
  
  if (reading_count == SLEEP_COUNT)
  {
    ready_to_sleep = true;
  }
  Serial.println("");
  Serial.print("reading_count: "); Serial.println(reading_count);
  Serial.println("");
  // ----------------------------------------------------------------------------------------------------------------
  if (ready_to_sleep == false)
  {
    
    // for oxygen sensor --------------------------------------------------------------------------------------
    
    float oxygenData = Oxygen.getOxygenData(COLLECT_NUMBER);
    Serial.print("Oxygen concentration is ");
    Serial.print(oxygenData);
    Serial.println(" %vol");
    delay(1000);

    // store data in oxygen array
    oxygen_avg += oxygenData;
    // --------------------------------------------------------------------------------------
  
    // for temp sensor --------------------------------------------------------------------------------------
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
    //Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
    Serial.print("Temperature: "); Serial.print(temp.temperature * 1.8 + 32); Serial.println(" degrees F");
    Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
  
    delay(500);

    // store data in oxygen array
    temperature_avg += temp.temperature * 1.8 + 32;
    humidity_avg += humidity.relative_humidity;
    
    // --------------------------------------------------------------------------------------
    
    // for soil moisture sensor --------------------------------------------------------------------------------------
    float voltage,vol_water_cont; // preallocate to approx. voltage and theta_v
    Serial.print("Voltage: ");
    voltage = (float(analogRead(soil_pin))/1023.0)*3.3;
    Serial.println(voltage); // read sensor
    Serial.print("Volumetric Water Content: ");
    vol_water_cont = ((1.0/voltage)*slope)+intercept; // calc of theta_v (vol. water content)
    Serial.print(vol_water_cont);
    Serial.println(" cm^3/cm^3"); // cm^3/cm^3
    delay(1000); // slight delay between readings

    moisture_avg += vol_water_cont;
    // --------------------------------------------------------------------------------------
  
    // Final alarm algorithm
    if (reading_count == READING_MAX_COUNT-1){
      /* Get average of the arrays*/
      temperature_avg = temperature_avg/(READING_MAX_COUNT-1);
      humidity_avg = humidity_avg/(READING_MAX_COUNT-1);
      moisture_avg = moisture_avg/(READING_MAX_COUNT-1);
      oxygen_avg = oxygen_avg/(READING_MAX_COUNT-1);
      
      Serial.print("temperature average: "); Serial.println(temperature_avg);
      Serial.print("humidity average: "); Serial.println(humidity_avg);
      Serial.print("moisture average: "); Serial.println(moisture_avg);
      Serial.print("Oxygen average: "); Serial.println(oxygen_avg);
      
      /* Algorithm for alarm temp */
      // if temperature > 16 temperature is a threat
      if (temperature_avg >= 75.20)
      {
        Serial.println("High Temperature: Temperature >= 68.00 degrees F ");
        temp_threshold = true;
      }
      if (humidity_avg <= 36.00)
      {
        Serial.println("Low Humidity: Humidity <= 36.00% rH");
        humidity_threshold = true;
      }
      
      // --------------------------------------------------------------------------------------
      /* Algorithm for Alarm soil moisture*/
        if (moisture_avg >= 0.60) // wet (based off soil sample)
        {
            Serial.println("Soil Condition is: Wet - Volumetric water content >= 0.60 cm^3");
        }
        else if (moisture_avg <= 0.35) // dry (based off soil sample)
        {
            Serial.println("Soil Condition is: Dry - Volumetric water content <= 0.35 cm^3");
            moisture_threshold = true;
        }
        else 
        {
            Serial.println("Soil Condition is: Moderate - 0.35 cm^3 < Volumetric water content < 0.60 cm^3");
        }
       delay(1000);
      // --------------------------------------------------------------------------------------
      /* Algorithm for alarm oxygen sensor*/
      // if oxygenData > 25 oxygen level is a threat (25 is the limit for the device)
      if (oxygen_avg >= 20.00)
      {
        Serial.println("High Oxygen content: Oxygen concentration >= 20.00 %vol ");
        oxygen_threshold = true;
        
      }
      // for sending an alert -----------------------------------------------------------------
       if (oxygen_threshold && temp_threshold && humidity_threshold && moisture_threshold)
       {
        alarm = 1;
        Serial.println("SEND ALARM!!!!");
        delay(1000);
       }
      // --------------------------------------------------------------------------------------
      delay(3000);
      Serial.print(".0"); Serial.println(temperature_avg);
      delay(3000);
      Serial.print(".1");Serial.println(humidity_avg);
      delay(3000);
      Serial.print(".2");Serial.println(moisture_avg);
      delay(3000);
      Serial.print(".3");Serial.println(oxygen_avg);
      delay(3000);
      Serial.print(".4");Serial.println(alarm);
      delay(3000);
      
      
      temperature_avg = 0.0;
      humidity_avg = 0.0;
      moisture_avg = 0.0;
      oxygen_avg = 0.0;
    }
  }
  
  // for sleep algorithm - second half -----------------------------------------------------
  reading_count += 1;
  
  
  if (ready_to_sleep == true)
  {
    reading_count = 0; // reset sleep counter
    
    ready_to_sleep = false;
    Serial.print("Going to sleep for: ");
    Serial.print(delay1Mins);
    Serial.print(" seconds");
    delay(10000);
    longSleep( delay1Mins );
    
  }
  // --------------------------------------------------------------------------------------
  
  
}

// Power -----------------------------------------------------
// ==================================================================
// Power down the system for the desired number of seconds. Note that
// the time interval is approximate because the watchdog oscillator
// frequency is not meant to be accurate.
// ==================================================================
void longSleep( uint16_t sleepInSeconds )
{
  if ( sleepInSeconds & 0x01 ) LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  if ( sleepInSeconds & 0x02 ) LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
  if ( sleepInSeconds & 0x04 ) LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);

  while ( sleepInSeconds & 0xFFFFFFF8 ) {
    sleepInSeconds = sleepInSeconds - 8;
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}

// ------------------------------------------------------------

  
