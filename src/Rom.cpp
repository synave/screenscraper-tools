#include "Rom.h"

Rom::Rom(const std::string& path): chemin(path){
  _scrape=NULL;
}

bool Rom::scrape(const ScreenScraper& ss){
  _scrape = ss.recherche_jeu_par_CRC(this->getCRC());
  if(_scrape != NULL) return true;  
  return false;
}
