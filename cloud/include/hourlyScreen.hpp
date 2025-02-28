#ifndef HOURLY_SCREEN_INTERFACE_HPP
#define HOURLY_SCREEN_INTERFACE_HPP

#include "screen.hpp"

class HourlyScreen : public Screen
{
public:
  HourlyScreen(Display *display);
  void execute(std::map<std::string, std::string> hourlyData) override;
};

#endif // HOURLY_SCREEN_INTERFACE_HPP