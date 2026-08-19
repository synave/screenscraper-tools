#ifndef SCREENSCRAPER_H
#define SCREENSCRAPER_H

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <tinyxml2.h>

#include "JeuScrape.h"

class JeuScrape;
class ScreenScraper {
private :
  std::string _devid;
  std::string _devpassword;
  std::string _devpassword_debug;
  std::string _ssid;
  std::string _sspassword;

  int _numid;
  int _niveau;
  int _maxthreads;
  int _maxdownloadspeed;
  int _requeststoday;
  int _requestskotoday;
  int _maxrequestspermin;
  int _maxrequestsperday;
  int _maxrequestskoperday;

  static size_t write_to_string(void* contents, size_t size, size_t nmemb, void* userp);
  static size_t write_to_file(void *ptr, size_t size, size_t nmemb, void *userdata);

  tinyxml2::XMLDocument* requete(const std::string& url);

public :
  std::vector<std::string> extensions_connues = {
    ".zip", ".gb", ".gba", ".gbc", ".cue", ".sfc", ".md", ".nes"
  };
  
  ScreenScraper(const std::string& devid,
		const std::string& devpassword,
		const std::string& devpassword_debug,
		const std::string& ssid,
		const std::string& sspassword);

  inline const int& numid() const {return _numid;};
  inline const int& niveau() const {return _niveau;};
  inline const int& maxthreads() const {return _maxthreads;};
  inline const int& maxdownloadspeed() const {return _maxdownloadspeed;};
  inline const int& requeststoday() const {return _requeststoday;};
  inline const int& requestskotoday() const {return _requestskotoday;};
  inline const int& maxrequestspermin() const {return _maxrequestspermin;};
  inline const int& maxrequestsperday() const {return _maxrequestsperday;};
  inline const int& maxrequestskoperday() const {return _maxrequestskoperday;};
  
  JeuScrape* recherche_jeu_par_CRC(const std::string& crc);
  tinyxml2::XMLDocument* infos_utilisateur();
  
  //JeuScrape* recherche_jeu_par_MD5(const std::string& md5);
  //void telechargeImg(std::string url, std::string outfile);
  
};

#endif
