#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
//#include <cstring>
#include <unordered_map>
#include <algorithm>

#include <curl/curl.h>

#include <tinyxml2.h>

#include <filesystem>

#include "Rom.h"
#include "JeuScrape.h"
#include "ScreenScraper.h"



std::vector<std::pair<int, int>> group(std::string gamelist) {
  std::ifstream infile(gamelist);
  if (!infile) {
    std::cerr << "Erreur : impossible d'ouvrir le fichier " << gamelist << std::endl;
    exit(EXIT_FAILURE);
  }

  std::unordered_map<int, int> compteur;
  std::string ligne;

  // Lecture ligne par ligne
  while (std::getline(infile, ligne)) {
    std::stringstream ss(ligne);

    int numero;

    // On lit seulement le premier entier de la ligne
    if (ss >> numero) {
      compteur[numero]++;  
    }
    // Le reste de la ligne (nom de fichier avec espaces) est ignoré volontairement
  }
  infile.close();

  // Conversion en tableau à deux colonnes
  std::vector<std::pair<int, int>> tableau;
  tableau.reserve(compteur.size());

  for (const auto& entry : compteur) {
    tableau.emplace_back(entry.first, entry.second);
  }

  return tableau;
}

int getNbOccurrences(const std::vector<std::pair<int, int>>& tableau, int valeur) {
  for (const auto& p : tableau) {
    if (p.first == valeur) {
      return p.second;   // nombre d'occurrences trouvé
    }
  }
  return 0;  // valeur non trouvée
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage:\n"
	      << "  " << argv[0] << " <répertoire>\n\n";
    return EXIT_FAILURE;
  }

  std::string dir = argv[1];

  std::string fichier_gamelist = dir+"/gamelist.dat";

  if (std::filesystem::exists(fichier_gamelist)) {
    std::cerr << "Le fichier gamelist.dat existe déjà" << std::endl;
    return EXIT_FAILURE;
  }

  std::ofstream gamelist(fichier_gamelist, std::ios::trunc); // ouvre le fichier en mode ajout
  if (!gamelist) {
    std::cerr << "Impossible d'ouvrir gamelist.dat pour écriture" << std::endl;
    return EXIT_FAILURE;
  }

  // Récupération du nombre de fichiers à traiter
  int nb_fichiers = 0;
  for (const auto& entry : std::filesystem::directory_iterator(dir)) {
    if (entry.is_regular_file() && (entry.path().extension() == ".gba" || entry.path().extension() == ".zip")){
      nb_fichiers++;
    }
  }

  double nb_fichiers_parcourus = 0;
  for (const auto& entry : std::filesystem::directory_iterator(dir)) {
    if (entry.is_regular_file() && (entry.path().extension() == ".gba" || entry.path().extension() == ".zip")){
      std::cout << std::string(20, '\b') << std::string(20, ' ') << std::string(20, '\b');
      std::cout << ((nb_fichiers_parcourus/nb_fichiers)*100) << "% " << std::flush;
      nb_fichiers_parcourus++;

    
      std::string chemin_rom = dir+std::string("/")+entry.path().filename().string();
      Rom rom(chemin_rom);
      std::string crchex = rom.getCRC();
      std::string md5hex = rom.getMD5();

      /*
      // Construire l'URL avec encodage des paramètres via curl_easy_escape
      CURL* curl = curl_easy_init();
      if (!curl) {
        std::cerr << "Impossible d'initialiser libcurl\n";
        return 3;
      }

      char* enc_devid = curl_easy_escape(curl, devid.c_str(), 0);
      char* enc_devpassword = curl_easy_escape(curl, devpassword.c_str(), 0);
      char* enc_ssid = curl_easy_escape(curl, ssid.c_str(), 0);
      char* enc_sspassword = curl_easy_escape(curl, sspassword.c_str(), 0);
      char* enc_md5 = curl_easy_escape(curl, md5hex.c_str(), 0);
      char* enc_crc = curl_easy_escape(curl, crchex.c_str(), 0);

      std::ostringstream url;
      url << "https://api.screenscraper.fr/api2/jeuInfos.php"
	  << "?devid=" << (enc_devid ? enc_devid : "")
	  << "&devpassword=" << (enc_devpassword ? enc_devpassword : "")
	  << "&output=xml"
	  << "&ssid=" << (enc_ssid ? enc_ssid : "")
	  << "&sspassword=" << (enc_sspassword ? enc_sspassword : "")
	  << "&md5=" << (enc_md5 ? enc_md5 : "")
	  << "&crc=" << (enc_crc ? enc_crc : "");

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
        if (enc_crc) curl_free(enc_crc);
        curl_easy_cleanup(curl);
        return 4;
      }

      long http_code = 0;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

      // cleanup
      if (enc_devid) curl_free(enc_devid);
      if (enc_devpassword) curl_free(enc_devpassword);
      if (enc_ssid) curl_free(enc_ssid);
      if (enc_sspassword) curl_free(enc_sspassword);
      if (enc_md5) curl_free(enc_md5);
      if (enc_crc) curl_free(enc_crc);
      curl_easy_cleanup(curl);*/

      JeuScrape *jeu = ScreenScraper::recherche_jeu_par_CRC(crchex);

      if(jeu!=NULL){
	gamelist << jeu->getNumeroDeJeu() << " " << entry.path().filename().string() << std::endl;
      }else
	gamelist << "-1" << " " << entry.path().filename().string() << std::endl;
      
      /*if(http_code == 200){
	tinyxml2::XMLDocument doc;
	doc.Parse(response.c_str());
	tinyxml2::XMLElement* data = doc.RootElement();
	tinyxml2::XMLElement* jeu  = data->FirstChildElement("jeu");
	std::string jeuId = jeu->Attribute("id");

	gamelist << jeuId << " " << entry.path().filename().string() << std::endl;
      }else
	gamelist << "-1" << " " << entry.path().filename().string() << std::endl;
      */
      
    }
    
  }


  /*
  std::vector<std::pair<int, int>> tableau = group("gamelist.dat");

  std::ifstream infile("gamelist.dat");
  if (!infile) {
    std::cerr << "Erreur : impossible d'ouvrir input.txt\n";
    exit(EXIT_FAILURE);
  }

  std::string ligne;

  while (std::getline(infile, ligne)) {
    std::stringstream ss(ligne);

    int numero;
    std::string nomFichier;

    // On lit seulement le premier entier de la ligne
    ss >> numero;
    std::getline(ss, nomFichier);

    if (!nomFichier.empty() && nomFichier[0] == ' ')
      nomFichier.erase(0, 1);

    // std::cout << numero << " - " << nomFichier << std::endl;

    if(getNbOccurrences(tableau, numero) > 1 || numero < 0){
      //std::cout << "Doublon pour " << nomFichier << std::endl;
      std::filesystem::path repPath(dir+"/"+std::to_string(numero));
      if (!std::filesystem::exists(repPath))
	std::filesystem::create_directories(repPath);
      std::filesystem::rename(dir+"/"+nomFichier, dir+"/"+std::to_string(numero)+"/"+nomFichier); 
    }
  }

  std::cout << std::endl;
  */
    
  return 0;
}
