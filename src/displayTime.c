#include "../header/displayTime.h"

/**
 * Funktion:        getPastTime
 *
 * Parameter:       time (double) -> Savegame-Struktur, in dem der aktuelle Spielfortschritt, die Lösung und die bislang benötigte Zeit enthalten ist
 *
 * Rückgabewert:    Gibt die Struktur 'time' zurück
 *
 * Beschreibung:    Wandelt Sekunden in Stunden, Minuten und Sekunden um.
 */
struct time getPastTime(double time) {
    struct time result;
    double hours, minutes, seconds;

    // Berechnung von Stunden, Minuten und Sekunden
    hours = floor(time / 3600);
    minutes = floor((time - (hours * 3600)) / 60);
    seconds = floor((int) time % 60);

    // Werte zu Ganzzahlen casten und in die Rückgabestruktur schreiben
    result.hours = (int) hours;
    result.minutes = (int) minutes;
    result.seconds = (int) seconds;

    return result;
}
