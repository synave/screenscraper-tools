#ifndef SCREENSCRAPER_H
#define SCREENSCRAPER_H

#include <iostream>
#include <sstream>
#include <string>
#include <curl/curl.h>

#include "JeuScrape.h"
class JeuScrape;
class ScreenScraper {
 private :
  inline static std::string devid = SSDEVID;
  inline static std::string devpassword = SSDEVPASSWD;
  inline static std::string devpasswd_debug = SSDEVPASSWDDEBUG;
  inline static std::string ssid = SSID;
  inline static std::string sspassword = SSPASSWD;

  static size_t write_to_string(void* contents, size_t size, size_t nmemb, void* userp);
  static size_t write_to_file(void *ptr, size_t size, size_t nmemb, void *userdata);

 public :

  static JeuScrape* recherche_jeu_par_CRC(const std::string& crc);
  static JeuScrape* recherche_jeu_par_MD5(const std::string& md5);
  static void telechargeImg(std::string url, std::string outfile);
  
};

#endif
