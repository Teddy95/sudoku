#ifndef VIEWS_H_INCLUDED
#define VIEWS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#include "sudoku.h"

// Ausgabe-/Viewfunktionen
int showView(int view);
int viewHome();
int viewGameNew();
int viewGameLoad();
int viewGameDelete();
int viewGamePlay(struct savegame);
int viewGameSave(struct savegame);
int viewGameReadFromSudokuFile();
int viewCredits();
void printGrid(struct sudoku);

#endif // VIEWS_H_INCLUDED
