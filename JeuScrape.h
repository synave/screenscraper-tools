#ifndef JEUSCRAPE_H
#define JEUSCRAPE_H

#include <string>
#include <tinyxml2.h>

class JeuScrape : public tinyxml2::XMLDocument {

  std::string *chemin = NULL;
  
 public:
  std::string getNumeroDeJeu();
  void setChemin(const std::string& path);
  //void telecharge_miniature();
};

#endif
