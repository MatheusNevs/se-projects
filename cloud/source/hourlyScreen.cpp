#include "hourlyScreen.hpp"

HourlyScreen::HourlyScreen(Display *display)
{
  this->display = display;
}

void HourlyScreen::execute(HourlyDomain hourlyData)
{
  display->clear_display();

  // Draw the first two lines, title and hour description
  display->draw_string(4, "Hourly Forecast", 1);
  char message[32]; // Fixed-size buffer
  sprintf(message, "%s:%s", hourlyData.getHour(), hourlyData.getMinutes());
  display->draw_string(4, message, 1);

  display->draw_line(0, 128, 0, 0);

  // Now, draw the precipitation, temperature and humidity
  sprintf(message, "humidity: %s %", hourlyData.getHumidity());
  display->draw_string(4, message, 1);
}