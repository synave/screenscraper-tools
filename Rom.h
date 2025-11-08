#ifndef ROM_H
#define ROM_H

#include "Cryptage.h"

class Rom {
 private :
  const std::string chemin;

 public:
  Rom(const std::string& path);

  const std::string getChemin(){return this->chemin;}
  std::string getMD5(){return Cryptage::md5(chemin);}
  std::string getCRC(){return Cryptage::crc(chemin);}
};

#endif
