#ifndef JEUSCRAPE_H
#define JEUSCRAPE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <tinyxml2.h>

#include "ScreenScraper.h"

class JeuScrape : public tinyxml2::XMLDocument {

private:
  std::string urlMiniature();
  
 public:
  std::string getNumeroDeJeu();
  void sauvegarder(std::string chemin);
  void telechargeMiniature(std::string chemin);
};

#endif
