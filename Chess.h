#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHESS_DIM 8
#define NUM_PIECES 32
#define MAX_INPUT_LENGTH 140
#define MAX_FILENAME_LENGTH 128
#define MAX_MOVE_CHARS 2048

int hasCastled[2] = {0, 0};
int takenPiecesCounter[10] = {0};
int gameOver = 0;

typedef enum { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING } PieceType;
typedef enum { WHITE, BLACK } Color;

typedef struct {
    PieceType type;
    Color color;
    int x, y;
    int hasMoved;
} Piece;

typedef struct {
    Piece* piece;
    int bx, by;
    int ax, ay; 
} Move;

typedef struct {
    Piece* squares[CHESS_DIM][CHESS_DIM];
    int turnCounter;
    Move* lastMove;
} Board;

Board* createBoard();
void destroyBoard(Board* board);
void setupBoardStandard(Board* board);
void initializePieceAtSquare(Board* board, PieceType type, Color color, int i, int j);
char pieceToChar(PieceType p);
char colorToChar(Color c);
const char* pieceToName(PieceType p);
const char* colorToName(Color c);
const char* getPieceUnicode(PieceType t, Color c);
int doesTerminalSupportUnicode();
int checkForUnicodeShellsWindows();
void printBoard(Board* board, int unicodeSupported);
void saveBoard(Board* board, const char* filename);
void loadBoard(Board* board, const char* filename);
void resetBoard(Board* board, int unicodeSupported);
void notationToArray(char* notation, int* r, int* c);
void arrayToNotation(int r, int c, char* notation);
void appendMoveToHistory(char* history, const char* move);
int collectInput(char* input, char* filename);
void incrementCaptureCounter(PieceType p, Color c);
int checkChessRules(Board* board, Piece* piece, int bx, int by, int ax, int ay, int inc_mode);
int isSquareAttacked(Board* board, Color color, int x, int y);
int isKingInCheck(Board* board, Color color);
Piece* clonePiece(Piece* original);
int isCheckmateOrStalemate(Board* board, Color color);
void movePiece(Board* board, int bx, int by, int ax, int ay);
int validateAndRunMove(Board* board, char* before, char* after, int* movesPlayed);
int validateInput(char* input, Board* board);
void readMovesFromFile(Board* board, const char* filename, 
                       int* movesPlayed, char* gameHistory, int unicodeSupported);
void chessMain();