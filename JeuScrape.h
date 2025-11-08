#ifndef JEUSCRAPE_H
#define JEUSCRAPE_H

#include <string>
#include <tinyxml2.h>

class JeuScrape : public tinyxml2::XMLDocument {

 public:
  std::string getNumeroDeJeu();
  //void telecharge_miniature();
};

#endif
