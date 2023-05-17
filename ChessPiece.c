#include "ChessPiece.h"
#include <stdlib.h>

Piece* clonePiece(Piece* original) {
	if (original == NULL) return NULL;
	Piece* copy = (Piece*)malloc(sizeof(Piece));
	copy->type = original->type;
	copy->color = original->color;
	copy->x = original->x;
	copy->y = original->y;
	copy->hasMoved = original->hasMoved;
	return copy;
}

char pieceToChar(PieceType p) {
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

char colorToChar(Color c) {
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