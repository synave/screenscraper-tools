#include "Temps.h"
#include <chrono>
#include <iomanip>

// Constructeur
Temps::Temps(long long h, long long m, long long s, long long ms)
    : heures(h), minutes(m), secondes(s), millisecondes(ms) {}


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


// Affichage via <<
std::ostream& operator<<(std::ostream& os, const Temps& t) {
    os << std::setfill('0')
       << std::setw(2) << t.heures << ":"
       << std::setw(2) << t.minutes << ":"
       << std::setw(2) << t.secondes << "."
       << std::setw(3) << t.millisecondes;
    return os;
}
