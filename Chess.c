#include "Chess.h"
#include <locale.h>

#ifdef _WIN32
	#define OS_NAME "Windows"
	#include <windows.h>
	#include <tlhelp32.h>
#elif __unix__
	#define OS_NAME "Unix-based"
#else
	#define OS_NAME "Unknown"
#endif

Board* createBoard() {
    Board* board = (Board*)malloc(sizeof(Board));
    board->turnCounter = 0;
    for (int i = 0; i < CHESS_DIM; ++i) {
        for (int j = 0; j < CHESS_DIM; ++j)
            board->squares[i][j] = NULL;
    }
    board->lastMove = (LastMove*)malloc(sizeof(LastMove));
    board->lastMove->piece = NULL;
    board->lastMove->bx = -1;
    board->lastMove->by = -1;
    board->lastMove->ax = -1;
    board->lastMove->ay = -1;
    return board;
}


void destroyBoard(Board* board) {
    for (int i = 0; i < CHESS_DIM; ++i) {
        for (int j = 0; j < CHESS_DIM; ++j)
            free(board->squares[i][j]);
    }
    free(board->lastMove);
    free(board);
}


void setupBoardStandard(Board* board) {
    if (board != NULL) {
        initializePieceAtSquare(board, ROOK, WHITE, 0, 0);
        initializePieceAtSquare(board, KNIGHT, WHITE, 0, 1);
        initializePieceAtSquare(board, BISHOP, WHITE, 0, 2);
        initializePieceAtSquare(board, QUEEN, WHITE, 0, 3);
        initializePieceAtSquare(board, KING, WHITE, 0, 4);
        initializePieceAtSquare(board, BISHOP, WHITE, 0, 5);
        initializePieceAtSquare(board, KNIGHT, WHITE, 0, 6);
        initializePieceAtSquare(board, ROOK, WHITE, 0, 7);
        for (int i = 0; i < CHESS_DIM; ++i) {
            initializePieceAtSquare(board, PAWN, WHITE, 1, i);
            initializePieceAtSquare(board, PAWN, BLACK, 6, i);
        }
        initializePieceAtSquare(board, ROOK, BLACK, 7, 0);
        initializePieceAtSquare(board, KNIGHT, BLACK, 7, 1);
        initializePieceAtSquare(board, BISHOP, BLACK, 7, 2);
        initializePieceAtSquare(board, QUEEN, BLACK, 7, 3);
        initializePieceAtSquare(board, KING, BLACK, 7, 4);
        initializePieceAtSquare(board, BISHOP, BLACK, 7, 5);
        initializePieceAtSquare(board, KNIGHT, BLACK, 7, 6);
        initializePieceAtSquare(board, ROOK, BLACK, 7, 7);
    }
}


void initializePieceAtSquare(Board* board, PieceType type, Color color, int i, int j) {
    board->squares[i][j] = (Piece*)malloc(sizeof(Piece));
    board->squares[i][j]->type = type;
    board->squares[i][j]->color = color;
    board->squares[i][j]->x = i;
    board->squares[i][j]->y = j;
    board->squares[i][j]->hasMoved = 0;
}


const char pieceToChar(PieceType p) {
    switch(p) {
        case PAWN: return 'p';
        case KNIGHT: return 'N';
        case BISHOP: return 'B';
        case ROOK: return 'R';
        case QUEEN: return 'Q';
        case KING: return 'K';
        default: return '-';
    }
}


const char colorToChar(Color c) {
    switch(c) {
        case WHITE: return 'W';
        case BLACK: return 'B';
        default: return '-';
    }
}

const char* pieceToName(PieceType p) {
    switch(p) {
        case PAWN: return "Pawn";
        case KNIGHT: return "Knight";
        case BISHOP: return "Bishop";
        case ROOK: return "Rook";
        case QUEEN: return "Queen";
        case KING: return "King";
        default: return "None";
    }
}

const char* colorToName(Color c) {
    switch(c) {
        case WHITE: return "White";
        case BLACK: return "Black";
        default: return "None";
    }
}

const char* getPieceUnicode(PieceType t, Color c) {
    switch(t) {
        case KING: default: return c == WHITE ? "\u265A" : "\u2654";
        case QUEEN: return c == WHITE ? "\u265B" : "\u2655";
        case ROOK: return c == WHITE ? "\u265C" : "\u2656";
        case BISHOP: return c == WHITE ? "\u265D" : "\u2657";
        case KNIGHT: return c == WHITE ? "\u265E" : "\u2658";
        case PAWN: return c == WHITE ? "\u265F" : "\u2659";
    }
}


int doesTerminalSupportUnicode() {
    char* term = getenv("TERM");
    int unicodeSupported = 0;
    if (term != NULL) {
        setlocale(LC_CTYPE, "");
        if (strstr(term, "xterm") != NULL || strstr(term, "rxvt") != NULL || strstr(term, "linux") != NULL) {
            unicodeSupported = 1;
        }
    }
    if (strcmp(OS_NAME, "Windows") == 0) {
    	unicodeSupported = checkForUnicodeShellsWindows();
    	if (unicodeSupported)
    		SetConsoleOutputCP(CP_UTF8);
    }
    return unicodeSupported;
}

void printBoard(Board* board, int unicodeSupported) {
	if (unicodeSupported) {
    	printf("\n    +---+---+---+---+---+---+---+---+");
    	printf("\n    | a | b | c | d | e | f | g | h |");
    	printf("\n+---+---+---+---+---+---+---+---+---+\n");
	}
	else {
    	printf("\n     +----+----+----+----+----+----+----+----+");
    	printf("\n     | a  | b  | c  | d  | e  | f  | g  | h  |");
    	printf("\n+----+----+----+----+----+----+----+----+----+\n");
    }
    for (int i = CHESS_DIM - 1; i >= 0; --i) {
        if (unicodeSupported) printf("| %d ", (i + 1));
        else printf("| %d  ", (i + 1));
        for (int j = 0; j < CHESS_DIM; ++j) {
            if (j == 0) printf("| ");
            if (board->squares[i][j] != NULL) {
               // new unicode special chars
                if (unicodeSupported) {
                    // causes tofu boxes if the terminal does support unicode, but doesnt have the char in the font
                    printf("%s | ", getPieceUnicode(board->squares[i][j]->type, 
                                                         board->squares[i][j]->color));
                } else {
                    // Terminal does not support Unicode
                    printf("%c%c | ", colorToChar(board->squares[i][j]->color),
                                      pieceToChar(board->squares[i][j]->type));
                }
            }
            else if ((i + j) % 2 == 0) {
            	if (unicodeSupported)
            		printf("- | ");
            	else
                	printf("-- | ");
            }
            else {
            	if (unicodeSupported)
                	printf("  | ");
                else
                	printf("   | ");
            }
        }
        if (unicodeSupported && i == 0) 
        	printf("\n+---+---+---+---+---+---+---+---+---+--------+\n");
        else if (unicodeSupported)
        	printf("\n+---+---+---+---+---+---+---+---+---+\n");
        else 
        	printf("\n+----+----+----+----+----+----+----+----+----+\n");
    }
    printf("| Captured Pieces:                           |");
    printf("\n+--------------------------------------------+\n");
    printf("| White: ");

    for (int i = 0; i < 5; ++i) {
    	if (unicodeSupported) printf("%s: %d ", getPieceUnicode((PieceType)i, WHITE), takenPiecesCounter[i]);
        else printf("%c: %d ", pieceToChar((PieceType)i), takenPiecesCounter[i]);
    }

    int wscore = takenPiecesCounter[0];
    wscore += takenPiecesCounter[1] * 3;
    wscore += takenPiecesCounter[2] * 3;
    wscore += takenPiecesCounter[3] * 5;
    wscore += takenPiecesCounter[4] * 9; 
    printf("           |");
    printf("\n+--------------------------------------------+\n");
    printf("| Black: ");
    for(int i = 5; i < 10; ++i) {
    	if (unicodeSupported) printf("%s: %d ", getPieceUnicode((PieceType)(i - 5), BLACK), takenPiecesCounter[i]);
        else printf("%c: %d ", pieceToChar((PieceType)(i - 5)), takenPiecesCounter[i]);
    }

    int bscore = takenPiecesCounter[5];
    bscore += takenPiecesCounter[6] * 3;
    bscore += takenPiecesCounter[7] * 3;
    bscore += takenPiecesCounter[8] * 5;
    bscore += takenPiecesCounter[9] * 9;
    printf("           |\n");
    printf("+--------------------------------------------+\n");
    if (wscore - bscore == 1)
        printf("| Black is winning by material 1 point      |");
    else if (wscore - bscore > 1 && wscore - bscore < 10)
        printf("| Black is winning by material %d points     |", 
               wscore - bscore);
    else if (wscore - bscore >= 10)
        printf("| Black is winning by material %d points    |", 
               wscore - bscore);
    else if (bscore - wscore == 1)
        printf("| White is winning by material 1 point        |");
    else if (bscore - wscore > 1 && bscore - wscore < 10)
        printf("| White is winning by material %d points      |", 
               bscore - wscore);
    else if (bscore - wscore >= 10)
        printf("| White is winning by material %d points     |", 
               bscore - wscore);
    else
        printf("| Material is even.                          |");
    printf("\n+--------------------------------------------+\n");
    printf("\n");
}


void saveBoard(Board* board, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    fwrite(board, sizeof(Board), 1, file);
    fclose(file);
}


void loadBoard(Board* board, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return;
    }

    if (board == NULL) {
        printf("Error allocating memory for board.\n");
        fclose(file);
        return;
    }

    fread(board, sizeof(Board), 1, file);
    fclose(file);
    return;
}


void notationToArray(char* notation, int* r, int* c) {
    *c = notation[0] - 'a';
    *r = notation[1] - '1';
}


void arrayToNotation(int r, int c, char* notation) {
    notation[0] = c + 'a';
    notation[1] = r + '1';
    notation[2] = '\0';
}

int checkForUnicodeShellsWindows() {
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

    return 0;
}

void appendMoveToHistory(char* history, const char* move) {
    strncat(history, move, 6);
    strcat(history, "\n");
}


int collectInput(char* input) {
   if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
      printf("chess app status: fgets failed\n");
      return -1;
   }
   if (strncmp(input, "save", 4) == 0) return 2;
   if (strncmp(input, "load", 4) == 0) return 3;
   if (strncmp(input, "print", 5) == 0)  return 4;
   if (strncmp(input, "read", 4) == 0) return 5;
   if (strncmp(input, "reset", 5) == 0) return 6;
   if (strncmp(input, "exit", 4) == 0) return 1;
   if (strcmp(input, "\n") != 0) input[strcspn(input, "\n")] = '\0';
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
    switch (piece->type) {
        case PAWN: {
            if (piece->color == WHITE) {
                if (by == ay && bx == ax - 1 &&
                    board->squares[ax][ay] == NULL)
                    return 1; // normal forward white pawn move
                else if (bx == 1 && bx == ax - 2 &&
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
                else if (bx == 6 && bx == ax + 2 && 
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
                if (board->squares[ax][ay] != NULL) {
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
                    if (board->squares[x][y] != NULL)
                        return 0; // A piece is blocking the way

                    x += xdir;
                    y += ydir;
                }
                if (board->squares[ax][ay] != NULL) {
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
                if (board->squares[ax][ay] != NULL) {
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
            if ((dx == 0 && dy == 1) || (dx == 1 && dy == 0) || (dx == 1 && dy == 1))
                if (board->squares[ax][ay] == NULL || 
                    board->squares[ax][ay]->color != piece->color)
                    return 1; // normal king move
            else if (dx == 0 && dy == 2) {
                if (piece->hasMoved) return 0; //king has already moved too bad!
                int rookx = ax;
                int rooky = ay > by ? 7 : 0;
                int ydir = by < ay ? 1 : -1;
                int y = by + ydir;
                while (y != rooky) {
                    if (board->squares[rookx][y] != NULL || isSquareAttacked(board, piece->color % 2 == 1, rookx, y))
                        return 0; // piece is blocking the way, or middle square is being attacked
                    y += ydir;
                }

                if (board->squares[rookx][rooky]->type != ROOK ||
                    board->squares[rookx][rooky]->hasMoved)
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
                return 1; 
            }
            break; 
        }
        default:; // piece is unidentified so move is invalid
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
                            // Simulate the move by moving the piece to the target position
                            Piece* capturedPiece = board->squares[x2][y2];
                            board->squares[x2][y2] = piece;
                            board->squares[x1][y1] = NULL;

                            // Check if the king is still in check after the move
                            int kingStillInCheck = isKingInCheck(board, color);

                            // Undo the simulated move
                            board->squares[x1][y1] = piece;
                            board->squares[x2][y2] = capturedPiece;

                            // If the king is not in check after the move, then it's not a checkmate
                            if (!kingStillInCheck) {
                                char bef[3], aft[3];
                                arrayToNotation(x1, y1, bef);
                                arrayToNotation(x2, y2, aft);
                                if (kingInCheck)
                                    printf("One valid move that gets you out of check: %c%c at %s to %s\n",
                                           colorToChar(piece->color), pieceToChar(piece->type), bef, aft);
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }
    return kingInCheck ? 2 : 1;
}   


void movePiece(Board* board, int bx, int by, int ax, int ay) {
    if (board->squares[ax][ay] == NULL)
        initializePieceAtSquare(board, board->squares[bx][by]->type, board->squares[bx][by]->color, ax, ay);
    else {
        board->squares[ax][ay]->type = board->squares[bx][by]->type;
        board->squares[ax][ay]->color = board->squares[bx][by]->color;
        board->squares[ax][ay]->hasMoved = 1;
    }
    free(board->squares[bx][by]);
    board->squares[bx][by] = NULL; // always clear the prev square
}


int validateAndRunMove(Board* board, char* before, char* after, int* movesPlayed) {
    int bx = 0, by = 0, ax = 0, ay = 0;
    notationToArray(before, &bx, &by);
    notationToArray(after, &ax, &ay);

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
    }
    return status;
}


int validateInput(char* input, Board* board) {
    char* beforeMove; 
    char* afterMove;

    beforeMove = strtok(input, "=");
    if (beforeMove != NULL) {
        afterMove = strtok(NULL, "=");
        if (afterMove != NULL)
            return validateAndRunMove(board, beforeMove, afterMove, &board->turnCounter);
    }
    return -1;
}


void readMovesFromFile(Board* board, const char* filename, 
					   int* movesPlayed, char* gameHistory, int unicodeSupported) {
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


void chessMain() {
    printf("Welcome to a very tiny chess app\n");
    char input[MAX_INPUT_LENGTH]; // 6 bytes
    Board* board = createBoard(); // 524 bytes total
    setupBoardStandard(board); //
    int loop = 1; // 4 bytes
    int validationResult; // 4 bytes
    char gameHistory[MAX_MOVE_CHARS] = ""; // 2048 bytes total
    int unicodeSupported = doesTerminalSupportUnicode();
    printBoard(board, unicodeSupported);

    while (loop == 1) {
        int collectCode = collectInput(input);
        if (collectCode == -1 || collectCode == 1) loop = 0;
        if (collectCode == 2) {
           saveBoard(board, "savefile.bin");
           printf("Board saved.\n");
        }
        if (collectCode == 3) {
            loadBoard(board, "savefile.bin");
            if (board != NULL) {
                printf("Board loaded.\n");
                printBoard(board, unicodeSupported);
            }
            else
                printf("Failed to load board.\n");
        }

        if (collectCode == 4) {
            printf("Game History (length: %ld):\n%s", strlen(gameHistory), gameHistory);
            FILE* file = fopen("gamehist.txt", "w");
            fputs(gameHistory, file);
            fclose(file);
        }

        if (collectCode == 5)
             readMovesFromFile(board, "gamehist.txt", 
             				   &board->turnCounter, gameHistory, unicodeSupported);

        if (collectCode == 6) {
            destroyBoard(board);
            board = createBoard();
            setupBoardStandard(board);
            printf("Board was reset.\n");
            board->turnCounter = 0;
            for (int i = 0; i < 10; ++i)
                takenPiecesCounter[i] = 0;
            printBoard(board, unicodeSupported);
        }
        char input_cpy[6] = "";
        strcpy(input_cpy, input);
        validationResult = validateInput(input, board); // destructive of input
        if (validationResult == -1)
            continue;
        else if (validationResult == 1) {
            if (board->turnCounter < 300)
                appendMoveToHistory(gameHistory, input_cpy);
            printBoard(board, unicodeSupported);
            ++board->turnCounter;

            if (gameOver == 1) {
                printf("Game over: %s wins!\n", colorToName(board->turnCounter % 2 == 0));
                break;
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