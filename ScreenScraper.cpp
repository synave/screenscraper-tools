#include "ScreenScraper.h"

// callback libcurl pour écrire la réponse dans une string
size_t ScreenScraper::write_to_string(void* contents, size_t size, size_t nmemb, void* userp) {
  size_t real_size = size * nmemb;
  std::string* mem = static_cast<std::string*>(userp);
  mem->append(static_cast<char*>(contents), real_size);
  return real_size;
}

JeuScrape* ScreenScraper::recherche_jeu_par_CRC(const std::string& crc){
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
}

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
