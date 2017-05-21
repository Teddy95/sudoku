/*
 * SKRAM Sudoku
 *
 * Funktionsumfang:
 *
 * - Zuf�llig generiertes Sudoku nach Schwierigkeitsgrad
 * - Timer mit bislang ben�tigter Spielzeit nach jedem Zug und am Ende des Spiels
 * - Sudoku kann in einem Slot gespeichert werden (.skram-Datei)
 * - Sudoku kann aus einem Slot geladen und fortgesetzt werden (.skram-Datei)
 * - W�hrend dem Spielen kann man mit der Tipp-Funktion ein zuf�llig gew�hltes Feld automatisch f�llen lassen
 * - Nach jedem Zug wird das Sudoku im Hintergrund gepr�ft
 * - Sobald das Sudoku vollst�ndig und korrekt gel�st wurde, wird eine entsprechende Nachricht angezeigt
 *
 * Kompiliert mit: MinGW
 * Getestet auf OS: Windows 10
 */

#include "header/sudoku.h"

// Hauptprogramm
int main() {
    init();
    int input = VIEW_HOME;

    // Steuerung der Views / Bildschirme
    do {
        switch (input) {
            case VIEW_HOME:
                input = showView(VIEW_HOME);
                break;

            case VIEW_GAME_NEW:
                input = showView(VIEW_GAME_NEW);
                break;

            case VIEW_GAME_LOAD:
                input = showView(VIEW_GAME_LOAD);
                break;

            case VIEW_CREDITS:
                input = showView(VIEW_CREDITS);
                break;

            case VIEW_GAME_READ:
                input = showView(VIEW_GAME_READ);
                break;

            default:
                die();
        }
    } while (input != VIEW_EXIT);

    return 0;
}
