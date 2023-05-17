#pragma once
#include "Chess.h"

char* convertToLongAlgebraicNotation(Board* board, char* input);
void notationToArray(char* notation, int* r, int* c);
void arrayToNotation(int r, int c, char* notation);