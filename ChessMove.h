#pragma once
#include "ChessPiece.h"

typedef struct {
    Piece* piece;
    int bx, by;
    int ax, ay; 
} Move;