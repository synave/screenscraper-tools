#include "Temps.h"
#include <chrono>
#include <iomanip>

// Constructeur par défaut
Temps::Temps()
    : heures(0), minutes(0), secondes(0), millisecondes(0) {}

// Constructeur
Temps::Temps(long long h, long long m, long long s, long long ms)
    : heures(h), minutes(m), secondes(s), millisecondes(ms) {}

// Constructeur à partir d'un temps en millisecondes
Temps::Temps(long long totalMs) {
    heures        = totalMs / 3600000LL;
    totalMs      %= 3600000LL;

    minutes       = totalMs / 60000LL;
    totalMs      %= 60000LL;

    secondes      = totalMs / 1000LL;
    totalMs      %= 1000LL;

    millisecondes = totalMs;
}

// Méthode statique : récupère l'heure système et retourne un Temps
Temps Temps::clic() {
    using namespace std::chrono;

    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count();

    long long milli  = ms % 1000;
    long long sec    = (ms / 1000) % 60;
    long long min    = (ms / 60000) % 60;
    long long heure  = (ms / 3600000) % 24;

    return Temps(heure, min, sec, milli);
}


// Différence entre deux Temps
Temps Temps::operator-(const Temps& other) const {
    long long ms1 = heures * 3600000LL + minutes * 60000LL +
                    secondes * 1000LL + millisecondes;

    long long ms2 = other.heures * 3600000LL + other.minutes * 60000LL +
                    other.secondes * 1000LL + other.millisecondes;

    long long diff = ms1 - ms2;
    if (diff < 0) diff = -diff;

    long long h = diff / 3600000LL;
    diff %= 3600000LL;

    long long m = diff / 60000LL;
    diff %= 60000LL;

    long long s = diff / 1000LL;
    diff %= 1000LL;

    long long ms = diff;

    return Temps(h, m, s, ms);
}

// Multiplication d'un temps par un réel
Temps Temps::operator*(double facteur) const {
    // Convertit le temps courant en millisecondes
    long long totalMs =
        heures * 3600000LL +
        minutes * 60000LL +
        secondes * 1000LL +
        millisecondes;

    // Multiplie par un réel
    long long resultMs = static_cast<long long>(totalMs * facteur);

    // Retourne un nouvel objet Temps grâce au constructeur totalMs
    return Temps(resultMs);
}


// Affichage via <<
std::ostream& operator<<(std::ostream& os, const Temps& t) {
    os << std::setfill('0')
       << std::setw(2) << t.heures << ":"
       << std::setw(2) << t.minutes << ":"
       << std::setw(2) << t.secondes << "."
       << std::setw(3) << t.millisecondes;
    return os;
}
