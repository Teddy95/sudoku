#ifndef DISPLAYTIME_H_INCLUDED
#define DISPLAYTIME_H_INCLUDED

#include <math.h>

// Struktur für die Zeitanzeige
struct time {
    int hours;
    int minutes;
    int seconds;
};

// Zeitfunktionen
struct time getPastTime(double time);

#endif // DISPLAYTIME_H_INCLUDED
