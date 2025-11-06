#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstring>

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

  std::ofstream gamelist("gamelist.dat", std::ios::app); // ouvre le fichier en mode ajout
  if (!gamelist) {
    std::cerr << "Impossible d'ouvrir gamelist.dat pour écriture" << std::endl;
    return 1;
  }

  for (const auto& entry : fs::directory_iterator(dir)) {
    if (entry.is_regular_file() && entry.path().extension() == ".gba"){
      //std::cout << "Fichier trouvé : " << entry.path().filename() << std::endl;

    
      std::string file = entry.path().filename();
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

      tinyxml2::XMLDocument doc;
      doc.Parse(response.c_str());
      tinyxml2::XMLElement* data = doc.RootElement();
      tinyxml2::XMLElement* jeu  = data->FirstChildElement("jeu");
      std::string jeuId = jeu->Attribute("id");

      //std::cout << jeuId << " - " << file << std::endl;

      gamelist << jeuId << " - " << entry.path().filename() << std::endl;

      /*if (fs::exists(dir+"/"+jeu->Attribute("id")) && fs::is_directory(dir+"/"+jeu->Attribute("id")))
	fs::rename(file, dir+"/"+jeu->Attribute("id")+"/"+file);    
      else
      std::cout << "Le répertoire n'existe pas" << std::endl;*/
    }
  }
    
  return 0;
}
