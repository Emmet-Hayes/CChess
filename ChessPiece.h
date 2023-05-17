#pragma once

#define CHESS_DIM 8

typedef enum { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING } PieceType;
typedef enum { WHITE, BLACK } Color;

typedef struct {
    PieceType type;
    Color color;
    int x, y;
    int hasMoved;
} Piece;

Piece* clonePiece(Piece* original);
char pieceToChar(PieceType p);
char colorToChar(Color c);
const char* pieceToName(PieceType p);
const char* colorToName(Color c);
const char* getPieceUnicode(PieceType t, Color c);