#include <iostream>

#include "Rom.h"


int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage:\n"
	      << "  " << argv[0] << " <FICHIER>\n\n";
    return EXIT_FAILURE;
  }

  std::string fichier = argv[1];
  
  Rom rom(fichier);
  std::string crchex = rom.getSHA1();
  std::cout << crchex << std::endl;


  return 0;
}
