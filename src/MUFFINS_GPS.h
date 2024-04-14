#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <MUFFINS_Component_Base.h>

class GPS : public Component_Base
{
public:
  enum Measurement_Rate
  {
    MEASUREMENT_RATE_25ms = 25,    // 25ms (Should be the default)
    MEASUREMENT_RATE_50ms = 50,    // 50ms
    MEASUREMENT_RATE_100ms = 100,  // 100ms
    MEASUREMENT_RATE_200ms = 200,  // 200ms
    MEASUREMENT_RATE_500ms = 500,  // 500ms
    MEASUREMENT_RATE_1000ms = 1000 // 1000ms
  };

  enum Navigation_Rate
  {
    NAVIGATION_RATE_1Hz = 1,   // 1Hz
    NAVIGATION_RATE_2Hz = 2,   // 2Hz
    NAVIGATION_RATE_4Hz = 4,   // 4Hz
    NAVIGATION_RATE_5Hz = 5,   // 5Hz
    NAVIGATION_RATE_8Hz = 8,   // 8Hz (Should be the default)
    NAVIGATION_RATE_10Hz = 10, // 10Hz
    NAVIGATION_RATE_20Hz = 20, // 20Hz
    NAVIGATION_RATE_25Hz = 25, // 25Hz
  };

  struct Config
  {
    int read_interval;                 // The interval in ms between reads (The lowest it should be is the measurement rate. Can increase if the I2C buss is having trouble keeping up)
    Measurement_Rate measurement_rate; // How often measurement will be taken in ms
    Navigation_Rate navigation_rate;   // How often the navigation solution will be updated
    dynModel dynamic_model;            // Dynamic model
    TwoWire *wire;                     // The I2C buss
    int i2c_address;                   // The I2C address (Default should be 0x42)
  };

  struct Data
  {
    // Position
    bool position_valid;
    double lat;
    double lng;
    float altitude;
    float ground_speed;
    int satellites;
    float heading;
    // Time
    bool time_resolved;
    bool time_valid;
    bool time_confirmed;
    uint32_t epoch_microseconds;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int millisecond;
    // Accuracy
    int fix_type; // 0: No fix, 2: 2D fix, 3: 3D fix, 4: GNSS fix, 5: Time only fix
    float pdop;   // <1: Ideal, 1-2: Excellent, 2-5: Good, 5-10: Moderate, 10-20: Fair, >20: Poor (https://en.wikipedia.org/wiki/Dilution_of_precision_(navigation)#Interpretation) 
  } data;

private:
  SFE_UBLOX_GNSS _gps;
  Config _config;

  unsigned long _last_measurement_time;

  /**
   * @brief Configure the GPS
   * @return true If the GPS was successfully configured
   */
  bool _configure();

  /**
   * @brief Read the time from the GPS
   * @return true If the time was successfully read
   */
  bool _read_time();

  /**
   * @brief Read the position from the GPS
   * @return true If the position was successfully read
   */
  bool _read_position();

public:
  /**
   * @brief Construct a new Radio object
   */
  GPS(String component_name = "GPS", void (*info_function)(String) = nullptr, void (*error_function)(String) = nullptr);

  /**
   * @brief Destroy the Radio object
   */
  ~GPS();

  /**
   * @brief Begin the GPS
   * @param config The configuration of the GPS
   * @return true If the GPS was successfully started
   */
  bool begin(const Config &config);

  /**
   * @brief Read the GPS data
   * @return true If the GPS data was successfully read
   */
  bool read();
};