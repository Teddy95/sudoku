#include "../header/displayTime.h"

/**
 * Funktion:        getPastTime
 *
 * Parameter:       time (double) -> Savegame-Struktur, in dem der aktuelle Spielfortschritt, die L�sung und die bislang ben�tigte Zeit enthalten ist
 *
 * R�ckgabewert:    Gibt die Struktur 'time' zur�ck
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

    // Werte zu Ganzzahlen casten und in die R�ckgabestruktur schreiben
    result.hours = (int) hours;
    result.minutes = (int) minutes;
    result.seconds = (int) seconds;

    return result;
}
