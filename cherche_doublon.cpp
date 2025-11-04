// call_screenscraper.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstring>

#include <curl/curl.h>
#include <openssl/md5.h>
#include <zlib.h>

// callback libcurl pour écrire la réponse dans une string
static size_t write_to_string(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t real_size = size * nmemb;
    std::string* mem = static_cast<std::string*>(userp);
    mem->append(static_cast<char*>(contents), real_size);
    return real_size;
}

// calculer MD5 d'un fichier -> renvoie hex string en minuscule
std::string md5_of_file(const std::string& path) {
    unsigned char buffer[8192];
    unsigned char md5_result[MD5_DIGEST_LENGTH];
    MD5_CTX ctx;
    MD5_Init(&ctx);

    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Impossible d'ouvrir le fichier pour MD5: " + path);
    }

    while (ifs) {
        ifs.read(reinterpret_cast<char*>(buffer), sizeof(buffer));
        std::streamsize s = ifs.gcount();
        if (s > 0) MD5_Update(&ctx, buffer, static_cast<size_t>(s));
    }

    MD5_Final(md5_result, &ctx);

    std::ostringstream oss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)md5_result[i];
    }
    return oss.str();
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
                  << "  " << argv[0] << " <devid> <devpassword> <ssid> <sspassword> <file_or_dash>\n\n"
                  << "file_or_dash: chemin d'un fichier pour calculer md5+crc OR '-' si vous fournirez md5+crc manuellement\n";
        return 1;
    }

    std::string devid = argv[1];
    std::string devpassword = argv[2];
    std::string ssid = argv[3];
    std::string sspassword = argv[4];
    std::string file_or_dash = argv[5];

    std::string md5hex, crchex;

    try {
        if (file_or_dash != "-") {
            md5hex = md5_of_file(file_or_dash);
            crchex = crc32_of_file(file_or_dash);
            std::cout << "MD5  = " << md5hex << "\n";
            std::cout << "CRC32= " << crchex << "\n";
        } else {
            // Si l'utilisateur passe '-', on lit md5 et crc depuis stdin (ou on pourrait les lire ailleurs)
            std::cout << "Entrez md5: "; std::cin >> md5hex;
            std::cout << "Entrez crc32: "; std::cin >> crchex;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Erreur calcul MD5/CRC: " << ex.what() << "\n";
        return 2;
    }

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
    std::cout << "URL construite:\n" << full_url << "\n\n";

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
    std::cout << "HTTP status: " << http_code << "\n";
    std::cout << "---- XML response start ----\n";
    std::cout << response << "\n";
    std::cout << "---- XML response end ----\n";

    // cleanup
    if (enc_devid) curl_free(enc_devid);
    if (enc_devpassword) curl_free(enc_devpassword);
    if (enc_ssid) curl_free(enc_ssid);
    if (enc_sspassword) curl_free(enc_sspassword);
    if (enc_md5) curl_free(enc_md5);
    if (enc_crc) curl_free(enc_crc);
    curl_easy_cleanup(curl);

    // Si tu veux parser le XML en C++, tu peux utiliser tinyxml2 (non inclus ici).
    // Exemple (commenté):
    // tinyxml2::XMLDocument doc;
    // doc.Parse(response.c_str());
    // tinyxml2::XMLElement* root = doc.RootElement();
    // if (root) { ... }

    return 0;
}
