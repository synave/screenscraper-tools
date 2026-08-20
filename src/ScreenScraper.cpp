#include "ScreenScraper.h"

ScreenScraper::ScreenScraper(const std::string& devid,
	       const std::string& devpassword,
	       const std::string& devpassword_debug = "",
	       const std::string& ssid = "",
	       const std::string& sspassword = ""){
    ScreenScraper::_devid = devid;
    ScreenScraper::_devpassword = devpassword;
    ScreenScraper::_devpassword_debug = devpassword_debug;
    ScreenScraper::_ssid = ssid;
    ScreenScraper::_sspassword = sspassword;
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

tinyxml2::XMLDocument* ScreenScraper::request(const std::string& url) const{
// Construire l'URL avec encodage des paramètres via curl_easy_escape
  CURL* curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Impossible d'initialiser libcurl\n";
    exit(EXIT_FAILURE);
  }
  /*char* enc_url = curl_easy_escape(curl, url.c_str(), 0);

  std::ostringstream os_url;
  os_url << enc_url;

  std::string full_url = os_url.str();*/

  #ifdef DEBUG
  std::cerr << "INFO : " << url << std::endl;
  #endif
   
  // libcurl options et exécution GET
  std::string response;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ScreenScraper::write_to_string);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  // Optionnel : timeout etc.
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    std::cerr << "Erreur curl: " << curl_easy_strerror(res) << "\n";
    // cleanup
    curl_easy_cleanup(curl);
    exit(EXIT_FAILURE);
  }

  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

  // cleanup
  curl_easy_cleanup(curl);
  
  if(http_code == 200){
    tinyxml2::XMLDocument *retour = new tinyxml2::XMLDocument();
    retour->Parse(response.c_str());
    return retour;
  }
  
  return NULL;
}

// callback libcurl pour écrire la réponse dans une string
size_t ScreenScraper::write_to_string(void* contents, size_t size, size_t nmemb, void* userp) {
  size_t real_size = size * nmemb;
  std::string* mem = static_cast<std::string*>(userp);
  mem->append(static_cast<char*>(contents), real_size);
  return real_size;
}

/* callback d'écriture — écrit les données reçues dans le FILE* passé via userdata */
size_t ScreenScraper::write_to_file(void *ptr, size_t size, size_t nmemb, void *userdata) {
  FILE *fp = (FILE *)userdata;
  size_t written = fwrite(ptr, size, nmemb, fp);
  return written;
}

tinyxml2::XMLDocument* ScreenScraper::infos_utilisateur(){
  std::string requete = "https://api.screenscraper.fr/api2/ssuserInfos.php";
  requete += "?devid="; requete += _devid;
  requete += "&devpassword="; requete += _devpassword;
  requete += "&output=xml";
  requete += "&ssid="; requete += _ssid;
  requete += "&sspassword="; requete += _sspassword;

  return this->request(requete);
}

JeuScrape* ScreenScraper::recherche_jeu_par_CRC(const std::string& crc) const{
  std::string requete;
  requete += "https://api.screenscraper.fr/api2/jeuInfos.php";
  requete += "?devid="; requete += _devid;
  requete += "&devpassword="; requete += _devpassword;
  requete += "&output=xml";
  requete += "&ssid="; requete += _ssid;
  requete += "&sspassword="; requete += _sspassword;
  requete += "&crc="; requete += crc;

  tinyxml2::XMLDocument* xml_document = this->request(requete);
  
  if(xml_document == NULL) return NULL;
  
  JeuScrape *retour = new JeuScrape();
  xml_document->DeepCopy(retour);
  return retour;
  
  
  /*
  // Construire l'URL avec encodage des paramètres via curl_easy_escape
  CURL* curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Impossible d'initialiser libcurl\n";
    exit(EXIT_FAILURE);
  }

  char* enc_devid = curl_easy_escape(curl, ScreenScraper::devid.c_str(), 0);
  char* enc_devpassword = curl_easy_escape(curl, ScreenScraper::devpassword.c_str(), 0);
  char* enc_ssid = curl_easy_escape(curl, ScreenScraper::ssid.c_str(), 0);
  char* enc_sspassword = curl_easy_escape(curl, ScreenScraper::sspassword.c_str(), 0);
  char* enc_crc = curl_easy_escape(curl, crc.c_str(), 0);

  std::ostringstream url;
  url << "https://api.screenscraper.fr/api2/jeuInfos.php"
      << "?devid=" << (enc_devid ? enc_devid : "")
      << "&devpassword=" << (enc_devpassword ? enc_devpassword : "")
      << "&output=xml"
      << "&ssid=" << (enc_ssid ? enc_ssid : "")
      << "&sspassword=" << (enc_sspassword ? enc_sspassword : "")
      << "&crc=" << enc_crc;

  std::string full_url = url.str();

  #ifdef DEBUG
  std::cerr << "INFO : " << full_url << std::endl;
  #endif
   
  // libcurl options et exécution GET
  std::string response;
  curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  // Optionnel : timeout etc.
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    std::cerr << "Erreur curl: " << curl_easy_strerror(res) << "\n";
    // cleanup
    if (enc_devid) curl_free(enc_devid);
    if (enc_devpassword) curl_free(enc_devpassword);
    if (enc_ssid) curl_free(enc_ssid);
    if (enc_sspassword) curl_free(enc_sspassword);
    if (enc_crc) curl_free(enc_crc);
    curl_easy_cleanup(curl);
    exit(EXIT_FAILURE);
  }

  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

  // cleanup
  if (enc_devid) curl_free(enc_devid);
  if (enc_devpassword) curl_free(enc_devpassword);
  if (enc_ssid) curl_free(enc_ssid);
  if (enc_sspassword) curl_free(enc_sspassword);
  if (enc_crc) curl_free(enc_crc);
  curl_easy_cleanup(curl);
  
  if(http_code == 200){
    JeuScrape *retour = new JeuScrape();
    retour->Parse(response.c_str());
    return retour;
  }
  
  return NULL;
  */
}


/*
JeuScrape* ScreenScraper::recherche_jeu_par_MD5(const std::string& md5){
  // Construire l'URL avec encodage des paramètres via curl_easy_escape
  CURL* curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Impossible d'initialiser libcurl\n";
    exit(EXIT_FAILURE);
  }

  char* enc_devid = curl_easy_escape(curl, ScreenScraper::devid.c_str(), 0);
  char* enc_devpassword = curl_easy_escape(curl, ScreenScraper::devpassword.c_str(), 0);
  char* enc_ssid = curl_easy_escape(curl, ScreenScraper::ssid.c_str(), 0);
  char* enc_sspassword = curl_easy_escape(curl, ScreenScraper::sspassword.c_str(), 0);
  char* enc_md5 = curl_easy_escape(curl, md5.c_str(), 0);

  std::ostringstream url;
  url << "https://api.screenscraper.fr/api2/jeuInfos.php"
      << "?devid=" << (enc_devid ? enc_devid : "")
      << "&devpassword=" << (enc_devpassword ? enc_devpassword : "")
      << "&output=xml"
      << "&ssid=" << (enc_ssid ? enc_ssid : "")
      << "&sspassword=" << (enc_sspassword ? enc_sspassword : "")
      << "&md5=" << enc_md5;

  std::string full_url = url.str();
   
  // libcurl options et exécution GET
  std::string response;
  curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  // Optionnel : timeout etc.
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    std::cerr << "Erreur curl: " << curl_easy_strerror(res) << "\n";
    // cleanup
    if (enc_devid) curl_free(enc_devid);
    if (enc_devpassword) curl_free(enc_devpassword);
    if (enc_ssid) curl_free(enc_ssid);
    if (enc_sspassword) curl_free(enc_sspassword);
    if (enc_md5) curl_free(enc_md5);
    curl_easy_cleanup(curl);
    exit(EXIT_FAILURE);
  }

  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

  // cleanup
  if (enc_devid) curl_free(enc_devid);
  if (enc_devpassword) curl_free(enc_devpassword);
  if (enc_ssid) curl_free(enc_ssid);
  if (enc_sspassword) curl_free(enc_sspassword);
  if (enc_md5) curl_free(enc_md5);
  curl_easy_cleanup(curl);

  if(http_code == 200){
    JeuScrape *retour = new JeuScrape();
    retour->Parse(response.c_str());
    return retour;
  }
  
  return NULL;
}

void ScreenScraper::telechargeImg(std::string url, std::string outfile) {
  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Impossible d'initialiser curl" << std::endl;
    exit(EXIT_FAILURE);
  }

  // ouvrir le fichier en mode binaire
  FILE *fp = fopen(outfile.c_str(), "wb");
  if (!fp) {
    std::cerr << "Impossible d'ouvrir le fichier " << url << " en écriture\n";
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    exit(EXIT_FAILURE);
  }
  
  // libcurl options et exécution GET
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_file);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
  // Optionnel : timeout etc.
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  // exécution
  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    std::cerr << "Erreur curl: " << curl_easy_strerror(res) << std::endl;
    fclose(fp);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    exit(EXIT_FAILURE);
  }

  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  if (http_code != 200L)
    std::cerr << "HTTP status: "<< http_code << "(attendu 200)" << std::endl;
   

  fclose(fp);
  curl_easy_cleanup(curl);
  curl_global_cleanup();
}
*/
