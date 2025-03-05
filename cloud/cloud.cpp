#include "display.hpp"
#include "hourlyScreen.hpp"

int main()
{
  Display display = Display(14, 15, 400000, 64, 128);
  HourlyScreen hourlyScreen = HourlyScreen(&display);

  HourlyDomain hourlyDomain = HourlyDomain();
  hourlyDomain.setHour(23);
  hourlyDomain.setMinutes(0);
  hourlyDomain.setHumidity(20.0);
  hourlyDomain.setPrecipitation(0);
  hourlyDomain.setTemperature(20.0);

  hourlyScreen.execute(hourlyDomain);
  return 0;
}