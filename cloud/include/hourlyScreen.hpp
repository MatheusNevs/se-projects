#ifndef HOURLY_SCREEN_INTERFACE_HPP
#define HOURLY_SCREEN_INTERFACE_HPP

#include "screen.hpp"
#include <cstdio>

class HourlyScreen : public Screen
{
public:
  HourlyScreen(Display *display);
  void execute(HourlyDomain hourlyData) override;
};

#endif // HOURLY_SCREEN_INTERFACE_HPP