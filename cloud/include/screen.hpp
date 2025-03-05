#ifndef SCREEN_INTERFACE_HPP
#define SCREEN_INTERFACE_HPP

#include "display.hpp"
#include "hourlyDomain.hpp"

class Screen
{
protected:
  Display *display;

public:
  virtual void execute(HourlyDomain) = 0;
};

#endif // SCREEN_INTERFACE_HPP