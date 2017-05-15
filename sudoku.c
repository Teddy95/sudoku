/*
 * SKRAM Sudoku
 *
 * Funktionsumfang:
 *
 * - Zufällig generiertes Sudoku nach Schwierigkeitsgrad
 * - Timer mit bislang benötigter Spielzeit nach jedem Zug und am Ende des Spiels
 * - Sudoku kann in einem Slot gespeichert werden (.skram-Datei)
 * - Sudoku kann aus einem Slot geladen und fortgesetzt werden (.skram-Datei)
 * - Während dem Spielen kann man mit der Tipp-Funktion ein zufällig gewähltes Feld automatisch füllen lassen
 * - Nach jedem Zug wird das Sudoku im Hintergrund geprüft
 * - Sobald das Sudoku vollständig und korrekt gelöst wurde, wird eine entsprechende Nachricht angezeigt
 *
 * Kompiliert mit: MinGW
 * Getestet auf OS: Windows 10
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>

// Definition von horizontaler sowie vertikaler Sudoku-Größe
#define SIZE 9

// Definition von Konstaten für den Schwierigkeitsgrad
#define EASY 1
#define MEDIUM 2
#define DIFFICULT 3

// Parser Konstanten
#define PARSER_VALID 0 //! Parsing erfolgreich
#define PARSER_FILE_INACCESSIBLE 1 //! Datei nicht lesbar
#define PARSER_SUDOKU_NUMBERS_INVALID 2 //! Sudoku enthielt falsch Eingaben.

// Definition von Konstaten für die Views
#define VIEW_HOME 0
#define VIEW_GAME_NEW 1
#define VIEW_GAME_LOAD 2
#define VIEW_GAME_PLAY 3
#define VIEW_GAME_SAVE 4
#define VIEW_CREDITS 5
#define VIEW_EXIT 6
#define VIEW_GAME_READ 7

// Definition von Konstante für maximale Zeichenkettenlänge
#define STRLEN 255

// Definition von Strukturen
// Sudoku-Gitter
struct sudoku {
    int value[9][9];     // Werte im Sudoku
    int generated[9][9]; // 1 = Wert kann nicht geändert werden, 0 = Wert kann geändert werden
};

// Koordinaten eines Feldes im Sudoku-Gitter
struct field {
    int row;
    int column;
};

// Struktur für einen gespeicherten Spielstand
struct savegame {
    struct sudoku sudokuGridSolved;
    struct sudoku sudokuGrid;
    double pastTime;
};

// Struktur für die Zeitanzeige
struct time {
    int hours;
    int minutes;
    int seconds;
};

/**
 * Ließt, intepretiert und überprüft eine Datei in ein Sudoku
 * Diese Funktion in den Views nutzen!
 *
 * @param detaiPfad
 * @param errorCode
 *
 * @return Das geparste und überprüfte Sudoku
 */
struct sudoku getSudokuFromFile(char[1024], int*);

/**
 * Ließt und parsed eine Datei in ein Sudoku.
 *
 * @param dateiHandle
 * @param errorCode
 *
 * @return geparstesSudoku
 */
struct sudoku parseToSudoku(FILE*, int*);

/**
 * Verifizieret ob dateiPfad eine lesbare Datei identifiziert.
 *
 * @param dateiPfad
 *
 * @return 1 wenn der Pfad
 */
int verifyFilePath(char[1024]);

/**
 * Überprüft ob das Sudoku nur Zahlen von (einschließlich)
 * 1 bis 9 enthält. Überprüft des Weiteren ob sudoku.generated nur 1 enthält.
 *
 * @param sudoku Sudoku to check
 *
 * @return Eine PARSER_* Konstante die den Fehlercode angibt.
 */
int checkParsedSudoku(struct sudoku);

// Funktionsprototypen (Funktionsbeschreibungen jeweils an den Funktionen)
// Funktionen zum Initialisieren und Terminieren
void init();
void die();
// Sudoku Generierungsfunktionen
struct sudoku generateFullSudoku();
struct sudoku makeSodukoPlayable(struct sudoku sudokuGrid, int difficulty);
// Funktionen zum Mischen von Werten und Zeilen/Spalten im Sudoku
void shiftRight(int array[], int size, int steps);
void shuffle(int array[], int size);
struct sudoku shuffleRows(struct sudoku sudokuGrid);
struct sudoku shuffleColumns(struct sudoku sudokuGrid);
// Funktionen zum vertauschen von Werten und Zeilen/Spalten im Sudoku
void swap(int *x, int *y);
struct sudoku swapRows(struct sudoku sudokuGrid, int quadrant, int x, int y);
struct sudoku swapColumns(struct sudoku sudokuGrid, int quadrant, int x, int y);
// Funktionen zum Prüfen des Sudokus
int checkSudoku(struct sudoku sudokuGrid);
int checkValue(struct sudoku sudokuGrid, struct field field);
int checkRow(struct sudoku sudokuGrid, struct field field);
int checkColumn(struct sudoku sudokuGrid, struct field field);
int checkGroup(struct sudoku sudokuGrid, struct field field);
// Funktionen zum prüfen und Konvertieren von Eingaben
int charToInt(char charakter);
int checkAndConvertInputToInt(char inputString[]);
int checkAndConvertInputChar(char inputString[]);
void readLine(char inputString[]);
// Feldfunktionen
struct sudoku alterValue(struct sudoku sudokuGrid, struct field field, int newValue, int *error);
struct sudoku fillRandomField(struct sudoku sudokuGridSolved, struct sudoku sudokuGrid);
int countEmptyFields(struct sudoku sudokuGrid);
// Savegame-Funktionen
int checkSavegameSlot(int slot);
struct savegame readSavegame(int slot, int *error);
int writeSavegame(struct savegame sudoku, int slot);
void deleteSavegame(int slot);
// Zeitfunktionen
struct time getPastTime(double time);
// Ausgabe-/Viewfunktionen
int showView(int view);
int viewHome();
int viewGameNew();
int viewGameLoad();
int viewGameDelete();
int viewGamePlay(struct savegame sudoku);
int viewGameSave(struct savegame sudoku);
int viewGameReadFromSudokuFile();
int viewCredits();
void printGrid(struct sudoku sudokuGrid);

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

            default:
                die();
        }
    } while (input != VIEW_EXIT);

    return 0;
}

/**
 * Funktion:        init
 *
 * Beschreibung:    Initialisiert das Programm.
 */
void init() {
    srand((unsigned)time(NULL));

    return;
}

/**
 * Funktion:        die
 *
 * Beschreibung:    Terminiert das Programm bei einem unerwarteten Fehler.
 */
void die() {
    system("cls");
    printf("Es ist ein unerwarteter Fehler aufgetreten! Bitte starten Sie das Programm neu!\n");
    exit(0);
}

/*************************************
        START PARSING
 *************************************/


struct sudoku getSudokuFromFile(char path[1024], int *error) {
    struct sudoku sudoku;
    FILE *fileHandle;
    if (verifyFilePath(path) == 1) {
        *error = PARSER_VALID;
        fileHandle = fopen(path, "r");
        sudoku = parseToSudoku(fileHandle, error);
    } else {
        *error = PARSER_FILE_INACCESSIBLE;
    }

    return sudoku;
}

struct sudoku parseToSudoku(FILE *fileHandle, int *error) {
    struct sudoku sudoku;
    int i;
    for (i = 0; i < 9; i++) {
        // Lese eine Zeile der Datei aus, und fülle die entsprechende Zeile
        // im Sudoku
        fscanf(
            fileHandle,
            "%i,%i,%i,%i,%i,%i,%i,%i,%i",
            &sudoku.value[i][0],
            &sudoku.value[i][1],
            &sudoku.value[i][2],
            &sudoku.value[i][3],
            &sudoku.value[i][4],
            &sudoku.value[i][5],
            &sudoku.value[i][6],
            &sudoku.value[i][7],
            &sudoku.value[i][8]
        );
    };

    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            sudoku.generated[i][j] = 1;
        }
    }

    *error = checkParsedSudoku(sudoku);

    return sudoku;
}

int checkParsedSudoku(struct sudoku sudoku) {
    int i, j, value = 0, generated = 0;
    int isError = PARSER_VALID;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            value = sudoku.value[i][j];
            generated = sudoku.generated[i][j];
            if (value < 1 || value > 9 || generated != 1) {
                isError = PARSER_SUDOKU_NUMBERS_INVALID;
            }
        }
    }
    return isError;
}

int verifyFilePath(char path[1024]) {
    int success;
    FILE *fileHandle;
    fileHandle = fopen(path, "r");

    if (fileHandle == NULL) {
        success = 0;
    } else {
        success = 1;
    }

    return success;
}
/*************************************
        ENDE PARSING
 *************************************/

/**
 * Funktion:        generateFullSudoku
 *
 * Rückgabewert:    Gibt die Struktur 'sudoku' zurück
 *
 * Beschreibung:    Generiert ein vollständiges Sudoku in einem zeidimensionalem numerischem Array innerhalb der Struktur 'sudoku'.
 */
struct sudoku generateFullSudoku() {
    int i, j, k;
    struct sudoku sudokuGrid;
    int row[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    // Zeilen-Muster (1, 2, 3, 4, ...) mischen
    shuffle(row, SIZE);

    // Sudoku-Muster (Grundmuster) erstellen -> Selbes Muster in jeder Zeile um wenige Spalten nach rechts verschoben
    for (i = 0; i < 3; i++) {
        for (j = (9 / 3 * i); j < ((9 / 3 * i) + 3); j++) {
            for (k = 0; k < 9; k++) {
                sudokuGrid.value[j][k] = row[k];
                sudokuGrid.generated[j][k] = 1;
            }

            shiftRight(row, SIZE, 3);
        }

        shiftRight(row, SIZE, 2);
    }

    // Zeilen und Spalten innerhalb von Dreiergruppen im Sudoku-Muster mischen
    for (i = 0; i < SIZE; i++) {
        sudokuGrid = shuffleRows(sudokuGrid);
        sudokuGrid = shuffleColumns(sudokuGrid);
    }

    return sudokuGrid;

    /*
    int i, j;
    struct sudoku sudokuGrid;
    struct field field;

    for (i = 0; i < SIZE; i++) {
    	field.row = i;

    	for (j = 0; j < SIZE; j++) {
    		field.column = j;
    		sudokuGrid.generated[i][j] = 1;

    		do {
    			sudokuGrid.value[i][j] = rand() % 9 + 1;
    		} while (!checkValue(sudokuGrid, field));
    	}
    }

    return sudokuGrid;
    */
}

/**
 * Funktion:        makeSodukoPlayable
 *
 * Parameter:       sudokuGrid (struct sudoku) -> Nimmt den Rückgabewert von Funktion 'generateFullSudoku' entgegen
 * Parameter:       difficulty (int)           -> Gibt einen Schwierigkeitsgrad an (1 = leicht, 2 = mittel und 3 = schwer)
 *
 * Rückgabewert:    Gibt die Struktur 'sudoku' zurück
 *
 * Beschreibung:    Erstellt aus dem vollständigen Sudoku eine spielbare Variante mit leeren Feldern.
 */
struct sudoku makeSodukoPlayable(struct sudoku sudokuGrid, int difficulty) {
    int emptyFields, i, row, column;

    // Leere Felder (Felder zum ausfüllen) nach angegebenen Schwierigkeitsgrad festlegen
    switch (difficulty) {
        case 1:
            emptyFields = 25;
            break;

        case 3:
            emptyFields = 45;
            break;

        case 2:
        default:
            emptyFields = 35;
            break;
    }

    // Per Zufall ausgewählte Felder im Sudoku-Muster durch 0 ersetzen (0 = leer)
    for (i = emptyFields; i > 0; i--) {
        do {
            row = rand() % SIZE;
            column = rand() % SIZE;
        } while (sudokuGrid.value[row][column] == 0);

        sudokuGrid.value[row][column] = 0;
        sudokuGrid.generated[row][column] = 0;
    }

    return sudokuGrid;
}

/**
 * Funktion:        shuffle
 *
 * Parameter:       array (int[]) -> Array, in welchem die Werte verschoben werden sollen
 * Parameter:       size (int)    -> Anzahl der Elemente im Array
 * Parameter:       steps (int)   -> Gibt an, um wie viele Stellen die Elemente nach rechts verschoben werden sollen
 *
 * Beschreibung:    Verschiebt alle Elemente im Array um die angegebene Anzahl 'steps' nach rechts.
 */
void shiftRight(int array[], int size, int steps) {
    int i, step;

    // Die beiden letzten Werte im Array sukzessiv nach vorne austauschen
    for (step = 0; step < steps; step++) {
        for (i = size - 1; i > 0; i--) {
            swap(&array[i], &array[i - 1]);
        }
    }

    return;
}

/**
 * Funktion:        shuffle
 *
 * Parameter:       array (int[]) -> Array, in welchem die Werte zufällig gemischt werden
 * Parameter:       size (int)    -> Anzahl der Elemente im Array
 *
 * Beschreibung:    Mischt zufällig die Werte in einem Array.
 */
void shuffle(int array[], int size) {
    int i, x, y;

    // Zwei per Zufall ausgewählte Werte im Array austauschen
    for (i = 0; i < size; i++) {
        x = rand() % size;
        y = rand() % size;

        swap(&array[x], &array[y]);
    }

    return;
}

/**
 * Funktion:        shuffleRows
 *
 * Parameter:       sudokuGrid (struct sudoku) -> Sudoku, welches manipuliert werden soll
 * Parameter:       quadrant (int)             -> Dreiergruppe in der getauscht wird (0, 1 oder 2)
 * Parameter:       x (int)                    -> Zeile 1 (0, 1 oder 2)
 * Parameter:       y (int)                    -> Zeile 2 (0, 1 oder 2)
 *
 * Rückgabewert:    Gibt die Struktur 'sudoku' zurück
 *
 * Beschreibung:    Mischt Zeilen im Sudoku durcheinander.
 */
struct sudoku shuffleRows(struct sudoku sudokuGrid) {
    int quadrant, x, y, i;

    // Dreiergruppe und Zeilen darin per Zufall auswählen
    for (i = 0; i < SIZE; i++) {
        quadrant = rand() % SIZE / 3;
        x = rand() % SIZE / 3;
        y = rand() % SIZE / 3;

        // Zeilen in Dreiergruppe austauschen
        sudokuGrid = swapRows(sudokuGrid, quadrant, x, y);
    }

    return sudokuGrid;
}

/**
 * Funktion:        shuffleColumns
 *
 * Parameter:       sudokuGrid (struct sudoku) -> Sudoku, welches manipuliert werden soll
 * Parameter:       quadrant (int)             -> Dreiergruppe in der getauscht wird (0, 1 oder 2)
 * Parameter:       x (int)                    -> Zeile 1 (0, 1 oder 2)
 * Parameter:       y (int)                    -> Zeile 2 (0, 1 oder 2)
 *
 * Rückgabewert:    Gibt die Struktur 'sudoku' zurück
 *
 * Beschreibung:    Mischt Spalten im Sudoku durcheinander.
 */
struct sudoku shuffleColumns(struct sudoku sudokuGrid) {
    int quadrant, x, y, i;

    // Dreiergruppe und Spalten darin per Zufall auswählen
    for (i = 0; i < SIZE; i++) {
        quadrant = rand() % SIZE / 3;
        x = rand() % SIZE / 3;
        y = rand() % SIZE / 3;

        // Spalten in Dreiergruppe austauschen
        sudokuGrid = swapColumns(sudokuGrid, quadrant, x, y);
    }

    return sudokuGrid;
}

/**
 * Funktion:        swap
 *
 * Parameter:       *x (int) -> Wert 1
 * Parameter:       *y (int) -> Wert 2
 *
 * Beschreibung:    Vertauscht die Werte von zwei Speicheradressen bzw. Variablen.
 */
void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;

    return;
}

/**
 * Funktion:        swapRows
 *
 * Parameter:       sudokuGrid (struct sudoku) -> Sudoku, welches manipuliert werden soll
 * Parameter:       quadrant (int)             -> Dreiergruppe in der getauscht wird (0, 1 oder 2)
 * Parameter:       x (int)                    -> Zeile 1 (0, 1 oder 2)
 * Parameter:       y (int)                    -> Zeile 2 (0, 1 oder 2)
 *
 * Rückgabewert:    Gibt die Struktur 'sudoku' zurück
 *
 * Beschreibung:    Tauscht zwei Zeilen im Sudoku miteinander aus.
 */
struct sudoku swapRows(struct sudoku sudokuGrid, int quadrant, int x, int y) {
    int i;
    int indexX;
    int indexY;

    // Prüfen ob übergebene Parameter legal sind
    if ((quadrant == 0 || quadrant == 1 || quadrant == 2) && (x == 0 || x == 1 || x == 2) && (y == 0 || y == 1 || y == 2)) {
        // Zeilen in Dreiergruppe errechnen
        indexX = (9 / 3 * quadrant) + x;
        indexY = (9 / 3 * quadrant) + y;

        for (i = 0; i < SIZE; i++) {
            swap(&sudokuGrid.value[indexX][i], &sudokuGrid.value[indexY][i]);
            swap(&sudokuGrid.generated[indexX][i], &sudokuGrid.generated[indexY][i]);
        }
    }

    return sudokuGrid;
}

/**
 * Funktion:        swapColumns
 *
 * Parameter:       sudokuGrid (struct sudoku) -> Sudoku, welches manipuliert werden soll
 * Parameter:       quadrant (int)             -> Dreiergruppe in der getauscht wird  (0, 1 oder 2)
 * Parameter:       x (int)                    -> Spalte 1 (0, 1 oder 2)
 * Parameter:       y (int)                    -> Spalte 2 (0, 1 oder 2)
 *
 * Rückgabewert:    Gibt die Struktur 'sudoku' zurück
 *
 * Beschreibung:    Tauscht zwei Spalten im Sudoku miteinander aus.
 */
struct sudoku swapColumns(struct sudoku sudokuGrid, int quadrant, int x, int y) {
    int i;
    int indexX;
    int indexY;

    // Prüfen ob übergebene Parameter legal sind
    if ((quadrant == 0 || quadrant == 1 || quadrant == 2) && (x == 0 || x == 1 || x == 2) && (y == 0 || y == 1 || y == 2)) {
        // Spalten in Dreiergruppe errechnen
        indexX = (9 / 3 * quadrant) + x;
        indexY = (9 / 3 * quadrant) + y;

        for (i = 0; i < SIZE; i++) {
            swap(&sudokuGrid.value[i][indexX], &sudokuGrid.value[i][indexY]);
            swap(&sudokuGrid.generated[i][indexX], &sudokuGrid.generated[i][indexY]);
        }
    }

    return sudokuGrid;
}

/**
 * Funktion:        checkSudoku
 *
 * Parameter:       sudokuGrid (struct sudoku) -> Sudoku-Muster, welches überprüft werden soll
 *
 * Rückgabewert:    Gibt eine 0 für falsch zurück und eine 1 für korrekt
 *
 * Beschreibung:    Prüft ob das Sudoku richtig und vollsträndig gelöst wurde.
 */
int checkSudoku(struct sudoku sudokuGrid) {
    int i, j;
    struct field field;

    // Wenn es noch leere Felder gibt, kann das Sudoku noch nicht gelöst sein
    if (countEmptyFields(sudokuGrid) > 0) {
        return 0;
    }

    // Jedes Feld einzeln prüfen
    for (i = 0; i < SIZE; i++) {
        field.row = i;

        for (j = 0; j < SIZE; j++) {
            field.column = j;

            if (!checkValue(sudokuGrid, field)) {
                return 0;
            }
        }
    }

    return 1;
}

/**
 * Funktion:        checkValue
 *
 * Parameter:       sudokuGrid (struct sudoku) -> Sudoku-Muster, in dem die Werte stehen
 * Parameter:       field (struct field)       -> Koordinaten vom Feld, welches überprüft werden soll
 *
 * Rückgabewert:    Gibt eine 0 für falsch zurück und eine 1 für korrekt (1 wird auch zurückgegeben, wenn der Wert aktuell korrekt ist, in der Gesamtlösung jedoch nicht)
 *
 * Beschreibung:    Prüft ob ein Wert im Sudoku richtig gesetzt wurde.
 */
int checkValue(struct sudoku sudokuGrid, struct field field) {
    // Feld in Zeile, Reihe und Gruppe (3x3) auf Einmaligkeit prüfen
    if (checkRow(sudokuGrid, field) && checkColumn(sudokuGrid, field) && checkGroup(sudokuGrid, field)) {
        return 1;
    }

    return 0;
}

/**
 * Funktion:        checkRow
 *
 * Parameter:       sudokuGrid (struct sudoku) -> Sudoku-Muster, in dem die Werte stehen
 * Parameter:       field (struct field)       -> Koordinaten vom Feld, welches überprüft werden soll
 *
 * Rückgabewert:    Gibt eine 0 für falsch zurück und eine 1 für korrekt (1 wird auch zurückgegeben, wenn der Wert aktuell korrekt ist, in der Gesamtlösung jedoch nicht)
 *
 * Beschreibung:    Prüft ob ein Wert in einer Zeile richtig gesetzt wurde.
 */
int checkRow(struct sudoku sudokuGrid, struct field field) {
    int i, counter;
    counter = 0;

    // Feld in Zeile auf Einmaligkeit prüfen
    for (i = 0; i < SIZE; i++) {
        if (sudokuGrid.value[field.row][i] == sudokuGrid.value[field.row][field.column]) {
            counter++;
        }
    }

    if (counter == 1) {
        // Feld ist einmalig
        return 1;
    }

    return 0;
}

/**
 * Funktion:        checkColumn
 *
 * Parameter:       sudokuGrid (struct sudoku) -> Sudoku-Muster, in dem die Werte stehen
 * Parameter:       field (struct field)       -> Koordinaten vom Feld, welches überprüft werden soll
 *
 * Rückgabewert:    Gibt eine 0 für falsch zurück und eine 1 für korrekt (1 wird auch zurückgegeben, wenn der Wert aktuell korrekt ist, in der Gesamtlösung jedoch nicht)
 *
 * Beschreibung:    Prüft ob ein Wert in einer Spalte richtig gesetzt wurde.
 */
int checkColumn(struct sudoku sudokuGrid, struct field field) {
    int i, counter;
    counter = 0;

    // Feld in Spalte auf Einmaligkeit prüfen
    for (i = 0; i < SIZE; i++) {
        if (sudokuGrid.value[i][field.column] == sudokuGrid.value[field.row][field.column]) {
            counter++;
        }
    }

    if (counter == 1) {
        // Feld ist einmalig
        return 1;
    }

    return 0;
}

/**
 * Funktion:        checkGroup
 *
 * Parameter:       sudokuGrid (struct sudoku) -> Sudoku-Muster, in dem die Werte stehen
 * Parameter:       field (struct field)       -> Koordinaten vom Feld, welches überprüft werden soll
 *
 * Rückgabewert:    Gibt eine 0 für falsch zurück und eine 1 für korrekt (1 wird auch zurückgegeben, wenn der Wert aktuell korrekt ist, in der Gesamtlösung jedoch nicht)
 *
 * Beschreibung:    Prüft ob ein Wert in einem 9x9 Block richtig gesetzt wurde.
 */
int checkGroup(struct sudoku sudokuGrid, struct field field) {
    int row, column, i, j, counter;
    counter = 0;

    //Quadranten berechnen
    row = (field.row - (field.row % 3)) / 3;
    column = (field.column - (field.column % 3)) / 3;

    // Feld in Gruppe (3x3) auf Einmaligkeit prüfen
    for (i = 0; i < SIZE / 3; i++) {
        for (j = 0; j < SIZE / 3; j++) {
            if (sudokuGrid.value[row * 3 + i][column * 3 + j] == sudokuGrid.value[field.row][field.column]) {
                counter++;
            }
        }
    }

    if (counter == 1) {
        // Feld ist einmalig
        return 1;
    }

    return 0;
}

/**
 * Funktion:        charToInt
 *
 * Parameter:       charakter (struct sudoku) -> Buchstabe
 *
 * Rückgabewert:    Gibt einen Wert zurück, welcher als 'column'-Wert in der Struktur 'field' verwendet werden kann. Gibt im Fehlerfall -1 zurück
 *
 * Beschreibung:    Prüft ob ein Wert im Sudoku richtig gesetzt wurde.
 */
int charToInt(char charakter) {
    int result = 0;

    if (charakter >= 65 && charakter <= 73) {
        // Großbuchstabe A - I -> ASCII Code von A subtrahieren
        result = charakter - 65;
    } else if (charakter >= 97 && charakter <= 105) {
        // Kleinbuchstabe a - i -> ASCII Code von a subtrahieren
        result = charakter - 97;
    } else {
        result--;
    }

    return result;
}

/**
 * Funktion:        checkAndConvertInputToInt
 *
 * Parameter:       inputString (char[]) -> Eingegebene Zeichenkette
 *
 * Rückgabewert:    Gibt einen für die View-Steuerung nötigen Int-Wert zurück
 *
 * Beschreibung:    Prüft und konvertiert eine eingegebene Zeichenkette.
 */
int checkAndConvertInputToInt(char inputString[]) {
    int i, strLen, charakter, result;
    result = 0;
    strLen = strlen(inputString);

    for (i = 0; i < strLen; i++) {
        // Sobald ein nicht numerischen Zeichen gefunden wurde abbrechen
        if (!isdigit(inputString[i])) {
            return 0;
        }

        // Wert mit 10 multiplizieren und neue Zahl addieren -> z.B. 125 = (((((0*10)+1)*10)+2)*10)+5
        result *= 10;
        result += inputString[i] - '0';
    }

    return result;
}

/**
 * Funktion:        checkAndConvertInputChar
 *
 * Parameter:       inputString (char[]) -> Eingegebene Zeichenkette
 *
 * Rückgabewert:    Gibt einen Wert zurück, welcher als 'column'-Wert in der Struktur 'field' verwendet werden kann. Gibt im Fehlerfall -1 zurück
 *
 * Beschreibung:    Prüft und konvertiert eine eingegebene Zeichenkette.
 */
int checkAndConvertInputChar(char inputString[]) {
    int i, strLen;
    strLen = strlen(inputString);

    // Zeichenkette darf nur ein Zeichen enthalten
    if (strLen > 1) {
        return -1;
    }

    return charToInt(inputString[0]);
}

/**
 * Funktion:        readLine
 *
 * Parameter:       inputString (char[]) -> Zeichenkette in dem die Eingabe geschrieben wird
 *
 * Beschreibung:    Liest eine Zeichenkette ein.
 */
void readLine(char inputString[]) {
    int strLen;

    fflush(stdin);
    fgets(inputString, STRLEN, stdin);
    strLen = strlen(inputString);

    if (inputString[strLen - 1] == '\n') {
        inputString[strLen - 1] = '\0';
    }

    return;
}

/**
 * Funktion:        alterValue
 *
 * Parameter:       sudokuGrid (struct sudoku) -> Sudoku-Muster, in dem die Werte stehen
 * Parameter:       field (struct field)       -> Koordinaten vom Feld, in welchem der Wert geändert werden soll
 * Parameter:       newValue (int)             -> Neuer Wert für das Feld
 * Parameter:       *error (int)               -> Enthält nach dem Funktionsaufruf: 1 = Feld kann nicht geändert werden, 2 = Neuer Wert ist kleiner als 1, 3 = Neuer Wert ist größer als 9
 *
 * Rückgabewert:    Gibt die Struktur 'sudoku' zurück
 *
 * Beschreibung:    Schreibt einen neuen Wert in ein ausgewähltes Feld im Sudoku-Muster.
 */
struct sudoku alterValue(struct sudoku sudokuGrid, struct field field, int newValue, int *error) {
    // Vordefinierte Felder dürfen nicht geändert werden
    if (sudokuGrid.generated[field.row][field.column] == 1) {
        *error = 1;
        return sudokuGrid;
    }

    // Wenn der Wert im Zahlenbereich der natürlichen Zahlen von 1 bis 9 liegt, Wert im Feld ändern
    if (newValue < 0) {
        *error = 2;
        return sudokuGrid;
    } else if (newValue > 9) {
        *error = 3;
        return sudokuGrid;
    } else {
        sudokuGrid.value[field.row][field.column] = newValue;
        *error = 0;
    }

    return sudokuGrid;
}

/**
 * Funktion:        fillRandomField
 *
 * Parameter:       sudokuGridSolved (struct sudoku) -> Sudoku-Muster, in dem die Werte stehen (fertig gelöst)
 * Parameter:       sudokuGrid (struct sudoku)       -> Sudoku-Muster, in dem die Werte stehen
 *
 * Rückgabewert:    Gibt die Struktur 'sudoku' zurück
 *
 * Beschreibung:    Schreibt einen neuen Wert in ein zufälliges Feld im Sudoku-Muster.
 */
struct sudoku fillRandomField(struct sudoku sudokuGridSolved, struct sudoku sudokuGrid) {
    struct field field;
    struct sudoku sudoku;
    int error;

    error = 1;
    sudoku = sudokuGrid;

    // Es kann nur ein Feld geüllt werden, wenn es noch leere Felder gibt
    if (countEmptyFields(sudokuGrid) > 0) {
        do {
            // Feld so lange zufällig wählen, bis alterValue() beim Ändern keinen Fehler zurückgibt
            field.row = rand() % 9;
            field.column = rand() % 9;

            if (sudokuGrid.value[field.row][field.column] == 0) {
                // Feld aus dem Sudoku mit dem selben Feld aus der Lösung ersetzen
                sudoku = alterValue(sudoku, field, sudokuGridSolved.value[field.row][field.column], &error);
            }
        } while (error != 0);
    }

    return sudoku;
}

/**
 * Funktion:        countEmptyFields
 *
 * Parameter:       sudokuGrid (struct sudoku) -> Sudoku-Muster, in dem die Werte stehen
 *
 * Rückgabewert:    Gibt die Anzahl der leeren Felder in einem Sudoku zurück
 *
 * Beschreibung:    Zählt leere Felder in einem Sudoku Muster.
 */
int countEmptyFields(struct sudoku sudokuGrid) {
    int i, j, counter;
    counter = 0;

    // Leere Felder zählen
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if (sudokuGrid.value[i][j] == 0) {
                counter++;
            }
        }
    }

    return counter;
}

/**
 * Funktion:        checkSavegameSlot
 *
 * Parameter:       slot (int) -> Speicherstand
 *
 * Rückgabewert:    Gibt eine 0 zurück, wenn der Spielstand nicht existiert, andernfalls wird eine 1 zurückgegeben
 *
 * Beschreibung:    Prüft ob Spielstand existiert.
 */
int checkSavegameSlot(int slot) {
    // Datei-Handle definieren
    FILE *fileHandle;
    char fileName[12] = "slot0.skram\0";

    fileName[4] = (slot - 1) + '0';

    // Prüfen ob Datei existiert
    fileHandle = fopen(fileName, "r");

    if (fileHandle == NULL) {
        return 0;
    }

    fclose(fileHandle);

    return 1;
}

/**
 * Funktion:        readSavegame
 *
 * Parameter:       slot (int)   -> Speicherstand
 * Parameter:       *error (int) -> Enthält nach dem Funktionsaufruf: 1 = Spielstand existiert nicht, 2 = Datei ist beschädigt oder enthält falschen Inhalt
 *
 * Rückgabewert:    Gibt die Struktur 'savegame' zurück
 *
 * Beschreibung:    Liest ein gespeichertes Spiel aus einer Datei (Slot) und gibt es als Struktur 'savegame' zurück.
 */
struct savegame readSavegame(int slot, int *error) {
    struct savegame sudoku;
    int temp, i, j, n;
    // Datei-Handle definieren
    FILE *fileHandle;
    char fileName[12] = "slot0.skram\0";

    *error = 0;
    fileName[4] = (slot - 1) + '0';
    i = 0;
    j = 0;
    n = 0;
    sudoku.pastTime = 0;

    fileHandle = fopen(fileName, "r");

    if (fileHandle == NULL) {
        *error = 1;
        return sudoku;
    }

    // Jedes Zeichen (Zahl) einzeln auslesen und in die Struktur zusammensetzen
    while (fscanf(fileHandle, "%1d", &temp) != EOF) {
        if (i < 10) {
            switch (n) {
                case 0:
                    sudoku.sudokuGridSolved.value[i][j] = temp;
                    break;

                case 1:
                    sudoku.sudokuGrid.value[i][j] = temp;
                    break;

                case 2:
                    sudoku.sudokuGridSolved.generated[i][j] = temp;
                    break;

                case 3:
                    sudoku.sudokuGrid.generated[i][j] = temp;
                    break;
            }

            n++;

            // Sobald 4 Ziffern ausgelesen wurden wird in die nächste Spalte gewechselt
            if (n > 3) {
                n = 0;
                j++;
            }

            // Sobald 9 Spalten ausgelesen wurden, wird in die nächste Zeile gewechselt
            if (j == 9) {
                j = 0;
                i++;
            }
        }
    }

    // Dateizeiger auf Dateianfang zurücksetzen
    fseek(fileHandle, 0, SEEK_SET);

    // Datei erneut bis Zeilenumbruch durchlaufen
    while ((temp = fgetc(fileHandle)) != EOF) {
        if (temp == '\n') {
            break;
        }
    }

    if (temp == EOF) {
        *error = 2;
        return sudoku;
    }

    // Nach Zeilenumbruch erneut jedes Zeichen (Zahl) einzeln auslesen und auf dem mit 10 multiplizierten Zeitwert der Struktur aufaddieren
    while (fscanf(fileHandle, "%1d", &temp) != EOF) {
        sudoku.pastTime *= 10;
        sudoku.pastTime += (double) temp;
    }

    fclose(fileHandle);

    return sudoku;
}

/**
 * Funktion:        writeSavegame
 *
 * Parameter:       sudoku (struct savegame) -> Savegame-Struktur, in dem der aktuelle Spielfortschritt, die Lösung und die bislang benötigte Zeit enthalten ist
 * Parameter:       slot (int)               -> Speicherstand
 *
 * Rückgabewert:    Gibt eine 0 für falsch und eine 1 für korrekt zurück
 *
 * Beschreibung:    Speichert den aktuellen Spielfortschritt in einer Datei (Slot).
 */
int writeSavegame(struct savegame sudoku, int slot) {
    int i, j;
    // Datei-Handle definieren
    FILE *fileHandle;
    char fileName[12] = "slot0.skram\0";

    fileName[4] = (slot - 1) + '0';

    fileHandle = fopen(fileName, "w");

    if (fileHandle == NULL) {
        return 0;
    }

    // Sudoku Strukturen zerlegen und in Datei schreiben
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            fprintf(fileHandle, "%d", sudoku.sudokuGridSolved.value[i][j]);
            fprintf(fileHandle, "%d", sudoku.sudokuGrid.value[i][j]);
            fprintf(fileHandle, "%d", sudoku.sudokuGridSolved.generated[i][j]);
            fprintf(fileHandle, "%d", sudoku.sudokuGrid.generated[i][j]);
        }
    }

    // Zeilenumbruch und Zeitwert in Datei schreiben
    fprintf(fileHandle, "\n%.0f", sudoku.pastTime);

    fclose(fileHandle);

    return 1;
}

/**
 * Funktion:        deleteSavegame
 *
 * Parameter:       slot (int) -> Speicherstand
 *
 * Beschreibung:    Löscht einen Speicherstand (Slot) -> Löscht die Datei.
 */
void deleteSavegame(int slot) {
	char fileName[12] = "slot0.skram\0";
    fileName[4] = (slot - 1) + '0';

	if (unlink(fileName) < 0) {
		// Darf nicht auftreten
		die();
	}

	return;
}

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

/**
 * Funktion:        showView
 *
 * Parameter:       view (int) -> View, welcher ausgefhrt werden soll
 *
 * Rückgabewert:    Gibt VIEW_HOME, VIEW_GAME_NEW, VIEW_GAME_LOAD, VIEW_CREDITS oder VIEW_EXIT zurück
 *
 * Beschreibung:    Stellt das Spiel in der Konsole dar.
 */
int showView(int view) {
    switch (view) {
        case VIEW_HOME:
            return viewHome();

        case VIEW_GAME_NEW:
            return viewGameNew();

        case VIEW_GAME_LOAD:
            return viewGameLoad();

        case VIEW_CREDITS:
            return viewCredits();

        default:
            return VIEW_EXIT;
    }
}

/**
 * Funktion:        viewHome
 *
 * Rückgabewert:    Gibt VIEW_GAME_NEW, VIEW_GAME_LOAD, VIEW_CREDITS oder VIEW_EXIT zurück
 *
 * Beschreibung:    Zeigt den Startbildschirm an.
 */
int viewHome() {
    int input, errorInput;
    char inputString[STRLEN];
    errorInput = 0;

    // Wiederholt die Anzeige des Startbilschirms so oft, bis eine gültige Eingabe erfolgt ist
    do {
        system("cls");
        printf("SKRAM Sudoku\n");
        printf("\n");
        printf("[1] Neues Spiel\n");
        printf("[2] Spiel laden\n");
        printf("[3] Credits\n");
        printf("[4] Beenden\n");
        printf("\n");

        if (errorInput == 1) {
            printf("Die Eingabe war Fehlerhaft, bitte erneut eingeben!\n");
        }

        printf("Eingabe: ");
        readLine(inputString);
        input = checkAndConvertInputToInt(inputString);

        if (input < 1 || input > 5) {
            errorInput = 1;
        } else {
            errorInput = 0;
        }
    } while (input < 1 || input > 5);

    switch (input) {
        case 1:
            return VIEW_GAME_NEW;

        case 2:
            return VIEW_GAME_LOAD;

        case 3:
            return VIEW_CREDITS;

        case 4:
            return VIEW_GAME_READ;

        case 5:
            return VIEW_EXIT;
    }

    // Darf nicht auftreten
    die();
}

/**
 * Funktion:        viewGameNew
 *
 * Rückgabewert:    Gibt VIEW_HOME zurück
 *
 * Beschreibung:    Zeigt den Bildschirm für ein neues Spiel an an.
 */
int viewGameNew() {
    int input, errorInput, difficulty;
    char inputString[STRLEN];
    errorInput = 0;

    // Wiederholt die Anzeige des Bildschirms für ein neues Spiel so oft, bis eine gültige Eingabe erfolgt ist
    do {
        system("cls");
        printf("SKRAM Sudoku\n");
        printf("\n");
        printf("Neues Spiel\n");
        printf("[1] Leicht\n");
        printf("[2] Mittel\n");
        printf("[3] Schwer\n");
        printf("[4] Zurueck zum Startbildschirm\n");
        printf("\n");

        if (errorInput == 1) {
            printf("Die Eingabe war Fehlerhaft, bitte erneut eingeben!\n");
        }

        printf("Eingabe: ");
        readLine(inputString);
        input = checkAndConvertInputToInt(inputString);

        if (input < 1 || input > 4) {
            errorInput = 1;
        } else {
            errorInput = 0;
        }
    } while (input < 1 || input > 4);

    switch (input) {
        case 1:
            difficulty = EASY;
            break;

        case 2:
            difficulty = MEDIUM;
            break;

        case 3:
            difficulty = DIFFICULT;
            break;

        case 4:
            return VIEW_HOME;
    }

    // Neues Sudoku nach Schwierigkeitsgrad generieren
    struct savegame sudoku;

    sudoku.sudokuGridSolved = generateFullSudoku();
    sudoku.sudokuGrid = makeSodukoPlayable(sudoku.sudokuGridSolved, difficulty);
    sudoku.pastTime = 0.0;

    // Generiertes Sudoku an den Play-View weitergeben und dessen Rückgabewert zurückgeben
    return viewGamePlay(sudoku);
}

/**
 * Funktion:        viewGameLoad
 *
 * Rückgabewert:    Gibt VIEW_HOME zurück
 *
 * Beschreibung:    Zeigt den Bildschirm an, um ein Spiel zu laden.
 */
int viewGameLoad() {
    int i, input, deletedSlot, errorInput, noSavegame;
    char inputString[STRLEN];
    deletedSlot = 0;
    errorInput = 0;
    noSavegame = 0;

    // Wiederholt die Anzeige des Bildschirms um ein gespeichertes Spiel zu laden so oft, bis eine gültige Eingabe erfolgt ist
    do {
        system("cls");
        printf("SKRAM Sudoku\n");
        printf("\n");
        printf("Spielstand laden\n");

        // Listet Spielstände 1 - 10 (0 - 9)
        for (i = 1; i <= 10; i++) {
            if (checkSavegameSlot(i)) {
                printf("[%d] Spielstand %d\n", i, i);
            } else {
                printf("[%d] Nicht vorhanden\n", i);
            }
        }

        printf("\n");
        printf("[11] Spielstand loeschen\n");
        printf("[12] Zurueck zum Startbildschirm\n");
        printf("\n");

        if (deletedSlot > 0) {
        	printf("Spielstand %d wurde erfolgreich geloescht!\n", deletedSlot);
        	deletedSlot = 0;
        } else {
        	if (errorInput == 1) {
        	    printf("Die Eingabe war Fehlerhaft, bitte erneut eingeben!\n");
        	} else {
        	    if (noSavegame == 1) {
        	        printf("Dieser Spielstand ist nicht vorhanden!\n");
        	    }
        	}
        }

        printf("Eingabe: ");
        readLine(inputString);
        input = checkAndConvertInputToInt(inputString);

        // Bei der Eingabe 11 soll ein Spielstand gelöscht werden
        if (input == 11) {
        	deletedSlot = viewGameDelete();
        	continue;
        }

        // Bei der Eingabe 12 soll zum Startbildschirm gewechselt werden
        if (input == 12) {
            return VIEW_HOME;
        }

        if (input < 1 || input > 10) {
            errorInput = 1;
        } else {
            errorInput = 0;
        }

        // Prüft, ob ausgewählter Spielstand verfügbar ist
        if (!checkSavegameSlot(input)) {
            noSavegame = 1;
        }
    } while (input < 1 || input > 10 || !checkSavegameSlot(input));

    // Definiert das Sudoku und liest es aus dem Spielstand ein
    struct savegame sudoku;
    int error;

    sudoku = readSavegame(input, &error);

    if (error != 0) {
        // Darf nicht auftreten
        die();
    }

    // Aus dem Speicherstand gelesenes Sudoku an den Play-View weitergeben und dessen Rückgabewert zurückgeben
    return viewGamePlay(sudoku);
}

/**
 * Funktion:        viewGameDelete
 *
 * Rückgabewert:    Gibt die Slotnummer des gelöschten Spielstandes zurück
 *
 * Beschreibung:    Zeigt den Bildschirm an, um ein Spiel zu löschen.
 */
int viewGameDelete () {
	int i, input, errorInput, noSavegame;
    char inputString[STRLEN];
    errorInput = 0;
    noSavegame = 0;

    // Wiederholt die Anzeige des Bildschirms um ein gespeichertes Spiel zu loeschen so oft, bis eine gültige Eingabe erfolgt ist
    do {
        system("cls");
        printf("SKRAM Sudoku\n");
        printf("\n");
        printf("Spielstand loeschen\n");

        // Listet Spielstände 1 - 10 (0 - 9)
        for (i = 1; i <= 10; i++) {
            if (checkSavegameSlot(i)) {
                printf("[%d] Spielstand %d\n", i, i);
            } else {
                printf("[%d] Nicht vorhanden\n", i);
            }
        }

        printf("\n");
        printf("[11] Abbrechen\n");
        printf("\n");

        if (errorInput == 1) {
            printf("Die Eingabe war Fehlerhaft, bitte erneut eingeben!\n");
        } else {
            if (noSavegame == 1) {
                printf("Dieser Spielstand ist nicht vorhanden!\n");
            }
        }

        printf("Eingabe: ");
        readLine(inputString);
        input = checkAndConvertInputToInt(inputString);

        // Bei der Eingabe 11 soll abgebrochen
        if (input == 11) {
        	return 0;
        }

        if (input < 1 || input > 10) {
            errorInput = 1;
        } else {
            errorInput = 0;
        }

        // Prüft, ob ausgewählter Spielstand verfügbar ist
        if (!checkSavegameSlot(input)) {
            noSavegame = 1;
        }
    } while (input < 1 || input > 10 || !checkSavegameSlot(input));

    // Spielstand löschen
    deleteSavegame(input);

    // Nummer des gelößchten Spielstandes zurückgeben
    return input;
}

/**
 * Funktion:        viewGamePlay
 *
 * Parameter:       sudoku (struct savegame) -> Savegame-Struktur, in dem der aktuelle Spielfortschritt, die Lösung und die bislang benötigte Zeit enthalten ist
 *
 * Rückgabewert:    Gibt VIEW_HOME zurück
 *
 * Beschreibung:    Zeigt den Spielbildschirm an.
 */
int viewGamePlay(struct savegame sudoku) {
    struct field field;
    struct time pastTime;
    int errorInput, row, value, column, error, alterError, saved, solved;
    char inputString[STRLEN];
    double startTime, timeDiff, staticPastTime;

    // Startzeitpunkt und bislang verbrauchte Zeit auslesen
    startTime = difftime(time(NULL), (time_t) 0);
    staticPastTime = sudoku.pastTime;
    errorInput = 0;
    alterError = 0;
    saved = 0;

    // Wiederholt die Anzeige des Spielebildschirms so oft, bis eine gültige Eingabe erfolgt ist bzw. das Sudoku gelöst wurde
    do {
        system("cls");
        printf("SKRAM Sudoku\n");
        printf("\n");
        printf("Das Spiel wird beendet, sobald Sie alle Felder korrekt ausgefuellt haben!\n");
        printf("Geben Sie folgende Menuecodes bei Zeile ein!\n");
        printf("[10] Zurueck zum Startbildschirm (Das aktuelle Spiel geht verloren!)\n");
        printf("[11] Spiel speichern\n");
        printf("[12] Tipp anzeigen (Befuellt ein zufaellig gewaehltes Feld)\n");
        printf("\n");

        // Sudoku in der Konsole darstellen
        printGrid(sudoku.sudokuGrid);

        printf("\n");

        if (saved == 1) {
            printf("Das Spiel wurde erfolgreich gespeichert!\n\n");
            saved = 0;
        }

        if (errorInput == 1) {
            printf("Die Eingabe war Fehlerhaft, bitte erneut eingeben!\n\n");
            errorInput = 0;
        } else {
            if (alterError == 1) {
                printf("Ein vordefiniertes Feld kann nicht geaendert werden!\n\n");
                alterError = 0;
            }
        }

        // Aktuell verbrauchte Zeit berechnen
        timeDiff = difftime(time(NULL), (time_t) 0) - startTime + staticPastTime;
        // Verbrauchte Zeit in hh:mm:ss umrechnen und in Struktur schreiben
        pastTime = getPastTime(timeDiff);
        printf("Bis zum zuletzt getaetigten Zug vergangene Zeit: %02d:%02d:%02d\n", pastTime.hours, pastTime.minutes, pastTime.seconds);
        printf("\n");

        fflush(stdin);
        printf("Zeile: ");
        readLine(inputString);
        row = checkAndConvertInputToInt(inputString);

        // Bei der Eingabe 10 soll zum Startbildschirm zurückgekehrt werden
        if (row == 10) {
            return VIEW_HOME;
        }

        // Bei der Eingabe 11 soll das Spiel gespeichert werden
        if (row == 11) {
            // Aktuell verbrauchte Zeit in die sudoku Struktur schreiben
            sudoku.pastTime = timeDiff;

            // sudoku Struktur an den Savegame-Viewcontroller übergeben und ausführen
            if (viewGameSave(sudoku)) {
                saved = 1;
                continue;
            } else {
                // Darf nicht auftreten
                die();
            }
        }

        // Bei der Eingabe 12 soll ein Tipp gegeben werden -> zufällig gewähltes Feld füllen
        if (row == 12) {
            // Nur ein Feld füllen, wenn es noch leere Felder gibt
            if (countEmptyFields(sudoku.sudokuGrid) > 0) {
                sudoku.sudokuGrid = fillRandomField(sudoku.sudokuGridSolved, sudoku.sudokuGrid);
            }

            // Sudoku prüfen
            solved = checkSudoku(sudoku.sudokuGrid);
            continue;
        }

        fflush(stdin);
        printf("Spalte: ");
        readLine(inputString);
        column = checkAndConvertInputChar(inputString);
        fflush(stdin);
        printf("Neuer Wert: ");
        readLine(inputString);
        value = checkAndConvertInputToInt(inputString);

        if (row < 1 || row > 12 || column < 0 || value < 0 || value > 9) {
            errorInput = 1;
        } else {
            errorInput = 0;
            field.row = row - 1;
            field.column = column;
            // Feld im Sudoku ändern
            sudoku.sudokuGrid = alterValue(sudoku.sudokuGrid, field, value, &error);

            if (error == 1 || error == 2 || error == 3) {
                alterError = 1;
            } else {
                alterError = 0;
            }
        }

        // Sudoku prüfen
        solved = checkSudoku(sudoku.sudokuGrid);
    } while (solved == 0);

    // Aktuell verbrauchte Zeit berechnen
    timeDiff = difftime(time(NULL), (time_t) 0) - startTime + staticPastTime;
    // Verbrauchte Zeit in hh:mm:ss umrechnen und in Struktur schreiben
    pastTime = getPastTime(timeDiff);

    // Sudoku gelöst Ausgabe
    system("cls");
    printf("SKRAM Sudoku\n");
    printf("\n");
    printGrid(sudoku.sudokuGrid);
    printf("\n");
    printf("Herzlichen Glueckwunsch!\n");
    printf("Sie haben das Sudoku erfolgreich geloest!\n");
    printf("\n");
    printf("Benoetigte Zeit: %02d:%02d:%02d\n", pastTime.hours, pastTime.minutes, pastTime.seconds);
    printf("\n");
    printf("Bitte beliebige Taste druecken, um zum Startbildschirm zurueckzukehren...\n");
    getch();

    // Nach beliebigen Tastendruck zum Startbilschirm zurückkehren
    return VIEW_HOME;
}

/**
 * Funktion:        viewGameSave
 *
 * Parameter:       sudoku (struct savegame) -> Savegame-Struktur, in dem der aktuelle Spielfortschritt, die Lösung und die bislang benötigte Zeit enthalten ist
 *
 * Rückgabewert:    Gibt eine 0 für falsch zurück und eine 1 für korrekt
 *
 * Beschreibung:    Zeigt den Speicherbildschirm an an.
 */
int viewGameSave(struct savegame sudoku) {
    int i, input, errorInput;
    char inputString[STRLEN];
    errorInput = 0;

    // Wiederholt die Anzeige des Speicherbildschirms so oft, bis eine gültige Eingabe erfolgt ist
    do {
        system("cls");
        printf("SKRAM Sudoku\n");
        printf("\n");
        printf("Spielstand speichern\n");

        // Speicherstand 1 - 10 (0 - 9)
        for (i = 1; i <= 10; i++) {
            if (checkSavegameSlot(i)) {
                printf("[%d] Spielstand %d (Waehlen zum Ueberschreiben)\n", i, i);
            } else {
                printf("[%d] Spielstand %d (leer)\n", i, i);
            }
        }

        printf("[11] Abbrechen\n");
        printf("\n");

        if (errorInput == 1) {
            printf("Die Eingabe war Fehlerhaft, bitte erneut eingeben!\n");
        }

        printf("Eingabe: ");
        readLine(inputString);
        input = checkAndConvertInputToInt(inputString);

        // Bei Eingabe 11 zum Spiel zurückkehren ohne Speichern
        if (input == 11) {
            return 1;
        }

        if (input < 1 || input > 10) {
            errorInput = 1;
        } else {
            errorInput = 0;
        }
    } while (input < 1 || input > 10);

    // Spielfortschritt speichern und zum Spiel zurückkehren
    if (!writeSavegame(sudoku, input)) {
        // Darf nicht auftreten
        die();
    }

    return 1;
}

/**
 * Funktion:        viewGameReadFromSudokuFile
 *
 * Rückgabewert:    xxx
 *
 * Beschreibung:    xxx.
 */
int viewGameReadFromSudokuFile() {
    struct savegame sudoku;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Für Marvin:
    // 1. Sudoku aus Datei einlesen
    // 2. Sudoku parsen und in die sudoku Struktur bringen
    // 3. Sudoku Struktur in die Savegamestruktur schreiben (oben in dieser Funktion definierte Variable) [sudokuGridSolved]
    // 4. Sudoku Struktur an die Funktion makeSudokuPlayable weitergeben und dessen Rückgabewert ebenfalls in die Savegamestruktur schreiben [sudokuGrid]
    // 5. pastTime in der Savegamestruktur auf 0 setzen
    //
    // Wichtig:
    // Die Sudoku-Struktur mit dem fertig gelösten Sudoku muss erst mit checkSudoku() geprüft werden, sonst kann nicht gespielt werden!
    // Eingaben von Zahlen werden als String eingelesen und an checkAndConvertInputToInt() weitergegeben!
    // Falls nicht gespielt werden kann, kann mit return VIEW_HOME zum Startbildschirm zurückgekehrt werden!
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Eingelesenes Sudoku an den Play-View weitergeben und dessen Rückgabewert zurückgeben
    return viewGamePlay(sudoku);
}

/**
 * Funktion:        viewCredits
 *
 * Rückgabewert:    Gibt VIEW_HOME zurück
 *
 * Beschreibung:    Zeigt die Credits an.
 */
int viewCredits() {
    int input, errorInput;
    char inputString[STRLEN];
    errorInput = 0;

    // Wiederholt die Anzeige des Credits-Bildschirms so oft, bis eine gültige Eingabe erfolgt ist
    do {
        system("cls");
        printf("SKRAM Sudoku\n");
        printf("\n");
        printf("Projektteam\n");
        printf("Kevin Wilsmann, Entwickler\n");
        printf("Andre Sieverding, Entwickler\n");
        printf("Robin Beckmann, Projectowner\n");
        printf("Marvin Buchmann, SCRUM-Master\n");
        printf("\n");
        printf("[1] Zurueck zum Startbildschirm\n");
        printf("\n");

        if (errorInput == 1) {
            printf("Die Eingabe war Fehlerhaft, bitte erneut eingeben!\n");
        }

        printf("Eingabe: ");
        readLine(inputString);
        input = checkAndConvertInputToInt(inputString);

        if (input != 1) {
            errorInput = 1;
        } else {
            errorInput = 0;
        }
    } while (input != 1);

    // Zum Startbildschirm zurückkehren
    return VIEW_HOME;
}

/**
 * Funktion:        printGrid
 *
 * Parameter:       sudokuGrid (struct sudoku) -> Nimmt den Rückgabewert von Funktion 'generateFullSudoku' oder 'makeSudokuPlayable' entgegen
 *
 * Beschreibung:    Gibt das Sudokuspielfeld in der Kommandozeile aus.
 */
void printGrid(struct sudoku sudokuGrid) {
    int i, j, k, l, row;

    // Windows API (Textfarbe ändern -> Definition)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD savedAttributes;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    savedAttributes = consoleInfo.wAttributes;

    row = 1;

    printf("     A B C   D E F   G H I\n");
    printf("   +-------+-------+-------+\n");

    for (i = 0; i < 3; i++) {
        for (j = (9 / 3 * i); j < ((9 / 3 * i) + 3); j++) {
            printf("%d. | ", row++);

            for (k = 0; k < 3; k++) {
                for (l = (9 / 3 * k); l < ((9 / 3 * k) + 3); l++) {
                    // Falls das aktuelle Feld ein Standardfeld ist (generiert), dann gelb färben, sonst weiß färben
                    if (sudokuGrid.generated[j][l] == 1) {
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                    } else {
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                    }

                    if (sudokuGrid.value[j][l] == 0) {
                        printf("  ");
                    } else {
                        printf("%d ", sudokuGrid.value[j][l]);
                    }

                    // Textfarbe zurücksetzen
                    SetConsoleTextAttribute(hConsole, savedAttributes);
                }

                printf("\b | ");
            }

            printf("\n");
        }

        printf("   +-------+-------+-------+\n");
    }

    return;
}
