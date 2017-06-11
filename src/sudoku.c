#include "../header/sudoku.h"

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
    return;
}

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
    int i, strLen, result;
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
    int strLen;
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
 * Ließt, intepretiert und überprüft eine Datei in ein Sudoku
 * Diese Funktion in den Views nutzen!
 *
 * @param dateiPfad
 * @param errorCode
 *
 * @return Das geparste und überprüfte Sudoku
 */
struct sudoku getSudokuFromFile(char path[1024], int *error) {
    struct sudoku sudoku;
    FILE *fileHandle;
    if (verifyFilePath(path) == 1) {
        *error = PARSER_VALID;
        fileHandle = fopen(path, "r");
        sudoku = parseToSudoku(fileHandle, error);
        fclose(fileHandle);
    } else {
        *error = PARSER_FILE_INACCESSIBLE;
    }

    return sudoku;
}

/**
 * Ließt und parsed eine Datei in ein Sudoku.
 *
 * @param dateiHandle
 * @param errorCode
 *
 * @return geparstesSudoku
 */
struct sudoku parseToSudoku(FILE *fileHandle, int *error) {
    struct sudoku sudoku;
    int i,j;
    for (i = 0; i < 9; i++) {
        // Lese eine Zeile der Datei aus, und
        // fülle die entsprechende Zeile  im Sudoku
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

/**
 * Überprüft ob das Sudoku nur Zahlen von (einschließlich)
 * 1 bis 9 enthält. Überprüft des Weiteren ob sudoku.generated nur 1 enthält.
 *
 * @param sudoku Sudoku to check
 *
 * @return Eine PARSER_* Konstante die den Fehlercode angibt.
 */
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

/**
 * Zeigt eine Fehlernachricht entsprechend ihrer Bedeutung an.
 *
 * @param errorCode Fehlercode, zu welchem eine Nachricht darzustellen ist.
 */
void showParserErrorMessage(int errorCode) {
    switch (errorCode) {
        case (PARSER_FILE_INACCESSIBLE):
            printf("Die angegebene Datei existiert nicht, oder auf sie kann nicht zugegriffen werden.\n");
            printf("Bitte ueberpruefen sie Ihre Eingabe und versuchen es erneut!\n\n");
            break;

        case (PARSER_SUDOKU_NUMBERS_INVALID):
            printf("Die Datei enthaelt ungueltigen Eingaben.");
            printf("Sie sollte neun Reihen enthalten,\nwelche wie folgt aufgebaut sein sollten:\n");
            printf("\"i,i,i,i,i,i,i,i,i\" (i steht fuer eine beliebige Zahl von 1-9)\n\n");
            break;

        case (PARSER_SUDOKU_INVALID):
            printf("Das Sudoku ist kein gueltiges Sudoku.\n");
            printf("Die Zahlen in einer Reihe, Zeile und in einem 3x3 Quadrat muessen einmalig sein.\n\n");
            break;
    }
    return;
}

/**
 * Verifizieret ob dateiPfad eine lesbare Datei identifiziert.
 *
 * @param dateiPfad
 *
 * @return 1 wenn der Pfad
 */
int verifyFilePath(char path[1024]) {
    int fileExists;
    FILE *fileHandle;
    fileHandle = fopen(path, "r");

    if (fileHandle == NULL) {
        fileExists = 0;
    } else {
        fileExists = 1;
        fclose(fileHandle);
    }

    return fileExists;
}
