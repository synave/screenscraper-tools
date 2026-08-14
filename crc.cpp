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


int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage:\n"
	      << "  " << argv[0] << " <FICHIER>\n\n";
    return EXIT_FAILURE;
  }

  std::string fichier = argv[1];
  
  Rom rom(fichier);
  std::string crchex = rom.getCRC();
  std::cout << crchex << std::endl;


  return 0;
}
