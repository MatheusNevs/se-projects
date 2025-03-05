#ifndef HOURLY_DOMAIN_HPP
#define HOURLY_DOMAIN_HPP

class HourlyDomain
{
private:
  uint8_t hour;
  uint8_t minutes;
  double temperature;
  double humidity;
  double precipitation;

public:
  // Constructor with default values
  HourlyDomain(uint8_t h = 0, uint8_t m = 0, double temp = 00.0, double hum = 00.0, double precip = 0.0)
      : hour(h), minutes(m), temperature(temp), humidity(hum), precipitation(precip) {}

  // Getters
  uint8_t getHour() const { return hour; }
  uint8_t getMinutes() const { return minutes; }
  double getTemperature() const { return temperature; }
  double getHumidity() const { return humidity; }
  double getPrecipitation() const { return precipitation; }

  // Setters
  void setHour(uint8_t h) { hour = h; }
  void setMinutes(uint8_t m) { minutes = m; }
  void setTemperature(double temp) { temperature = temp; }
  void setHumidity(double hum) { humidity = hum; }
  void setPrecipitation(double precip) { precipitation = precip; }
};

#endif // HOURLY_DOMAIN_HPP