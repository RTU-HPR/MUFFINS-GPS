#include <Arduino.h>
#include <MUFFINS_GPS.h>

const int SENSOR_POWER_ENABLE_PIN = 17;
const int WIRE0_SCL = 1;
const int WIRE0_SDA = 0;

GPS gps;

GPS::Config config = {
  .read_interval = 25,
  .measurement_rate = GPS::MEASUREMENT_RATE_25ms,
  .navigation_rate = GPS::NAVIGATION_RATE_8Hz,
  .dynamic_model = DYN_MODEL_AIRBORNE2g,
  .wire = &Wire,
  .i2c_address = 0x42,
};

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    delay(1000);
  }

  if (Wire.setSCL(WIRE0_SCL) && Wire.setSDA(WIRE0_SDA))
  {
    Wire.begin();
  }

  pinMode(SENSOR_POWER_ENABLE_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_ENABLE_PIN, HIGH);

  if (!gps.begin(config))
  {
    Serial.println("Failed to initialize GPS");
    while (1)
    {
      delay(1000);
    }
  }
}

void loop()
{
  if (gps.read())
  {
    // Print all data
    Serial.println("--- Position ---");
    Serial.print("Latitude: ");
    Serial.println(gps.data.lat, 8);
    Serial.print("Longitude: ");
    Serial.println(gps.data.lng, 8);
    Serial.print("Altitude: ");
    Serial.print(gps.data.altitude);
    Serial.println(" meters");
    Serial.print("Ground Speed: ");
    Serial.print(gps.data.ground_speed);
    Serial.println(" m/s");
    Serial.print("Heading: ");
    Serial.print(gps.data.heading);
    Serial.println(" degrees");
    Serial.print("Satellites: ");
    Serial.println(gps.data.satellites);
    Serial.println();
    Serial.println("--- Time ---");
    Serial.print("Epoch Microseconds: ");
    Serial.println(gps.data.epoch_microseconds);
    Serial.print("Datetime (ISO 8601): ");
    Serial.print(gps.data.year);
    Serial.print("-");
    Serial.print(gps.data.month);
    Serial.print("-");
    Serial.print(gps.data.day);
    Serial.print(" ");
    Serial.print(gps.data.hour);
    Serial.print(":");
    Serial.print(gps.data.minute);
    Serial.print(":");
    Serial.print(gps.data.second);
    Serial.print(".");
    Serial.println(gps.data.millisecond);
    Serial.println();
    Serial.println("--- Accuracy ---");
    Serial.print("Fix Type: ");
    if (gps.data.fix_type == 0)
    {
      Serial.println("No fix");
    }
    else if (gps.data.fix_type == 2)
    {
      Serial.println("2D fix");
    }
    else if (gps.data.fix_type == 3)
    {
      Serial.println("3D fix");
    }
    else if (gps.data.fix_type == 4)
    {
      Serial.println("GNSS fix");
    }
    else if (gps.data.fix_type == 5)
    {
      Serial.println("Time only fix");
    }
    Serial.print("PDOP: ");
    Serial.println(gps.data.pdop);
    Serial.println();
  }
}