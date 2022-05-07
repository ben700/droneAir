#include <Arduino.h>
#include "droneConfig.h"

void mhz19bPrintValues()
{
    Serial.print("\n----- Time from start: ");
    Serial.print(millis() / 1000);
    Serial.println(" s");

    int ppm_uart = co2.readCO2UART();
    Serial.print("PPMuart: ");

    if (ppm_uart > 0)
    {
        Serial.print(ppm_uart);
    }
    else
    {
        Serial.print("n/a");
    }

    int ppm_pwm = co2.readCO2PWM();
    Serial.print(", PPMpwm: ");
    Serial.print(ppm_pwm);

    int temperature = co2.getLastTemperature();
    Serial.print(", Temperature: ");

    if (temperature > 0)
    {
        Serial.println(temperature);
    }
    else
    {
        Serial.println("n/a");
    }

    Serial.println("\n------------------------------");
}

void mhz19bConfig()
{
    pinMode(CO2_IN, INPUT);
    delay(100);
    Serial.println("MHZ 19B");

    // enable debug to get addition information
    // co2.setDebug(true);

    if (co2.isPreHeating())
    {
        Serial.print("Preheating");
        while (co2.isPreHeating())
        {
            Serial.print(".");
            delay(5000);
        }
        Serial.println();
    }
}

void tsl2591AdvancedPrintValues ()
{
  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.print("[ "); Serial.print(millis()); Serial.print(" ms ] ");
  Serial.print("IR: "); Serial.print(ir);  Serial.print("  ");
  Serial.print("Full: "); Serial.print(full); Serial.print("  ");
  Serial.print("Visible: "); Serial.print(full - ir); Serial.print("  ");
  Serial.print("Lux: "); Serial.println(tsl.calculateLux(full, ir));
}

void printTSL2591SensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution, 4); Serial.println(" lux");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void tsl2591PrintValues()
{
    sensor_t sensor;
    tsl.getSensor(&sensor);
    Serial.println(F("------------------------------------"));
    Serial.print(F("Sensor:       "));
    Serial.println(sensor.name);
    Serial.print(F("Driver Ver:   "));
    Serial.println(sensor.version);
    Serial.print(F("Unique ID:    "));
    Serial.println(sensor.sensor_id);
    Serial.print(F("Max Value:    "));
    Serial.print(sensor.max_value);
    Serial.println(F(" lux"));
    Serial.print(F("Min Value:    "));
    Serial.print(sensor.min_value);
    Serial.println(F(" lux"));
    Serial.print(F("Resolution:   "));
    Serial.print(sensor.resolution, 4);
    Serial.println(F(" lux"));
    Serial.println(F("------------------------------------"));
    Serial.println(F(""));
    delay(500);
}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2591
*/
/**************************************************************************/
void tsl2591Config()
{
    // You can change the gain on the fly, to adapt to brighter/dimmer light situations
    // tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
    tsl.setGain(TSL2591_GAIN_MED); // 25x gain
    // tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain

    // Changing the integration time gives you a longer time over which to sense light
    // longer timelines are slower, but are good in very low light situtations!
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
    tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

    /* Display the gain and integration time for reference sake */
    Serial.println(F("------------------------------------"));
    Serial.print(F("Gain:         "));
    tsl2591Gain_t gain = tsl.getGain();
    switch (gain)
    {
    case TSL2591_GAIN_LOW:
        Serial.println(F("1x (Low)"));
        break;
    case TSL2591_GAIN_MED:
        Serial.println(F("25x (Medium)"));
        break;
    case TSL2591_GAIN_HIGH:
        Serial.println(F("428x (High)"));
        break;
    case TSL2591_GAIN_MAX:
        Serial.println(F("9876x (Max)"));
        break;
    }
    Serial.print(F("Timing:       "));
    Serial.print((tsl.getTiming() + 1) * 100, DEC);
    Serial.println(F(" ms"));
    Serial.println(F("------------------------------------"));
    Serial.println(F(""));
}

void bme280Config()
{
  
    Serial.println("-- Weather Station Scenario --");
    Serial.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
    Serial.println("filter off");
    bme280.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF);

    // suggested rate is 1/60Hz (1m)
    //delayTime = 60000; // in milliseconds
}

void bme280PrintValues()
{
    Serial.print("Temperature = ");
    Serial.print(bme280.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme280.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme280.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme280.readHumidity());
    Serial.println(" %");

    Serial.println();
}