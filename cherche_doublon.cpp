#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstring>
#include <unordered_map>
#include <algorithm>

#include <curl/curl.h>
#include <openssl/evp.h>
#include <zlib.h>

#include <tinyxml2.h>

#include <filesystem>






// callback libcurl pour écrire la réponse dans une string
static size_t write_to_string(void* contents, size_t size, size_t nmemb, void* userp) {
  size_t real_size = size * nmemb;
  std::string* mem = static_cast<std::string*>(userp);
  mem->append(static_cast<char*>(contents), real_size);
  return real_size;
}

std::string md5_of_file(const std::string &path) {
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  const EVP_MD *md = EVP_md5();

  EVP_DigestInit_ex(ctx, md, nullptr);

  std::ifstream f(path, std::ios::binary);
  std::vector<unsigned char> buffer(4096);

  while (f.good()) {
    f.read((char*)buffer.data(), buffer.size());
    EVP_DigestUpdate(ctx, buffer.data(), f.gcount());
  }

  unsigned char digest[EVP_MAX_MD_SIZE];
  unsigned int digest_len = 0;

  EVP_DigestFinal_ex(ctx, digest, &digest_len);
  EVP_MD_CTX_free(ctx);

  // convert to hex string
  static const char hex[] = "0123456789abcdef";
  std::string out;
  out.reserve(digest_len * 2);
  for (unsigned int i = 0; i < digest_len; ++i) {
    out.push_back(hex[digest[i] >> 4]);
    out.push_back(hex[digest[i] & 0xF]);
  }
  return out;
}


// calculer CRC32 d'un fichier -> renvoie hex string (sans 0x)
std::string crc32_of_file(const std::string& path) {
  unsigned char buffer[8192];
  uLong crc = crc32(0L, Z_NULL, 0);

  std::ifstream ifs(path, std::ios::binary);
  if (!ifs) {
    throw std::runtime_error("Impossible d'ouvrir le fichier pour CRC32: " + path);
  }

  while (ifs) {
    ifs.read(reinterpret_cast<char*>(buffer), sizeof(buffer));
    std::streamsize s = ifs.gcount();
    if (s > 0) crc = crc32(crc, buffer, static_cast<uInt>(s));
  }

  std::ostringstream oss;
  oss << std::hex << std::setw(8) << std::setfill('0') << (unsigned)(crc & 0xFFFFFFFFu);
  return oss.str();
}

void sort_file(std::string input, std::string output) {
  std::ifstream infile(input);
  if (!infile) {
    std::cerr << "Erreur : impossible d'ouvrir le fichier " << input << std::endl;
    exit(EXIT_FAILURE);
  }

  std::vector<std::string> lines;
  std::string line;

  // Lire les lignes du fichier
  while (std::getline(infile, line)) {
    lines.push_back(line);
  }
  infile.close();

  // Trier les lignes par ordre alphabétique
  std::sort(lines.begin(), lines.end());

  // Écrire dans un fichier de sortie
  std::ofstream outfile(output);
  if (!outfile) {
    std::cerr << "Erreur : impossible d'ouvrir le fichier " << output << std::endl;
    exit(EXIT_FAILURE);
  }

  for (const auto& l : lines) {
    outfile << l << "\n";
  }
  outfile.close();
}

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
  if (argc < 6) {
    std::cerr << "Usage:\n"
	      << "  "
	      << argv[0]
	      << " <devid> <devpassword> <ssid> <sspassword> <répertoire>\n\n";
    return 1;
  }

  namespace fs = std::filesystem;

  std::string devid = argv[1];
  std::string devpassword = argv[2];
  std::string ssid = argv[3];
  std::string sspassword = argv[4];

  std::string dir = argv[5];

  std::ofstream gamelist("gamelist.dat", std::ios::trunc); // ouvre le fichier en mode ajout
  if (!gamelist) {
    std::cerr << "Impossible d'ouvrir gamelist.dat pour écriture" << std::endl;
    return 1;
  }

  for (const auto& entry : fs::directory_iterator(dir)) {
    if (entry.is_regular_file() && (entry.path().extension() == ".gba" || entry.path().extension() == ".zip")){
      //std::cout << "Fichier trouvé : " << entry.path().filename() << std::endl;

    
      std::string file = dir+std::string("/")+entry.path().filename().string();
      std::string crchex = crc32_of_file(file);
      std::string md5hex = md5_of_file(file);

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
      curl_easy_cleanup(curl);

      if(http_code == 200){
	tinyxml2::XMLDocument doc;
	doc.Parse(response.c_str());
	tinyxml2::XMLElement* data = doc.RootElement();
	tinyxml2::XMLElement* jeu  = data->FirstChildElement("jeu");
	std::string jeuId = jeu->Attribute("id");

	gamelist << jeuId << " " << entry.path().filename().string() << std::endl;
      }else
	gamelist << "-1" << " " << entry.path().filename().string() << std::endl;
      
    }
    
  }

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
      fs::rename(dir+"/"+nomFichier, dir+"/"+std::to_string(numero)+"/"+nomFichier); 
    }/*else{
      std::cout << "Pas de doublon" << std::endl;
      }*/
  }
  
    
  return 0;
}
