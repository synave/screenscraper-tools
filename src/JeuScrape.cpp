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

std::string JeuScrape::getNumeroDeJeu() const{
  tinyxml2::XMLDocument copie;
  this->DeepCopy(&copie);
  tinyxml2::XMLElement* data = copie.RootElement();
  tinyxml2::XMLElement* jeu  = data->FirstChildElement("jeu");
  return jeu->Attribute("id");
}

std::string* JeuScrape::getSynopsis(std::string lg) const{
  tinyxml2::XMLDocument copie;
  this->DeepCopy(&copie);
  tinyxml2::XMLElement* data = copie.RootElement();
  tinyxml2::XMLElement* jeu  = data->FirstChildElement("jeu");
  tinyxml2::XMLElement* synopsis  = jeu->FirstChildElement("synopsis");
  tinyxml2::XMLElement* langue  = synopsis->FirstChildElement("synopsis");
  #ifdef DEBUG
  std::cerr << "INFO : synopsis langue : " << langue->Attribute("langue") << std::endl;
  #endif
  if(langue==NULL)
    return NULL;
  while(lg.compare(std::string(langue->Attribute("langue"))))
    {
      langue=langue->NextSiblingElement();
      if(langue == NULL) return NULL;
#ifdef DEBUG
      std::cerr << "INFO : synopsis langue : " << langue->Attribute("langue") << std::endl;
#endif
    }
  
  return new std::string(langue->GetText());
}

void JeuScrape::telecharge_miniature(std::string type, std::string region, std::string chemin) const
{
  tinyxml2::XMLDocument copie;
  this->DeepCopy(&copie);
  tinyxml2::XMLElement* data = copie.RootElement();
  tinyxml2::XMLElement* jeu  = data->FirstChildElement("jeu");
  tinyxml2::XMLElement* medias  = jeu->FirstChildElement("medias");
  tinyxml2::XMLElement* media  = medias->FirstChildElement("media");

  if(media==NULL) return;
    
  while(type.compare(std::string(media->Attribute("type"))) || region.compare(std::string(media->Attribute("region"))))
    {
      media=media->NextSiblingElement();
      if(media == NULL) return;
    }
  ScreenScraper::telechargeImg(std::string(media->GetText()), chemin);
}

/*void JeuScrape::sauvegarder(std::string chemin){
  this->SaveFile(chemin.c_str());
}

void JeuScrape::telechargeMiniature(std::string chemin){
  #ifdef DEBUG
  std::cerr << urlMiniature() << std::endl;
  std::cerr << chemin << std::endl;
  #endif
  std::string url = urlMiniature();
  if(url!= "")
    ScreenScraper::telechargeImg(url, chemin);
    }*/
