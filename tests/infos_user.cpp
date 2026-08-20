#include <iostream>

#include "ScreenScraper.h"


int main() {
  ScreenScraper ss(std::getenv("SSDEVID"), std::getenv("SSDEVPASSWD"), std::getenv("SSDEVPASSWDDEBUG"), std::getenv("SSID"), std::getenv("SSPASSWD"));

  std::cout << ss.numid() << std::endl;

  return 0;
}
