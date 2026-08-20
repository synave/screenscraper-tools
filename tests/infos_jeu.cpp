#include <iostream>

#include "ScreenScraper.h"
#include "Rom.h"


int main(int argc, char* argv[]) {

  if (argc != 2) {
    std::cerr << "Usage:\n"
	      << "  " << argv[0] << " <fichier ROM>\n\n";
    return EXIT_FAILURE;
  }
  ScreenScraper ss(std::getenv("SSDEVID"), std::getenv("SSDEVPASSWD"), std::getenv("SSDEVPASSWDDEBUG"), std::getenv("SSID"), std::getenv("SSPASSWD"));

  Rom r(argv[1]);

  bool trouve = r.scrape(ss);

  if(trouve==false)
    std::cout << "jeu non trouvé" << std::endl;
  else
    {
      std::cout << r.jeuScrape()->getNumeroDeJeu() << std::endl;
      std::cout << *(r.jeuScrape()->getSynopsis("pt")) << std::endl;
      r.jeuScrape()->telecharge_miniature("mixrbv1","wor","test.png");
    }

  return 0;
}
