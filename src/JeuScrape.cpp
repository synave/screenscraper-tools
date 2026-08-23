#include "JeuScrape.h"

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

std::optional<std::string> JeuScrape::existe_video(std::string type) const
{
  tinyxml2::XMLDocument copie;
  this->DeepCopy(&copie);
  tinyxml2::XMLElement* data = copie.RootElement();
  tinyxml2::XMLElement* jeu  = data->FirstChildElement("jeu");
  tinyxml2::XMLElement* medias  = jeu->FirstChildElement("medias");
  tinyxml2::XMLElement* media  = medias->FirstChildElement("media");

  if(media==NULL) return std::nullopt;
    
  while(type.compare(std::string(media->Attribute("type"))))
    {
      media=media->NextSiblingElement();
      if(media == NULL) return std::nullopt;
    }
  return std::string(media->GetText());
}

bool JeuScrape::telecharge_video(std::string type, std::string chemin) const
{ 
  //auto url = existe_miniature(type, region);
  std::optional<std::string> url = existe_video(type);

  if(!url.has_value())
    return false;

  // Ici url n'est pas un pointeur ! *url permet de récupérer la valeur du std::string contenu dans la l'objet url
  Requete::api_to_save_file(*url, chemin);
  return true;
}

std::optional<std::string> JeuScrape::existe_miniature(std::string type, std::string region) const
{
  tinyxml2::XMLDocument copie;
  this->DeepCopy(&copie);
  tinyxml2::XMLElement* data = copie.RootElement();
  tinyxml2::XMLElement* jeu  = data->FirstChildElement("jeu");
  tinyxml2::XMLElement* medias  = jeu->FirstChildElement("medias");
  tinyxml2::XMLElement* media  = medias->FirstChildElement("media");

  if(media==NULL) return std::nullopt;
    
  while(type.compare(std::string(media->Attribute("type"))) || region.compare(std::string(media->Attribute("region"))))
    {
      media=media->NextSiblingElement();
      if(media == NULL) return std::nullopt;
    }
  return std::string(media->GetText());
}

bool JeuScrape::telecharge_miniature(std::string type, std::string region, std::string chemin) const
{ 
  //auto url = existe_miniature(type, region);
  std::optional<std::string> url = existe_miniature(type, region);

  if(!url.has_value())
    return false;

  // Ici url n'est pas un pointeur ! *url permet de récupérer la valeur du std::string contenu dans la l'objet url
  Requete::api_to_save_file(*url, chemin);
  return true;
}

std::optional<std::string> JeuScrape::existe_manuel(std::string region) const
{
  tinyxml2::XMLDocument copie;
  this->DeepCopy(&copie);
  tinyxml2::XMLElement* data = copie.RootElement();
  tinyxml2::XMLElement* jeu  = data->FirstChildElement("jeu");
  tinyxml2::XMLElement* medias  = jeu->FirstChildElement("medias");
  tinyxml2::XMLElement* media  = medias->FirstChildElement("media");

  if(media==NULL) return std::nullopt;

  std::string type("manuel");
    
  while(type.compare(std::string(media->Attribute("type"))) || region.compare(std::string(media->Attribute("region"))))
    {
      media=media->NextSiblingElement();
      if(media == NULL) return std::nullopt;
    }
  return std::string(media->GetText());
}

bool JeuScrape::telecharge_manuel(std::string region, std::string chemin) const
{ 
  //auto url = existe_miniature(type, region);
  std::optional<std::string> url = existe_manuel(region);

  if(!url.has_value())
    return false;

  // Ici url n'est pas un pointeur ! *url permet de récupérer la valeur du std::string contenu dans la l'objet url
  Requete::api_to_save_file(*url, chemin);
  return true;
}

tinyxml2::XMLError JeuScrape::sauver_fichier_xml(std::string chemin) const
{
  tinyxml2::XMLDocument to_save;
  this->DeepCopy(&to_save);
  return to_save.SaveFile(chemin.c_str());
}
