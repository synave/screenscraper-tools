#include "JeuScrape.h"

std::string JeuScrape::getNumeroDeJeu(){
  tinyxml2::XMLElement* data = this->RootElement();
  tinyxml2::XMLElement* jeu  = data->FirstChildElement("jeu");
  return jeu->Attribute("id");
}

void JeuScrape::setChemin(const std::string& path){
  chemin = new std::string(path);
}
