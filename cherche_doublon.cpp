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
#include "JeuScrape.h"
#include "ScreenScraper.h"
#include "Temps.h"



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
  if (argc < 2) {
    std::cerr << "Usage:\n"
	      << "  " << argv[0] << " <répertoire>\n\n";
    return EXIT_FAILURE;
  }

  std::string dir = argv[1];
  std::string fichier_gamelist = dir+"/gamelist.dat";
  Temps debut, inter, diff, eta;

  if (std::filesystem::exists(fichier_gamelist)) {
    std::cerr << "Le fichier gamelist.dat existe déjà" << std::endl;
    return EXIT_FAILURE;
  }

  std::ofstream gamelist(fichier_gamelist, std::ios::trunc); // ouvre le fichier en mode ajout
  if (!gamelist) {
    std::cerr << "Impossible d'ouvrir gamelist.dat pour écriture" << std::endl;
    return EXIT_FAILURE;
  }

  // Récupération du nombre de fichiers à traiter
  int nb_fichiers = 0;
  for (const auto& entry : std::filesystem::directory_iterator(dir)) {
    if (entry.is_regular_file() && (entry.path().extension() == ".gba" || entry.path().extension() == ".zip")){
      nb_fichiers++;
    }
  }

  debut = Temps::clic();
  
  double nb_fichiers_parcourus = 0;
  for (const auto& entry : std::filesystem::directory_iterator(dir)) {
    if (entry.is_regular_file() && entry.path().extension() == ".gba"){
      std::cout << std::string(40, '\b') << std::string(40, ' ') << std::string(40, '\b');
      std::cout << std::fixed
              << std::setprecision(3)
              << std::setw(6)
              << std::setfill('0')
	      << ((nb_fichiers_parcourus/nb_fichiers)*100) << " % " << std::flush;
      if(nb_fichiers_parcourus > 0){
	inter = Temps::clic();
	diff=inter-debut;
	eta=(diff*(1.0/(nb_fichiers_parcourus/nb_fichiers)) - diff);
	std::cout << " - ETA : " << eta << std::flush;
      }
      nb_fichiers_parcourus++;

    
      std::string chemin_rom = dir+std::string("/")+entry.path().filename().string();
      Rom rom(chemin_rom);
      std::string crchex = rom.getCRC();

      JeuScrape *jeu = ScreenScraper::recherche_jeu_par_CRC(crchex);
      jeu->setChemin(chemin_rom);

      if(jeu!=NULL){
	gamelist << jeu->getNumeroDeJeu() << " " << entry.path().filename().string() << std::endl;
      }else
	gamelist << "-1" << " " << entry.path().filename().string() << std::endl;
           
    }
    
  }

  std::cout << std::string(20, '\b') << std::string(20, ' ') << std::string(20, '\b') << "100%" << std::endl;


  /*
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
      std::filesystem::rename(dir+"/"+nomFichier, dir+"/"+std::to_string(numero)+"/"+nomFichier); 
    }
  }

  std::cout << std::endl;
  */
    
  return 0;
}
