#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ChessBoard.h"

#define NUM_PIECES 32
#define MAX_INPUT_LENGTH 140
#define MAX_FILENAME_LENGTH 128
#define MAX_MOVE_CHARS 2048

int doesTerminalSupportUnicode();
int checkForUnicodeShellsWindows();
void appendMoveToHistory(char* history, const char* move);
int collectInput(char* input, char* filename);
void incrementCaptureCounter(PieceType p, Color c);
int checkChessRules(Board* board, Piece* piece, int bx, int by, int ax, int ay, int inc_mode);
int isSquareAttacked(Board* board, Color color, int x, int y);
int isKingInCheck(Board* board, Color color);
int isCheckmateOrStalemate(Board* board, Color color);
int validateAndRunMove(Board* board, char* before, char* after, int* movesPlayed);
int validateInput(Board* board, char* input);
void readMovesFromFile(Board* board, const char* filename, 
                       int* movesPlayed, char* gameHistory, int unicodeSupported);
void chessMain();