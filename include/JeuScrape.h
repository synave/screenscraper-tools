#ifndef JEUSCRAPE_H
#define JEUSCRAPE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <tinyxml2.h>
#include <optional>

#include "ScreenScraper.h"

class JeuScrape : public tinyxml2::XMLDocument {

private:
  
 public:
  std::string getNumeroDeJeu() const;
  std::string* getSynopsis(std::string langue) const;
  std::optional<std::string> existe_video(std::string type) const;
  bool telecharge_video(std::string type, std::string chemin) const;
  std::optional<std::string> existe_miniature(std::string type, std::string region) const;
  bool telecharge_miniature(std::string type, std::string region, std::string chemin) const;
  std::optional<std::string> existe_manuel(std::string region) const;
  bool telecharge_manuel(std::string region, std::string chemin) const;
  tinyxml2::XMLError sauver_fichier_xml(std::string chemin) const;

};

#endif
