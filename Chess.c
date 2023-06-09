#include <locale.h>

#include "Chess.h"
#include "ChessParser.h"

#ifdef _WIN32
	#define OS_NAME "Windows"
	#include <windows.h>
	#include <tlhelp32.h>
#elif __unix__
	#define OS_NAME "Unix-based"
#else
	#define OS_NAME "Unknown"
#endif

static int hasCastled[2] = {0, 0};
static int takenPiecesCounter[10] = {0};
static int gameOver = 0;

int doesTerminalSupportUnicode() {
    char* term = getenv("TERM");
    int unicodeSupported = 0;
    if (term != NULL) {
        setlocale(LC_CTYPE, "");
        if (strstr(term, "xterm") != NULL || strstr(term, "rxvt") != NULL || strstr(term, "linux") != NULL) {
            unicodeSupported = 1;
        }
    }
   #ifdef _WIN32
    unicodeSupported = checkForUnicodeShellsWindows();
    if (unicodeSupported)
    	SetConsoleOutputCP(CP_UTF8);
   #endif
    return unicodeSupported;
}

int checkForUnicodeShellsWindows() {
   #ifdef _WIN32
    HWND consoleWindow = GetConsoleWindow();
    DWORD processId;
    GetWindowThreadProcessId(consoleWindow, &processId);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(entry);

        if (Process32First(snapshot, &entry)) {
            do {
                if (entry.th32ProcessID == processId) {
                    if (_stricmp(entry.szExeFile, "powershell.exe") == 0 ||
                        _stricmp(entry.szExeFile, "pwsh.exe") == 0 ||
                        _stricmp(entry.szExeFile, "wt.exe") == 0 ||
                        _stricmp(entry.szExeFile, "cmd.exe") == 0 ||
                        _stricmp(entry.szExeFile, "putty.exe") == 0 ||
                        _stricmp(entry.szExeFile, "git-bash.exe") == 0 ||
                        _stricmp(entry.szExeFile, "ConEmu.exe") == 0 ||
                        _stricmp(entry.szExeFile, "Cmder.exe") == 0) {
                        CloseHandle(snapshot);
                        return 1;
                    }
                    break;
                }
            } while (Process32Next(snapshot, &entry));
        }
        CloseHandle(snapshot);
    }
   #endif
    return 0;
}

void appendMoveToHistory(char* history, const char* move) {
    strncat(history, move, 6);
    strcat(history, "\n");
}

int collectInput(char* input, char* filename) {
	printf("Chess$ ");
    if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
        printf("chess app status: fgets failed\n");
        return -1;
    }

    if (strcmp(input, "\n") != 0) input[strcspn(input, "\n")] = '\0';

    if (strlen(input) > 5)
   	    strcpy(filename, input + 5);

    char* ext = NULL;
    size_t fnlen = strlen(filename);
    if (fnlen > 4)
    	ext = strstr(filename + fnlen - 4, ".");


    if (strncmp(input, "save", 4) == 0 && ext != NULL && strcmp(ext, ".bin") == 0) return 2;
    if (strncmp(input, "load", 4) == 0 && ext != NULL && strcmp(ext, ".bin") == 0) return 3;
    if (strncmp(input, "print", 5) == 0 && ext != NULL && strcmp(ext, ".txt") == 0)  return 4;
    if (strncmp(input, "read", 4) == 0 && ext != NULL && strcmp(ext, ".txt") == 0) return 5;
    if (strncmp(input, "reset", 5) == 0) return 6;
    if (strncmp(input, "sizes", 5) == 0) return 7;
    if (strncmp(input, "help", 4) == 0) return 8;
    if (strncmp(input, "yes", 3) == 0) return 9;
    if (strncmp(input, "no", 2) == 0) return 10;
    if (strncmp(input, "Queen", 5) == 0 || strncmp(input, "Knight", 6) == 0 ||
    	strncmp(input, "Rook", 4) == 0 || strncmp(input, "Bishop", 6) == 0)
    	return 11;
    if (strncmp(input, "exit", 4) == 0) return 1;

    return 0;
}

void incrementCaptureCounter(PieceType p, Color c) {
    int counterIndex = -1;
    switch(c) {
        case BLACK: counterIndex += 5; break;
        case WHITE: default: break;
    }
    switch(p) {
        case PAWN: counterIndex += 1; break;
        case KNIGHT: counterIndex += 2; break;
        case BISHOP: counterIndex += 3; break;
        case ROOK: counterIndex += 4; break;
        case QUEEN: counterIndex += 5; break;
        case KING: default: break;
    }
    ++takenPiecesCounter[counterIndex];
}

int checkChessRules(Board* board, Piece* piece, int bx, int by, int ax, int ay, int inc_mode) {
    if (piece != NULL) {
        switch (piece->type) {
            case PAWN: {
                if (piece->color == WHITE) {
                    if (by == ay && bx == ax - 1 &&
                        board->squares[ax][ay] == NULL)
                        return 1; // normal forward white pawn move
                    else if (bx == 1 && ax == 3 &&
                        board->squares[ax - 1][ay] == NULL &&
                        board->squares[ax][ay] == NULL)
                        return 1; // two-square forward white pawn move
                    else if (bx == ax - 1 && (by == ay - 1 || by == ay + 1) &&
                        board->squares[ax][ay] != NULL &&
                        board->squares[ax][ay]->color == BLACK) {
                        if (inc_mode == 1) incrementCaptureCounter(board->squares[ax][ay]->type, board->squares[ax][ay]->color);
                        return 1; // Capture
                    }
                    else if (bx == ax - 1 && (by == ay - 1 || by == ay + 1) &&
                        board->squares[ax][ay] == NULL &&
                        board->lastMove->piece != NULL &&
                        board->lastMove->piece->type == PAWN &&
                        board->lastMove->piece->color == BLACK &&
                        board->lastMove->bx == ax + 1 &&
                        board->lastMove->ax == ax - 1 &&
                        board->lastMove->by == ay &&
                        board->lastMove->ay == ay) {
                        int i = board->lastMove->ax;
                        int j = board->lastMove->ay;
                        if (inc_mode == 1) incrementCaptureCounter(board->squares[i][j]->type, board->squares[i][j]->color);
                        board->squares[i][j] = NULL;
                        return 1; // En passant for white pawn
                    }
                }
                else if (piece->color == BLACK) {
                    if (by == ay && bx == ax + 1 && 
                        board->squares[ax][ay] == NULL)
                        return 1; // Normal move
                    else if (bx == 6 && ax == 4 && 
                        board->squares[ax + 1][ay] == NULL && 
                        board->squares[ax][ay] == NULL)
                        return 1; // First move of the game
                    else if (bx == ax + 1 && (by == ay - 1 || by == ay + 1) && 
                        board->squares[ax][ay] != NULL && 
                        board->squares[ax][ay]->color == WHITE) {
                        if (inc_mode == 1) incrementCaptureCounter(board->squares[ax][ay]->type, board->squares[ax][ay]->color);
                        return 1; // Capture
                    } 
                    else if (bx == ax + 1 && (by == ay - 1 || by == ay + 1) &&
                        board->squares[ax][ay] == NULL &&
                        board->lastMove->piece != NULL &&
                        board->lastMove->piece->type == PAWN &&
                        board->lastMove->piece->color == WHITE &&
                        board->lastMove->bx == ax - 1 &&
                        board->lastMove->ax == ax + 1 &&
                        board->lastMove->by == ay &&
                        board->lastMove->ay == ay) {
                        int i = board->lastMove->ax;
                        int j = board->lastMove->ay;
                        if (inc_mode == 1) incrementCaptureCounter(board->squares[i][j]->type, board->squares[i][j]->color);
                        board->squares[i][j] = NULL;
                        return 1; // En passant for white pawn
                    }
                }
                break;
            }
            case ROOK: { 
                if ((bx == ax && by != ay) || (by == ay && bx != ax)) {
                    int xdir = (bx - ax) > 0 ? 1 : (bx - ax) < 0 ? -1 : 0;
                    int ydir = (by - ay) > 0 ? 1 : (by - ay) < 0 ? -1 : 0;
                    int x = ax + xdir;
                    int y = ay + ydir;
                    while (x != bx || y != by) {
                        if (board->squares[x][y] != NULL)
                            return 0; // A piece is blocking rook
                        x += xdir;
                        y += ydir;
                    }
                    if (board->squares[ax][ay] != NULL && board->squares[bx][by] != NULL) {
                        if (board->squares[bx][by]->color == board->squares[ax][ay]->color)
                            return 0; // don't take own color
                        if (inc_mode == 1) incrementCaptureCounter(board->squares[ax][ay]->type, board->squares[ax][ay]->color);
                    }
                    return 1; // nothing is blocking the rook
                }
                break;
            }
            case KNIGHT: {
                int dx = abs(ax - bx);
                int dy = abs(ay - by);
                if ((dx == 1 && dy ==2) || (dx == 2 && dy == 1)) {
                    if (board->squares[ax][ay] == NULL)
                        return 1;
                    if (board->squares[ax][ay] != NULL &&
                        board->squares[ax][ay]->color != piece->color) {
                        if (inc_mode == 1) incrementCaptureCounter(board->squares[ax][ay]->type, board->squares[ax][ay]->color);
                        return 1;
                    }
                }
                break; 
            }
            case BISHOP: { 
                int dx = abs(ax - bx);
                int dy = abs(ay - by);
                if (dx == dy) {
                    int xdir = (bx - ax) > 0 ? 1 : (bx - ax) < 0 ? -1 : 0;
                    int ydir = (by - ay) > 0 ? 1 : (by - ay) < 0 ? -1 : 0;
                    int x = ax + xdir;
                    int y = ay + ydir;
                    while (x != bx || y != by) {
                    	if (x >= 0 && x < CHESS_DIM && y >= 0 && y < CHESS_DIM) {
    	                    if (board->squares[x][y] != NULL)
    	                        return 0; // A piece is blocking the way

    	                    x += xdir;
    	                    y += ydir;
    	                }
    	                else
    	                	break;
                    }
                    if (board->squares[ax][ay] != NULL && board->squares[bx][by] != NULL) {
                        if (board->squares[bx][by]->color == board->squares[ax][ay]->color)
                            return 0; // don't take own color
                        if (inc_mode == 1) incrementCaptureCounter(board->squares[ax][ay]->type, board->squares[ax][ay]->color);
                    }
                    return 1; // No pieces are blocking
                }

                break; 
            }
            case QUEEN: { 
                int dx = abs(ax - bx);
                int dy = abs(ay - by);
                if ((bx == ax && by != ay) || (by == ay && bx != ax) || (dx == dy)) {
                    int xdir = (bx - ax) > 0 ? 1 : (bx - ax) < 0 ? -1 : 0;
                    int ydir = (by - ay) > 0 ? 1 : (by - ay) < 0 ? -1 : 0;
                    int x = ax + xdir;
                    int y = ay + ydir;
                    while (x != bx || y != by) {
                        if (board->squares[x][y] != NULL)
                            return 0;
                        x += xdir;
                        y += ydir;
                    }
                    if (board->squares[ax][ay] != NULL && board->squares[bx][by] != NULL) {
                        if (board->squares[bx][by]->color == board->squares[ax][ay]->color)
                            return 0; // don't take own color
                        if (inc_mode == 1) incrementCaptureCounter(board->squares[ax][ay]->type, board->squares[ax][ay]->color);
                    }
                    return 1; // no pieces are blocking
                }
                break; 
            }
            case KING: { 
                int dx = abs(ax - bx);
                int dy = abs(ay - by);
                if ((dx == 0 && dy == 1) || (dx == 1 && dy == 0) || (dx == 1 && dy == 1)) {
                    if (board->squares[ax][ay] == NULL || 
                        board->squares[ax][ay]->color != piece->color)
                        return 1; // normal king move
                }
                else if (dx == 0 && dy == 2) {
                    if (piece->hasMoved || hasCastled[piece->color] == 1) return 0; //king has already moved too bad!
                    int rookx = ax;
                    int rooky = ay > by ? 7 : 0;
                    int ydir = by < ay ? 1 : -1;
                    int y = by + ydir;
                    while (y != rooky) {
                        if (board->squares[rookx][y] != NULL || isSquareAttacked(board, piece->color % 2 == 1, rookx, y))
                            return 0; // piece is blocking the way, or middle square is being attacked
                        y += ydir;
                    }

                    if (board->squares[rookx][rooky] != NULL && (board->squares[rookx][rooky]->type != ROOK ||
                        board->squares[rookx][rooky]->hasMoved))
                        return 0;
                    
                    if (rooky == 0) { // queen side rook replacing:
                        free(board->squares[rookx][rooky]);
                        board->squares[rookx][rooky] = NULL;
                        initializePieceAtSquare(board, ROOK, piece->color, rookx, rooky + 3);
                    } else { // king side
                        free(board->squares[rookx][rooky]);
                        board->squares[rookx][rooky] = NULL;
                        initializePieceAtSquare(board, ROOK, piece->color, rookx, rooky - 2);
                    }

                    hasCastled[piece->color] = 1;
                    return 1; 
                }
                break; 
            }
            default:; // piece is unidentified so move is invalid
        } 
    }
    return 0;
}

int isSquareAttacked(Board* board, Color color, int x, int y) {
    for (int i = 0; i < CHESS_DIM; ++i) {
        for (int j = 0; j < CHESS_DIM; ++j) {
            Piece* piece = board->squares[i][j];
            if (piece != NULL && piece->color != color) {
                if (checkChessRules(board, piece, i, j, x, y, 0)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int isKingInCheck(Board* board, Color color) {
    int kingX, kingY;
    for (int i = 0; i < CHESS_DIM; ++i) {
        for (int j = 0; j < CHESS_DIM; ++j) {
            Piece* piece = board->squares[i][j];
            if (piece != NULL && piece->type == KING && piece->color == color) {
                kingX = i;
                kingY = j;
                break;
            }
        }
    }
    return isSquareAttacked(board, color, kingX, kingY);
}

int isCheckmateOrStalemate(Board* board, Color color) {
    int kingInCheck = isKingInCheck(board, color);
    for (int x1 = 0; x1 < CHESS_DIM; ++x1) {
        for (int y1 = 0; y1 < CHESS_DIM; ++y1) {
            Piece* piece = board->squares[x1][y1];
            if (piece != NULL && piece->color == color) {
                for (int x2 = 0; x2 < CHESS_DIM; ++x2) {
                    for (int y2 = 0; y2 < CHESS_DIM; ++y2) {
                        if (checkChessRules(board, piece, x1, y1, x2, y2, 0)) {
							Piece* defendingPiece = clonePiece(board->squares[x2][y2]);
							int foundEscape = 0;
							movePiece(board, x1, y1, x2, y2); // run the move, will roll back later
							if (!isKingInCheck(board, color)) {
								foundEscape = 1;
                                char bef[3], aft[3];
                                arrayToNotation(x1, y1, bef);
                                arrayToNotation(x2, y2, aft);
                                if (kingInCheck)
                                    printf("One valid move hint that gets you out of check: %c%c at %s to %s\n",
                                           colorToChar(piece->color), pieceToChar(piece->type), bef, aft);							
							}
							movePiece(board, x2, y2, x1, y1); // reverse the move, restore the defending piece if not null
							if (defendingPiece != NULL) {
								initializePieceAtSquare(board, defendingPiece->type, defendingPiece->color, x2, y2);
                                free(defendingPiece);
                            }
                        	if (foundEscape)
                        		return 0;
                        }
                    }
                }
            }
        }
    }
    return kingInCheck ? 2 : 1;
}

int checkForDiscoveredCheck(Board* board, int bx, int by, int ax, int ay, int* movesPlayed) {
	int foundDiscovery = 0;
	Piece* defendingPiece = clonePiece(board->squares[ax][ay]);
	movePiece(board, bx, by, ax, ay); // run the move, will roll back later
	if (isKingInCheck(board, *movesPlayed % 2 == 1)) {
		foundDiscovery = 1;
	}

	movePiece(board, ax, ay, bx, by); // reverse the move, restore the defending piece if not null
	if (defendingPiece != NULL)
		initializePieceAtSquare(board, defendingPiece->type, defendingPiece->color, ax, ay);
	free(defendingPiece);
	return foundDiscovery;
}

void promotePawn(Board* board, char* input) {
	int x = board->lastMove->ax;
	int y = board->lastMove->ay;
	if (strncmp(input, "Queen", 1) == 0)
		board->squares[x][y]->type = QUEEN;
	else if (strncmp(input, "Knight", 1) == 0)
		board->squares[x][y]->type = KNIGHT;
	else if (strncmp(input, "Rook", 1) == 0)
		board->squares[x][y]->type = ROOK;
	else if (strncmp(input, "Bishop", 1) == 0)
		board->squares[x][y]->type = BISHOP;
}

int validateAndRunMove(Board* board, char* before, char* after, int* movesPlayed) {
    int bx = 0, by = 0, ax = 0, ay = 0;
    notationToArray(before, &bx, &by);
    notationToArray(after, &ax, &ay);
    if (before[0] < 'a' || before[0] > 'h' ||
    	before[1] < '1' || before[1] > '8' ||
    	after[0] < 'a' || after[0] > 'h' ||
    	after[1] < '1' || after[1] > '8') {
    	printf("Error, Move is out of bounds of the board.\n");
    	return 0;
	}
    if (board->squares[bx][by] == NULL) {
        printf("starting square is empty. Invalid move\n");
        return 0;
    }
    if (*movesPlayed % 2 == 0) {
        if (board->squares[bx][by]->color != WHITE) {
            printf("Error, it should be white's turn.\n");
            return 0;
        }
    }
    else {
        if (board->squares[bx][by]->color != BLACK) {
            printf("Error, it should be black's turn.\n");
            return 0;
        }
    }
    
    int status = checkChessRules(board, board->squares[bx][by], bx, by, ax, ay, 1);
    if (status == 0)
        printf("Error, move violates chess rules.\n");
    else {
    	if (checkForDiscoveredCheck(board, bx, by, ax, ay, movesPlayed))
    		return 0;

        movePiece(board, bx, by, ax, ay);
        board->lastMove->piece = board->squares[ax][ay];
        board->lastMove->bx = bx;
        board->lastMove->by = by;
        board->lastMove->ax = ax;
        board->lastMove->ay = ay;
        int checkformate = isCheckmateOrStalemate(board, *movesPlayed % 2 == 0);
        if (checkformate == 2) {
            printf("CHECKMATE!!!!\n");
            gameOver = 1;
        }
        else if (checkformate == 1) {
            printf("STALEMATE!!!\n");
            gameOver = 1;
        }
        else if (isKingInCheck(board, *movesPlayed % 2 == 0)) {
            printf("Check!\n");
        }

        if (board->lastMove->piece->type == PAWN && 
          ((board->lastMove->ax == 7 && board->lastMove->piece->color == WHITE) ||
           (board->lastMove->ax == 0 && board->lastMove->piece->color == BLACK))) {
        	printf("Pawn promotion!\n");
        	printf("What piece would you like to promote your pawn to?\n");
            printf("(Type Queen, Knight, Rook, or Bishop):\n");
        	char input[MAX_INPUT_LENGTH];
            char filename[MAX_FILENAME_LENGTH];
        	int collectCode = collectInput(input, filename);
        	while (collectCode != 11) {
        		printf("Did not understand input, try again.\n");
                printf("(Type Queen, Knight, Rook, or Bishop):\n");
        		collectCode = collectInput(input, filename);
        	}
        	
        	promotePawn(board, input);
        }
    }
    return status;
}

int validateInput(Board* board, char* input) {
    if (strlen(input) < 5) {
        char* lan = convertToLongAlgebraicNotation(board, input);
        if (lan != NULL)
            strcpy(input, lan);
        else {
            printf("Couldn't validate move. Try again using Long Algebraic Notation (input was: %s)\n", input);
            return -1;
        }
    }

    char* beforeMove; 
    char* afterMove;
    beforeMove = strtok(input, "=");
    if (beforeMove != NULL) {
        afterMove = strtok(NULL, "=");
        if (afterMove != NULL)
            return validateAndRunMove(board, beforeMove, afterMove, &board->turnCounter);
        else
    		printf("Error: Move is missing = token.\n");
    }

    return -1;
}

void readMovesFromFile(Board* board, const char* filename, int* movesPlayed, char* gameHistory, int unicodeSupported) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return;
    }

    char line[MAX_INPUT_LENGTH];
    while (fgets(line, MAX_INPUT_LENGTH, file) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        char moveCopy[6];
        strcpy(moveCopy, line);
        char* beforeMove = strtok(line, "=");
        char* afterMove = strtok(NULL, "=");
        
        if (beforeMove != NULL && afterMove != NULL) {
            int bx = -1, by = -1, ax = -1, ay = -1;
            notationToArray(beforeMove, &bx, &by);
            notationToArray(afterMove, &ax, &ay);
            if (validateAndRunMove(board, beforeMove, afterMove, movesPlayed)) {
                printf("\n+--------------------------------------------+\n"
                         "| Move # %d (%s's turn): %s at %s to %s"
                       "\n+--------------------------------------------+\n", 
                       (*movesPlayed / 2) + 1, (*movesPlayed % 2 == 0) ? "White" : "Black", 
                       pieceToName(board->squares[ax][ay]->type), beforeMove, afterMove);
                appendMoveToHistory(gameHistory, moveCopy);
                printBoard(board, unicodeSupported);
                ++*movesPlayed;
            } else {
                printf("Invalid move found in the file: %s=%s\n", beforeMove, afterMove);
                printf("Total moves played until invalid move: %d\n", *movesPlayed);
                return;
            }
        }
    }
    fclose(file);
}

void printSizes() {
	printf("Size of input buffer: %d\n", MAX_INPUT_LENGTH);
    printf("Size of filename buffer: %d\n", MAX_FILENAME_LENGTH);
    printf("Size of game history: %d\n", MAX_MOVE_CHARS);
   #ifdef _WIN32
    printf("Size of chess board: %lld\n", sizeof(Board));
    printf("Size of piece: %lld\n", sizeof(Piece));
    printf("Total (32 pieces): %lld\n", MAX_INPUT_LENGTH + MAX_FILENAME_LENGTH + sizeof(Board) +
                          MAX_MOVE_CHARS + (32 * sizeof(Piece)));
   #else
    printf("Size of chess board: %ld\n", sizeof(Board));
    printf("Size of piece: %ld\n", sizeof(Piece));
    printf("Total (32 pieces): %ld\n", MAX_INPUT_LENGTH + MAX_FILENAME_LENGTH + sizeof(Board) +
                          MAX_MOVE_CHARS + (32 * sizeof(Piece)));
   #endif
}

void printHelp() {
	printf("save <filename> - save the file to a .bin file\n");
	printf("load <filename> - load the file from a .bin file\n");
	printf("print <filename> - print the game history to a .txt file\n");
	printf("read <filename> - load the game history from a .txt file\n");
	printf("reset - reset the current board state to white's first move\n");
	printf("sizes - print the sizes of each storage type in the program\n");
	printf("help - print this help menu to describe other commands\n");
	printf("exit - exit the program\n");
}

void print_ascii_art_intro(const char* filename) {
    printf("\n");
    printf(" TTTTT EEEEE RRRRR  M   M IIIII N   N   A   L\n");
    printf("   T   E     R   R  MM MM   I   NN  N  A A  L\n");
    printf("   T   EEEEE RRRRR  M M M   I   N N N AAAAA L\n");
    printf("   T   E     R R    M   M   I   N  NN A   A L\n");
    printf("   T   EEEEE R   R  M   M IIIII N   N A   A LLLLL\n\n");
    printf(" CCCCC H   H EEEEE SSSSS SSSSS\n");
    printf(" C     H   H E     S     S\n");
    printf(" C     HHHHH EEEEE SSSSS SSSSS\n");
    printf(" C     H   H E         S     S\n");
    printf(" CCCCC H   H EEEEE SSSSS SSSSS\n");
    FILE* artfile = fopen(filename, "r");

    if (artfile == NULL) {
        printf("\n    .::.          \n"
       "    _::_           \n"
       "  _/____\\_        ()       \n"
       "  \\      /      <~~~~>\n"
       "   \\____/        \\__/         <>_\n"
       "   (____)       (____)      (\\)  )                        __/\"\"\"\\\n"
       "    |  |         |  |        \\__/      WWWWWW            ]___ 0  }\n"
       "    |__|         |  |       (____)      |  |       __        /    }\n"
       "   /    \\        |__|        |  |       |  |      (  )     /~    }\n"
       "  (______)      /____\\       |__|       |__|       ||      \\____/\n"
       " (________)    (______)     /____\\     /____\\     /__\\     /____\\\n"
       " /________\\   (________)   (______)   (______)   (____)   (______)\n\n");
        return;
    }

    char ch;
    while ((ch = fgetc(artfile)) != EOF) {
        putchar(ch);
    }

    fclose(artfile);
}

void chessMain() {
    print_ascii_art_intro("knight.txt");
    char input[MAX_INPUT_LENGTH];
    char filename[MAX_FILENAME_LENGTH];
    Board* board = createBoard();
    setupBoardStandard(board);
    int loop = 1; // 4 bytes
    int validationResult; // 4 bytes
    char gameHistory[MAX_MOVE_CHARS] = ""; // 2048 bytes total
    int unicodeSupported = doesTerminalSupportUnicode();

    printBoard(board, unicodeSupported);

    while (loop == 1) {
        int collectCode = collectInput(input, filename);
        if (collectCode == -1 || collectCode == 1) loop = 0;
        if (collectCode == 2) {
           saveBoard(board, filename);
           printf("Board saved.\n");
        }
        if (collectCode == 3) {
            loadBoard(board, filename);
            if (board != NULL) {
                printf("Board loaded.\n");
                printBoard(board, unicodeSupported);
            }
            else
                printf("Failed to load board.\n");
        }

        if (collectCode == 4) {
           #if _WIN32
            printf("Game History (length: %lld):\n%s", strlen(gameHistory), gameHistory);
           #else
            printf("Game History (length: %ld):\n%s", strlen(gameHistory), gameHistory);
           #endif 
            FILE* file = fopen(filename, "w");
            fputs(gameHistory, file);
            fclose(file);
        }

        if (collectCode == 5)
            readMovesFromFile(board, filename, 
             				  &board->turnCounter, gameHistory, unicodeSupported);
        if (collectCode == 6) {
        	resetBoard(board, unicodeSupported);
        }

        if (collectCode == 7) printSizes();
        if (collectCode == 8) printHelp();

        if (collectCode == 0) {
	        char input_cpy[6] = "";
	        strcpy(input_cpy, input);
	        validationResult = validateInput(board, input); // destructive of input
	        if (validationResult == -1)
	            continue;
	        else if (validationResult == 1) {
	            if (board->turnCounter < 300)
	                appendMoveToHistory(gameHistory, input_cpy);
	            printBoard(board, unicodeSupported);
	            ++board->turnCounter;

	            if (gameOver == 1) {
	                printf("Game over: %s wins!\n", colorToName(board->turnCounter % 2 == 0));
	                printf("Would you like to keep playing?\n");
	                do {
	                	collectCode = collectInput(input, filename);
	            	} while (collectCode != 10 && collectCode != 9 && 
	            		     collectCode != 1  && collectCode != -1);
	            	if (collectCode == 9) {
	            		resetBoard(board, unicodeSupported);
	            		continue;
	            	}
	            	else
	                	break;
	            }
	        }
    	}
    }
    printf("Exiting...\n");
    destroyBoard(board);
}

int main() {
    chessMain();
    return 0;
}