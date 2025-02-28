#ifndef SCREEN_INTERFACE_HPP
#define SCREEN_INTERFACE_HPP

#include "display.hpp"

#include <string>
#include <vector>
#include <map>

class Screen
{
protected:
  Display *display;

public:
  virtual void execute(std::map<std::string, std::string>) = 0;
};

#endif // SCREEN_INTERFACE_HPP