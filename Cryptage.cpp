#include "Cryptage.h"

std::string Cryptage::md5(const std::string &path){
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

std::string Cryptage::crc(const std::string &path){
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
