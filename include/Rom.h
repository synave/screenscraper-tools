#ifndef ROM_H
#define ROM_H

#include <tinyxml2.h>

#include "Cryptage.h"
#include "JeuScrape.h"
#include "ScreenScraper.h"

class Rom {
 private :
  const std::string chemin;
  JeuScrape *_scrape;

 public:
  Rom(const std::string& path);

  inline const JeuScrape* jeuScrape(){return _scrape;};
  
  const std::string getChemin(){return this->chemin;}
  const std::string getCRC() const {return Cryptage::crc(chemin);}
  std::string getMD5(){return Cryptage::md5(chemin);}
  std::string getSHA1(){return Cryptage::sha1(chemin);}
  
  bool scrape(const ScreenScraper& ss);

};

#endif
