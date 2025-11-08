#include "JeuScrape.h"

std::string JeuScrape::getNumeroDeJeu(){
  tinyxml2::XMLElement* data = this->RootElement();
  tinyxml2::XMLElement* jeu  = data->FirstChildElement("jeu");
  return jeu->Attribute("id");
}
