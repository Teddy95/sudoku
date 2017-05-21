#ifndef VIEWS_H_INCLUDED
#define VIEWS_H_INCLUDED

#include "../header/sudoku.h"

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

#endif // VIEWS_H_INCLUDED
