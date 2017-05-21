#ifndef SUDOKU_H_INCLUDED
#define SUDOKU_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>
#include "../header/displayTime.h"
#include "../header/views.h"

// Definition von Konstaten f�r den Schwierigkeitsgrad
#define EASY 1
#define MEDIUM 2
#define DIFFICULT 3

// Parser Konstanten
#define PARSER_VALID 0 //! Parsing erfolgreich
#define PARSER_FILE_INACCESSIBLE 1 //! Datei nicht lesbar
#define PARSER_SUDOKU_NUMBERS_INVALID 2 //! Sudoku enthielt falsch Eingaben.
#define PARSER_SUDOKU_INVALID 3

// Definition von Konstaten f�r die Views
#define VIEW_HOME 0
#define VIEW_GAME_NEW 1
#define VIEW_GAME_LOAD 2
#define VIEW_GAME_PLAY 3
#define VIEW_GAME_SAVE 4
#define VIEW_CREDITS 5
#define VIEW_EXIT 6
#define VIEW_GAME_READ 7

// Definition von Konstante f�r maximale Zeichenkettenl�nge
#define STRLEN 1024

// Definition von horizontaler sowie vertikaler Sudoku-Gr��e
#define SIZE 9


// Sudoku-Gitter
struct sudoku {
    int value[9][9];     // Werte im Sudoku
    int generated[9][9]; // 1 = Wert kann nicht ge�ndert werden, 0 = Wert kann ge�ndert werden
};

// Koordinaten eines Feldes im Sudoku-Gitter
struct field {
    int row;
    int column;
};

// Struktur f�r einen gespeicherten Spielstand
struct savegame {
    struct sudoku sudokuGridSolved;
    struct sudoku sudokuGrid;
    double pastTime;
};

// Savegame-Funktionen
int checkSavegameSlot(int slot);
struct savegame readSavegame(int slot, int *error);
int writeSavegame(struct savegame sudoku, int slot);
void deleteSavegame(int slot);


/**
 * Lie�t, intepretiert und �berpr�ft eine Datei in ein Sudoku
 * Diese Funktion in den Views nutzen!
 *
 * @param detaiPfad
 * @param errorCode
 *
 * @return Das geparste und �berpr�fte Sudoku
 */
struct sudoku getSudokuFromFile(char[1024], int*);

/**
 * Lie�t und parsed eine Datei in ein Sudoku.
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
 * �berpr�ft ob das Sudoku nur Zahlen von (einschlie�lich)
 * 1 bis 9 enth�lt. �berpr�ft des Weiteren ob sudoku.generated nur 1 enth�lt.
 *
 * @param sudoku Sudoku to check
 *
 * @return Eine PARSER_* Konstante die den Fehlercode angibt.
 */
int checkParsedSudoku(struct sudoku);

/**
 * Zeigt eine Fehlernachricht entsprechend ihrer Bedeutung an.
 *
 * @param errorCode Fehlercode, zu welchem eine Nachricht darzustellen ist.
 */
void showParserErrorMessage(int);

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
// Funktionen zum Pr�fen des Sudokus
int checkSudoku(struct sudoku sudokuGrid);
int checkValue(struct sudoku sudokuGrid, struct field field);
int checkRow(struct sudoku sudokuGrid, struct field field);
int checkColumn(struct sudoku sudokuGrid, struct field field);
int checkGroup(struct sudoku sudokuGrid, struct field field);
// Funktionen zum pr�fen und Konvertieren von Eingaben
int charToInt(char charakter);
int checkAndConvertInputToInt(char inputString[]);
int checkAndConvertInputChar(char inputString[]);
void readLine(char inputString[]);
// Feldfunktionen
struct sudoku alterValue(struct sudoku sudokuGrid, struct field field, int newValue, int *error);
struct sudoku fillRandomField(struct sudoku sudokuGridSolved, struct sudoku sudokuGrid);
int countEmptyFields(struct sudoku sudokuGrid);

#endif // SUDOKU_H_INCLUDED
