#ifndef TEMPS_H
#define TEMPS_H

#include <iostream>

class Temps {
private:
  long long heures;
  long long minutes;
  long long secondes;
  long long millisecondes;

public:
  // Constructeur par défaut
  Temps();
  // Constructeur explicite (plus de constructeur par défaut)
  Temps(long long h, long long m, long long s, long long ms);
  // Constructeur qui prend un temps en millisecondes
  Temps(long long totalMs);

  // Méthode statique : retourne le temps système dans un objet Temps
  static Temps clic();

  // Différence entre deux temps
  Temps operator-(const Temps& other) const;
  // Multiplication d'un temps par un réel
  Temps operator*(double facteur) const;

  // Affichage via <<
  friend std::ostream& operator<<(std::ostream& os, const Temps& t);
};

#endif
