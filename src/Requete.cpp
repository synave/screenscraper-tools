#include "Requete.h"

// callback libcurl pour écrire la réponse dans une string
size_t Requete::write_to_string(void* contents, size_t size, size_t nmemb, void* userp) {
  size_t real_size = size * nmemb;
  std::string* mem = static_cast<std::string*>(userp);
  mem->append(static_cast<char*>(contents), real_size);
  return real_size;
}

/* callback d'écriture — écrit les données reçues dans le FILE* passé via userdata */
size_t Requete::write_to_file(void *ptr, size_t size, size_t nmemb, void *userdata) {
  FILE *fp = (FILE *)userdata;
  size_t written = fwrite(ptr, size, nmemb, fp);
  return written;
}

/*
tinyxml2::XMLDocument* Requete::request(const std::string& url) const{

  CURL* curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Impossible d'initialiser libcurl\n";
    exit(EXIT_FAILURE);
  }
   
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
}*/

tinyxml2::XMLDocument* Requete::api_to_xml(const std::string& base_url, std::vector<std::string> parameter_names, std::vector<std::string> parameter_values)
{
  if(parameter_names.size() != parameter_values.size())
    {
      std::cout << "ERREUR" << std::endl
		<< "tinyxml2::XMLDocument* request(const std::string& base_url, const std::vector<const std::string&> parameter_names, const std::vector<const std::string&> parameter_values) const;" << std::endl
		<< "nombre de paramètres et de valeurs différents" << std::endl;
      return NULL;
    }

  unsigned int nbparam = parameter_names.size();
  
  CURL* curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Impossible d'initialiser libcurl\n";
    exit(EXIT_FAILURE);
  }

  std::vector<char*> enc_parameter_values;
  for(unsigned int i=0; i<nbparam; i++)
    enc_parameter_values.push_back(curl_easy_escape(curl, parameter_values[i].c_str(), 0));

  // Construire l'URL avec encodage des paramètres via curl_easy_escape
  std::ostringstream url;
  url << base_url;
  if(nbparam != 0)
    url << "?" << parameter_names[0] << "=" << enc_parameter_values[0];
  for(unsigned int i=1; i<parameter_names.size(); i++)
    url << "&" << parameter_names[i] << "=" << enc_parameter_values[i];
  
  std::string full_url = url.str();
  
#ifdef DEBUG
  std::cerr << "INFO : " << base_url << std::endl;
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
    for(unsigned int i=0; i<nbparam; i++)
      curl_free(enc_parameter_values[i]);
    curl_easy_cleanup(curl);
    exit(EXIT_FAILURE);
  }

  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

  // cleanup
  for(unsigned int i=0; i<nbparam; i++)
      curl_free(enc_parameter_values[i]);
  curl_easy_cleanup(curl);

#ifdef DEBUG
  std::cerr << "INFO : retour appel API : " << http_code << std::endl;
#endif
  
  if(http_code == 200){
    tinyxml2::XMLDocument* retour = new tinyxml2::XMLDocument();
    retour->Parse(response.c_str());
    return retour;
  }
  
  return NULL;
}


void Requete::api_to_save_file(std::string url, std::string outfile) {
  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Impossible d'initialiser curl" << std::endl;
    exit(EXIT_FAILURE);
  }

  // ouvrir le fichier en mode binaire
  FILE *fp = fopen(outfile.c_str(), "wb");
  if (!fp) {
    std::cerr << "Impossible d'ouvrir le fichier " << outfile << " en écriture\n";
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
