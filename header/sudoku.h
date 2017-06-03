#ifndef SUDOKU_H_INCLUDED
#define SUDOKU_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>
#include <ctype.h>
#include <conio.h>


// Definition von Konstaten für den Schwierigkeitsgrad
#define EASY 1
#define MEDIUM 2
#define DIFFICULT 3

// Parser Konstanten
#define PARSER_VALID 0 //! Parsing erfolgreich
#define PARSER_FILE_INACCESSIBLE 1 //! Datei nicht lesbar
#define PARSER_SUDOKU_NUMBERS_INVALID 2 //! Sudoku enthielt falsch Eingaben.
#define PARSER_SUDOKU_INVALID 3

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
#define STRLEN 1024

// Definition von horizontaler sowie vertikaler Sudoku-Größe
#define SIZE 9


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

// Savegame-Funktionen
int checkSavegameSlot(int slot);
struct savegame readSavegame(int slot, int *error);
int writeSavegame(struct savegame sudoku, int slot);
void deleteSavegame(int slot);
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
// File parsing for custom sudoku
struct sudoku getSudokuFromFile(char[1024], int*);
struct sudoku parseToSudoku(FILE*, int*);
int checkParsedSudoku(struct sudoku);
void showParserErrorMessage(int);
int verifyFilePath(char[1024]);

#include "displayTime.h"
#include "views.h"

#endif // SUDOKU_H_INCLUDED
