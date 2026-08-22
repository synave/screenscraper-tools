#include "ScreenScraper.h"

/**** METHODES STATIC ****/



/**** CONSTRUCTEURS ****/
ScreenScraper::ScreenScraper(const std::string& devid,
	       const std::string& devpassword,
	       const std::string& devpassword_debug = "",
	       const std::string& ssid = "",
	       const std::string& sspassword = ""){
    this->_devid = devid;
    this->_devpassword = devpassword;
    this->_devpassword_debug = devpassword_debug;
    this->_ssid = ssid;
    this->_sspassword = sspassword;
#ifdef DEBUG
    std::cerr << "INFO : Récupération des informations de l'utilisateur" << std::endl;
#endif
    tinyxml2::XMLDocument* infos = this->infos_utilisateur();
    if(infos == NULL){
      std::cout << "Erreur : vérifiez les identifiants développeurs" << std::endl;
      exit(1);
    }
    tinyxml2::XMLElement* data = infos->RootElement();
    tinyxml2::XMLElement* user  = data->FirstChildElement("ssuser");
    this->_numid = std::stoi(user->FirstChildElement("numid")->GetText());
    this->_niveau = std::stoi(user->FirstChildElement("niveau")->GetText());
    this->_maxthreads = std::stoi(user->FirstChildElement("maxthreads")->GetText());
    this->_maxdownloadspeed = std::stoi(user->FirstChildElement("maxdownloadspeed")->GetText());
    this->_requeststoday = std::stoi(user->FirstChildElement("requeststoday")->GetText());
    this->_requestskotoday = std::stoi(user->FirstChildElement("requestskotoday")->GetText());
    this->_maxrequestspermin = std::stoi(user->FirstChildElement("maxrequestspermin")->GetText());
    this->_maxrequestsperday = std::stoi(user->FirstChildElement("maxrequestsperday")->GetText());
    this->_maxrequestskoperday = std::stoi(user->FirstChildElement("maxrequestskoperday")->GetText());
}


/**** METHODES PRIVEES ****/


/**** METHODES PUBLICS ****/

tinyxml2::XMLDocument* ScreenScraper::infos_utilisateur(){
  std::string base("https://api.screenscraper.fr/api2/ssuserInfos.php");
  
  std::vector<std::string> noms;
  noms.push_back(std::string("devid"));
  noms.push_back(std::string("devpassword"));
  noms.push_back(std::string("output"));
  noms.push_back(std::string("ssid"));
  noms.push_back(std::string("sspassword"));
  
  std::vector<std::string> valeurs;
  valeurs.push_back(_devid);
  valeurs.push_back(_devpassword);
  valeurs.push_back(std::string("xml"));
  valeurs.push_back(_ssid);
  valeurs.push_back(_sspassword);

  return Requete::api_to_xml(base, noms, valeurs);
}

JeuScrape* ScreenScraper::recherche_jeu_par_CRC(const std::string& crc) const
{
  std::string base("https://api.screenscraper.fr/api2/jeuInfos.php");
  
  std::vector<std::string> noms;
  noms.push_back(std::string("devid"));
  noms.push_back(std::string("devpassword"));
  noms.push_back(std::string("output"));
  noms.push_back(std::string("ssid"));
  noms.push_back(std::string("sspassword"));
  noms.push_back(std::string("crc"));
  
  std::vector<std::string> valeurs;
  valeurs.push_back(_devid);
  valeurs.push_back(_devpassword);
  valeurs.push_back(std::string("xml"));
  valeurs.push_back(_ssid);
  valeurs.push_back(_sspassword);
  valeurs.push_back(std::string(crc));

  tinyxml2::XMLDocument* xml_document = Requete::api_to_xml(base, noms, valeurs);
  
  if(xml_document == NULL) return NULL;
  
  JeuScrape *retour = new JeuScrape();
  xml_document->DeepCopy(retour);
  return retour;
}

JeuScrape* ScreenScraper::recherche_jeu_par_MD5(const std::string& md5) const
{
  std::string base("https://api.screenscraper.fr/api2/jeuInfos.php");
  
  std::vector<std::string> noms;
  noms.push_back(std::string("devid"));
  noms.push_back(std::string("devpassword"));
  noms.push_back(std::string("output"));
  noms.push_back(std::string("ssid"));
  noms.push_back(std::string("sspassword"));
  noms.push_back(std::string("md5"));
  
  std::vector<std::string> valeurs;
  valeurs.push_back(_devid);
  valeurs.push_back(_devpassword);
  valeurs.push_back(std::string("xml"));
  valeurs.push_back(_ssid);
  valeurs.push_back(_sspassword);
  valeurs.push_back(std::string(md5));

  tinyxml2::XMLDocument* xml_document = Requete::api_to_xml(base, noms, valeurs);
  
  if(xml_document == NULL) return NULL;
  
  JeuScrape *retour = new JeuScrape();
  xml_document->DeepCopy(retour);
  return retour;
}

JeuScrape* ScreenScraper::recherche_jeu_par_SHA1(const std::string& sha1) const
{
  std::string base("https://api.screenscraper.fr/api2/jeuInfos.php");
  
  std::vector<std::string> noms;
  noms.push_back(std::string("devid"));
  noms.push_back(std::string("devpassword"));
  noms.push_back(std::string("output"));
  noms.push_back(std::string("ssid"));
  noms.push_back(std::string("sspassword"));
  noms.push_back(std::string("sha1"));
  
  std::vector<std::string> valeurs;
  valeurs.push_back(_devid);
  valeurs.push_back(_devpassword);
  valeurs.push_back(std::string("xml"));
  valeurs.push_back(_ssid);
  valeurs.push_back(_sspassword);
  valeurs.push_back(std::string(sha1));

  tinyxml2::XMLDocument* xml_document = Requete::api_to_xml(base, noms, valeurs);
  
  if(xml_document == NULL) return NULL;
  
  JeuScrape *retour = new JeuScrape();
  xml_document->DeepCopy(retour);
  return retour;
}
