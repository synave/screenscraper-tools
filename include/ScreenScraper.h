#ifndef SCREENSCRAPER_H
#define SCREENSCRAPER_H

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <curl/curl.h>

#include "JeuScrape.h"
class JeuScrape;
class ScreenScraper {
private :
  // inline static std::string devid = std::getenv("SSDEVID");
  // inline static std::string devpassword = std::getenv("SSDEVPASSWD");
  // inline static std::string devpasswd_debug = std::getenv("SSDEVPASSWDDEBUG");
  // inline static std::string ssid = std::getenv("SSID");
  // inline static std::string sspassword = std::getenv("SSPASSWD");

  static size_t write_to_string(void* contents, size_t size, size_t nmemb, void* userp);
  static size_t write_to_file(void *ptr, size_t size, size_t nmemb, void *userdata);

public :
  ScreeScraper(const std::string& devid,
	       const std::string& devpassword,
	       const std::string& devpassword_debug = "",
	       const std::string& ssid = "",
	       const std::string& sspassword = ""){
    ConfigSS.devid = devid;
    ConfigSS.devpassword = devpassword;
    ConfigSS.devpassword_debug = devpassword_debug;
    ConfigSS.ssid = ssvid;
    ConfigSS.sspassword = sspassword;
  };
  
  JeuScrape* recherche_jeu_par_CRC(const std::string& crc);
  JeuScrape* recherche_jeu_par_MD5(const std::string& md5);
  void telechargeImg(std::string url, std::string outfile);
  
};

#endif
