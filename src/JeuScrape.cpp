#include "JeuScrape.h"

std::string JeuScrape::urlMiniature(){
  tinyxml2::XMLElement* data = this->RootElement();
  tinyxml2::XMLElement* jeu  = data->FirstChildElement("jeu");
  tinyxml2::XMLElement* medias  = jeu->FirstChildElement("medias");
  
  // Ordre de préférence des régions
  std::vector<std::string> priority = {"fr", "eu", "wor", "us"};

  tinyxml2::XMLElement* bestMedia = nullptr;

  // On va stocker les candidats par région
  std::map<std::string, tinyxml2::XMLElement*> mediaByRegion;

  for (tinyxml2::XMLElement* m = medias->FirstChildElement("media"); m; m = m->NextSiblingElement("media")) {
    const char* type   = m->Attribute("type");
    const char* region = m->Attribute("region");

    if (!type || !region) continue;

    // On ne garde que les media de type "mixrbv1"
    if (strcmp(type, "mixrbv1") == 0) {
      mediaByRegion[region] = m;
    }
  }

  // Sélection de la meilleure région disponible
  for (const std::string& r : priority) {
    if (mediaByRegion.count(r)) {
      bestMedia = mediaByRegion[r];
      break;
    }
  }

  if (bestMedia) {
    const char* url = bestMedia->GetText();
    if (url)
      return std::string(url);
  }
  return std::string();
}

std::string JeuScrape::getNumeroDeJeu(){
  tinyxml2::XMLElement* data = this->RootElement();
  tinyxml2::XMLElement* jeu  = data->FirstChildElement("jeu");
  return jeu->Attribute("id");
}

void JeuScrape::sauvegarder(std::string chemin){
  this->SaveFile(chemin.c_str());
}

void JeuScrape::telechargeMiniature(std::string chemin){
  /*std::cout << urlMiniature() << std::endl;
    std::cout << chemin << std::endl;*/
  std::string url = urlMiniature();
  if(url!= "")
    ScreenScraper::telechargeImg(url, chemin);
}
