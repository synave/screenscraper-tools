#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
//#include <cstring>
#include <unordered_map>
#include <algorithm>

#include <curl/curl.h>

#include <tinyxml2.h>

#include <filesystem>

#include "Rom.h"
#include "JeuScrape.h"
#include "ScreenScraper.h"
#include "Temps.h"


int main(int argc, char* argv[]) {
  ScreenScraper ss(std::getenv("SSDEVID"), std::getenv("SSDEVPASSWD"), std::getenv("SSDEVPASSWDDEBUG"), std::getenv("SSID"), std::getenv("SSPASSWD"));

  std::cout << ss.numid() << std::endl;

  return 0;
}
