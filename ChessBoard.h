#pragma once
#include "ChessMove.h"

typedef struct {
    Piece* squares[CHESS_DIM][CHESS_DIM];
    int turnCounter;
    Move* lastMove;
} Board;


Board* createBoard();
void destroyBoard(Board* board);
void setupBoardStandard(Board* board);
void initializePieceAtSquare(Board* board, PieceType type, Color color, int i, int j);
void printBoard(Board* board, int unicodeSupported);
void saveBoard(Board* board, const char* filename);
void loadBoard(Board* board, const char* filename);
void resetBoard(Board* board, int unicodeSupported);
void movePiece(Board* board, int bx, int by, int ax, int ay);