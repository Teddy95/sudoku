#include "../header/views.h"

/**
 * Funktion:        showView
 *
 * Parameter:       view (int) -> View, welcher ausgefhrt werden soll
 *
 * R�ckgabewert:    Gibt VIEW_HOME, VIEW_GAME_NEW, VIEW_GAME_LOAD, VIEW_CREDITS oder VIEW_EXIT zur�ck
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

        case VIEW_GAME_READ:
            return viewGameReadFromSudokuFile();

        default:
            return VIEW_EXIT;
    }
}

/**
 * Funktion:        viewHome
 *
 * R�ckgabewert:    Gibt VIEW_GAME_NEW, VIEW_GAME_LOAD, VIEW_CREDITS oder VIEW_EXIT zur�ck
 *
 * Beschreibung:    Zeigt den Startbildschirm an.
 */
int viewHome() {
    int input, errorInput;
    char inputString[STRLEN];
    errorInput = 0;

    // Wiederholt die Anzeige des Startbilschirms so oft, bis eine g�ltige Eingabe erfolgt ist
    do {
        system("cls");
        printf("SKRAM Sudoku\n");
        printf("\n");
        printf("[1] Neues Spiel\n");
        printf("[2] Spiel laden\n");
        printf("[3] Credits\n");
        printf("[4] Spiel einlesen\n");
        printf("[5] Beenden\n");
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
    return -1;
}

/**
 * Funktion:        viewGameNew
 *
 * R�ckgabewert:    Gibt VIEW_HOME zur�ck
 *
 * Beschreibung:    Zeigt den Bildschirm f�r ein neues Spiel an an.
 */
int viewGameNew() {
    int input, errorInput, difficulty;
    char inputString[STRLEN];
    errorInput = 0;

    // Wiederholt die Anzeige des Bildschirms f�r ein neues Spiel so oft, bis eine g�ltige Eingabe erfolgt ist
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

    // Generiertes Sudoku an den Play-View weitergeben und dessen R�ckgabewert zur�ckgeben
    return viewGamePlay(sudoku);
}

/**
 * Funktion:        viewGameLoad
 *
 * R�ckgabewert:    Gibt VIEW_HOME zur�ck
 *
 * Beschreibung:    Zeigt den Bildschirm an, um ein Spiel zu laden.
 */
int viewGameLoad() {
    int i, input, deletedSlot, errorInput, noSavegame;
    char inputString[STRLEN];
    deletedSlot = 0;
    errorInput = 0;
    noSavegame = 0;

    // Wiederholt die Anzeige des Bildschirms um ein gespeichertes Spiel zu laden so oft, bis eine g�ltige Eingabe erfolgt ist
    do {
        system("cls");
        printf("SKRAM Sudoku\n");
        printf("\n");
        printf("Spielstand laden\n");

        // Listet Spielst�nde 1 - 10 (0 - 9)
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

        // Bei der Eingabe 11 soll ein Spielstand gel�scht werden
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

        // Pr�ft, ob ausgew�hlter Spielstand verf�gbar ist
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

    // Aus dem Speicherstand gelesenes Sudoku an den Play-View weitergeben und dessen R�ckgabewert zur�ckgeben
    return viewGamePlay(sudoku);
}

/**
 * Funktion:        viewGameDelete
 *
 * R�ckgabewert:    Gibt die Slotnummer des gel�schten Spielstandes zur�ck
 *
 * Beschreibung:    Zeigt den Bildschirm an, um ein Spiel zu l�schen.
 */
int viewGameDelete () {
	int i, input, errorInput, noSavegame;
    char inputString[STRLEN];
    errorInput = 0;
    noSavegame = 0;

    // Wiederholt die Anzeige des Bildschirms um ein gespeichertes Spiel zu loeschen so oft, bis eine g�ltige Eingabe erfolgt ist
    do {
        system("cls");
        printf("SKRAM Sudoku\n");
        printf("\n");
        printf("Spielstand loeschen\n");

        // Listet Spielst�nde 1 - 10 (0 - 9)
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

        // Pr�ft, ob ausgew�hlter Spielstand verf�gbar ist
        if (!checkSavegameSlot(input)) {
            noSavegame = 1;
        }
    } while (input < 1 || input > 10 || !checkSavegameSlot(input));

    // Spielstand l�schen
    deleteSavegame(input);

    // Nummer des gel��chten Spielstandes zur�ckgeben
    return input;
}

/**
 * Funktion:        viewGamePlay
 *
 * Parameter:       sudoku (struct savegame) -> Savegame-Struktur, in dem der aktuelle Spielfortschritt, die L�sung und die bislang ben�tigte Zeit enthalten ist
 *
 * R�ckgabewert:    Gibt VIEW_HOME zur�ck
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

    // Wiederholt die Anzeige des Spielebildschirms so oft, bis eine g�ltige Eingabe erfolgt ist bzw. das Sudoku gel�st wurde
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

        // Bei der Eingabe 10 soll zum Startbildschirm zur�ckgekehrt werden
        if (row == 10) {
            return VIEW_HOME;
        }

        // Bei der Eingabe 11 soll das Spiel gespeichert werden
        if (row == 11) {
            // Aktuell verbrauchte Zeit in die sudoku Struktur schreiben
            sudoku.pastTime = timeDiff;

            // sudoku Struktur an den Savegame-Viewcontroller �bergeben und ausf�hren
            if (viewGameSave(sudoku)) {
                saved = 1;
                continue;
            } else {
                // Darf nicht auftreten
                die();
            }
        }

        // Bei der Eingabe 12 soll ein Tipp gegeben werden -> zuf�llig gew�hltes Feld f�llen
        if (row == 12) {
            // Nur ein Feld f�llen, wenn es noch leere Felder gibt
            if (countEmptyFields(sudoku.sudokuGrid) > 0) {
                sudoku.sudokuGrid = fillRandomField(sudoku.sudokuGridSolved, sudoku.sudokuGrid);
            }

            // Sudoku pr�fen
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
            // Feld im Sudoku �ndern
            sudoku.sudokuGrid = alterValue(sudoku.sudokuGrid, field, value, &error);

            if (error == 1 || error == 2 || error == 3) {
                alterError = 1;
            } else {
                alterError = 0;
            }
        }

        // Sudoku pr�fen
        solved = checkSudoku(sudoku.sudokuGrid);
    } while (solved == 0);

    // Aktuell verbrauchte Zeit berechnen
    timeDiff = difftime(time(NULL), (time_t) 0) - startTime + staticPastTime;
    // Verbrauchte Zeit in hh:mm:ss umrechnen und in Struktur schreiben
    pastTime = getPastTime(timeDiff);

    // Sudoku gel�st Ausgabe
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

    // Nach beliebigen Tastendruck zum Startbilschirm zur�ckkehren
    return VIEW_HOME;
}

/**
 * Funktion:        viewGameSave
 *
 * Parameter:       sudoku (struct savegame) -> Savegame-Struktur, in dem der aktuelle Spielfortschritt, die L�sung und die bislang ben�tigte Zeit enthalten ist
 *
 * R�ckgabewert:    Gibt eine 0 f�r falsch zur�ck und eine 1 f�r korrekt
 *
 * Beschreibung:    Zeigt den Speicherbildschirm an an.
 */
int viewGameSave(struct savegame sudoku) {
    int i, input, errorInput;
    char inputString[STRLEN];
    errorInput = 0;

    // Wiederholt die Anzeige des Speicherbildschirms so oft, bis eine g�ltige Eingabe erfolgt ist
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

        // Bei Eingabe 11 zum Spiel zur�ckkehren ohne Speichern
        if (input == 11) {
            return 1;
        }

        if (input < 1 || input > 10) {
            errorInput = 1;
        } else {
            errorInput = 0;
        }
    } while (input < 1 || input > 10);

    // Spielfortschritt speichern und zum Spiel zur�ckkehren
    if (!writeSavegame(sudoku, input)) {
        // Darf nicht auftreten
        die();
    }

    return 1;
}

/**
 * Funktion:        viewGameReadFromSudokuFile
 *
 * R�ckgabewert:    Gibt VIEW_HOME zur�ck
 *
 * Beschreibung:    Zeigt den Bildschirm an, um ein eigenes Sudoku einzulesen.
 */
int viewGameReadFromSudokuFile() {
    struct savegame sudoku;
    struct sudoku parsedSudoku;

    int input = 0, errorInput = 0, difficulty = 0, difficultyInput = 0;
    char inputString[STRLEN], difficultyInputString[STRLEN];
    errorInput = -1;

    // Wiederholt die Anzeige des Startbilschirms so oft, bis eine g�ltige Eingabe erfolgt ist
    do {
        system("cls");
        printf("SKRAM Sudoku\n");
        printf("\n");
        printf("Sudoku aus Datei laden\n");
        printf("\n");
        printf("Geben Sie bitte den Pfad zur einzulesenden Datei ein.\nGeben Sie eine 1 ein, wenn Sie zurueck zum Hauptmenue navigieren moechten.\n");
        printf("\n");


        showParserErrorMessage(errorInput);

        printf("Eingabe: ");
        readLine(inputString);
        input = checkAndConvertInputToInt(inputString);

        parsedSudoku = getSudokuFromFile(inputString, &errorInput);

        do {
            if (input == 1 || errorInput != PARSER_VALID) {
                break;
            }

            system("cls");
            printf("SKRAM Sudoku\n");
            printf("\n");
            printf("Sudoku aus Datei laden\n");
            printf("\n");
            printf("Bitte geben Sie einen Schwierigkeits an:\n");
            printf("[1] Leicht\n");
            printf("[2] Mittel\n");
            printf("[3] Schwer\n");
            printf("[4] Zurueck zum Startbildschirm\n");
            printf("\n");
            printf("Eingabe: ");

            readLine(difficultyInputString);
            difficultyInput = checkAndConvertInputToInt(difficultyInputString);

            switch(difficultyInput) {
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
                    break;
            }
        } while (difficulty != EASY
                 && difficulty != MEDIUM
                 && difficulty != DIFFICULT);

        if (input == 1) {
            return VIEW_HOME;
        }

        if (checkSudoku(parsedSudoku)) {
            sudoku.sudokuGridSolved = parsedSudoku;
            sudoku.sudokuGrid = makeSodukoPlayable(parsedSudoku, difficulty);
            sudoku.pastTime = 0.0;
        } else if (errorInput == -1 || errorInput == 0) {
            errorInput = PARSER_SUDOKU_INVALID;
        }

    } while (input != 1 && errorInput != PARSER_VALID);

    // Eingelesenes Sudoku an den Play-View weitergeben und dessen R�ckgabewert zur�ckgeben
    return viewGamePlay(sudoku);
}

/**
 * Funktion:        viewCredits
 *
 * R�ckgabewert:    Gibt VIEW_HOME zur�ck
 *
 * Beschreibung:    Zeigt die Credits an.
 */
int viewCredits() {
    int input, errorInput;
    char inputString[STRLEN];
    errorInput = 0;

    // Wiederholt die Anzeige des Credits-Bildschirms so oft, bis eine g�ltige Eingabe erfolgt ist
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

    // Zum Startbildschirm zur�ckkehren
    return VIEW_HOME;
}

/**
 * Funktion:        printGrid
 *
 * Parameter:       sudokuGrid (struct sudoku) -> Nimmt den R�ckgabewert von Funktion 'generateFullSudoku' oder 'makeSudokuPlayable' entgegen
 *
 * Beschreibung:    Gibt das Sudokuspielfeld in der Kommandozeile aus.
 */
void printGrid(struct sudoku sudokuGrid) {
    int i, j, k, l, row;

    // Windows API (Textfarbe �ndern -> Definition)
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
                    // Falls das aktuelle Feld ein Standardfeld ist (generiert), dann gelb f�rben, sonst wei� f�rben
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

                    // Textfarbe zur�cksetzen
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
