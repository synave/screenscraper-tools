#ifndef CRYPTAGE_H
#define CRYPTAGE_H

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>

#include <openssl/evp.h>
#include <zlib.h>

class Cryptage {
 public:
  static std::string md5(const std::string &path);
  static std::string crc(const std::string &path);
};

#endif
