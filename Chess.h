#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHESS_DIM 8
#define NUM_PIECES 32
#define MAX_INPUT_LENGTH 6
#define MAX_MOVE_CHARS 2048

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
} LastMove;

typedef struct {
    Piece* squares[CHESS_DIM][CHESS_DIM];
    int turnCounter;
    LastMove* lastMove;
} Board;

Board* createBoard();
void destroyBoard(Board* board);
void setupBoardStandard(Board* board);
void initializePieceAtSquare(Board* board, PieceType type, Color color, int i, int j);
char pieceToChar(PieceType p);
char colorToChar(Color c);
char* pieceToName(PieceType p);
char* colorToName(Color c);
void printBoard(Board* board);
void saveBoard(Board* board, const char* filename);
void loadBoard(Board* board, const char* filename);
void notationToArray(char* notation, int* r, int* c);
void arrayToNotation(int r, int c, char* notation);
void appendMoveToHistory(char* history, const char* move);
int collectInput(char* input);
void incrementCaptureCounter(PieceType p, Color c);
int isSquareAttacked(Board* board, Color color, int x, int y);
int isKingInCheck(Board* board, Color color);
int isCheckmate(Board* board, Color color);
void movePiece(Board* board, int bx, int by, int ax, int ay);
int validateAndRunMove(Board* board, char* before, char* after, int* movesPlayed);
int validateInput(char* input, Board* board);
void readMovesFromFile(Board* board, const char* filename, int* movesPlayed, char* gameHistory);
void chessMain();