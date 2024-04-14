#include "MUFFINS_GPS.h"

GPS::GPS(String component_name, void (*info_function)(String), void (*error_function)(String)) : Component_Base(component_name, info_function, error_function)
{
  return;
}

GPS::~GPS()
{
  return;
}

bool GPS::_configure()
{
  // Set the I2C port to output UBX only (turn off NMEA noise)
  if (!_gps.setI2COutput(COM_TYPE_UBX))
  {
    error("Failed setting I2C output to UBX only");
    return false;
  }

  // How often (in ms) to update the GPS
  if (!_gps.setMeasurementRate(_config.measurement_rate))
  {
    error("Failed setting the measurement rate to: " + String(_config.measurement_rate));
    return false;
  }

  // How often to get a new navigation solution
  if (!_gps.setNavigationFrequency(_config.navigation_rate))
  {
    error("Failed setting the navigation frequency to: " + String(_config.navigation_rate));
    return false;
  }

  // Set the dynamic model
  if (!_gps.setDynamicModel(_config.dynamic_model))
  {
    error("Failed setting the dynamic model to: " + String(_config.dynamic_model));
    return false;
  }

  // Tell the GNSS to "send" each solution
  if (!_gps.setAutoPVT(true))
  {
    error("Failed setting auto PVT");
    return false;
  }

  // Save the configuration
  if (!_gps.saveConfiguration())
  {
    error("Failed to save configuration");
    return false;
  }
  return true;
}

bool GPS::_read_time()
{
  // Check if time is coming from the GPS
  if (_gps.getTimeFullyResolved())
  {
    data.time_resolved = true;
  }

  // Check if time is valid
  if (_gps.getTimeValid())
  {
    data.time_valid = true;
  }

  if (_gps.getConfirmedTime())
  {
    data.time_confirmed = true;
  }

  // Get epoch time
  uint32_t new_epoch_time_microseconds = _gps.getUnixEpoch(_gps.getUnixEpoch());

  // Need to check if the epoch time is more that 1713000000 seconds, as it sometimes returns time in the 1500000000 range
  if (new_epoch_time_microseconds > 1713000000 && data.time_valid && data.time_confirmed)
  {
    data.epoch_microseconds = new_epoch_time_microseconds;
    data.year = _gps.getYear();
    data.month = _gps.getMonth();
    data.day = _gps.getDay();
    data.hour = _gps.getHour();
    data.minute = _gps.getMinute();
    data.second = _gps.getSecond();
    data.millisecond = _gps.getMillisecond();
    return true;
  }
  return false;
}

bool GPS::_read_position()
{
  data.fix_type = _gps.getFixType();

  long new_gps_lat_raw = _gps.getLatitude();
  long new_gps_lng_raw = _gps.getLongitude();

  double new_gps_lat = new_gps_lat_raw / 10000000.0;
  double new_gps_lng = new_gps_lng_raw / 10000000.0;
  float new_gps_alt = _gps.getAltitude() / 1000.0;
  int new_satellites = _gps.getSIV();

  // SANITY CHECK
  // Check if location is somewhere in the northern eastern Europe adn we have more than 3 satellites
  if (new_satellites > 3)
  {
    if (((50 <= new_gps_lat && new_gps_lat <= 60) && (15 <= new_gps_lng && new_gps_lng <= 35)))
    {
      data.position_valid = true;
      data.lat = new_gps_lat;
      data.lng = new_gps_lng;
      data.altitude = new_gps_alt;
      data.satellites = new_satellites;
      data.ground_speed = _gps.getGroundSpeed() / 1000.0;
      data.heading = _gps.getHeading() / 100000.0;
      data.pdop = _gps.getPDOP() / 100.0;
      return true;
    }
    else
    {
      info("Location is not valid: " + String(new_gps_lat, 8) + " | " + String(new_gps_lng, 8) + " | " + String(new_gps_alt, 2) + " | " + String(new_satellites));
      return false;
    }
  }

  return false;
}

bool GPS::begin(const Config &config)
{
  // Copy the config to the local config
  _config = config;

  if (!_gps.begin(*(_config.wire), config.i2c_address))
  {
    error("Failed to begin");
    return false;
  }

  // Configure module
  if (!_configure())
  {
    error("Configure failed");
    return false;
  }

  info("GPS initialized");
  set_initialized(true);
  return true;
}

bool GPS::read()
{
  // Check if the GPS is initialized and if we have a new PVT solution
  if (!initialized() || !_gps.getPVT())
  {
    return false;
  }

  // Check if we need to read the GPS
  if ((millis() - _last_measurement_time) < _config.read_interval)
  {
    return false;
  }

  // Read the time and position
  _read_time();
  _read_position();

  return true;
}
