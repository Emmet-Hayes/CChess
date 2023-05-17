#include "Chess.h"
#include "ChessParser.h"

char* convertToLongAlgebraicNotation(Board* board, char* input) {
	int len = strlen(input);
	char* longNotation = (char*)malloc(6); // always 6 chars or NULL
    switch (len) {
        case 2: { // pawn moves don't need piece signifier char
        	int dest_col = input[0] - 'a';
    		int dest_row = input[1] - '1';
            int start_row = -1;
            if (dest_col < 0 || dest_col > 7 || dest_row < 0 || dest_row > 7) {
            	free(longNotation);
            	return NULL;
            }
            Color color = board->turnCounter % 2 == 0 ? WHITE : BLACK;
            if (color == WHITE) {
                // Check if it's a two-square move
                if (board->squares[dest_row - 2][dest_col] &&
                    board->squares[dest_row - 2][dest_col]->type == PAWN &&
                    board->squares[dest_row - 2][dest_col]->color == WHITE &&
                    board->squares[dest_row - 1][dest_col] == NULL) {
                    start_row = dest_row - 2;
                }
                // Check if it's a one-square move
                else if (board->squares[dest_row - 1][dest_col] &&
                         board->squares[dest_row - 1][dest_col]->type == PAWN &&
                         board->squares[dest_row - 1][dest_col]->color == WHITE) {
                    start_row = dest_row - 1;
                }
            } else {
                // Check if it's a two-square move
                if (board->squares[dest_row + 2][dest_col] &&
                    board->squares[dest_row + 2][dest_col]->type == PAWN &&
                    board->squares[dest_row + 2][dest_col]->color == BLACK &&
                    board->squares[dest_row + 1][dest_col] == NULL) {
                    start_row = dest_row + 2;
                }
                // Check if it's a one-square move
                else if (board->squares[dest_row + 1][dest_col] &&
                         board->squares[dest_row + 1][dest_col]->type == PAWN &&
                         board->squares[dest_row + 1][dest_col]->color == BLACK) {
                    start_row = dest_row + 1;
                }
                else {
                	free(longNotation);
                	return NULL;
                }
            }

            if (start_row != -1) {
                snprintf(longNotation, 7, "%c%d=%c%d", input[0], start_row + 1, input[0], dest_row + 1);
            	return longNotation;
            } else {
                free(longNotation);
                return NULL;
            }
    }
		case 3: { // non-pawn move, deterministic source square
	        char piece = input[0];
	        int dest_col = input[1] - 'a';
	        int dest_row = input[2] - '1';
            if (dest_col < 0 || dest_col > 7 || dest_row < 0 || dest_row > 7) {
            	free(longNotation);
            	return NULL;
            }
	        PieceType pieceType = piece == 'N' ? KNIGHT : piece == 'B' ? BISHOP : piece == 'R' ? ROOK : piece == 'Q' ? QUEEN : KING;
	        Color color = board->turnCounter % 2 == 0 ? WHITE : BLACK;
	        for (int row = 0; row < CHESS_DIM; row++) {
	            for (int col = 0; col < CHESS_DIM; col++) {
	                Piece *p = board->squares[row][col];
	                if (p && p->type == pieceType && p->color == color) {
	                    int valid_move = checkChessRules(board, p, col, row, dest_col, dest_row, 0);
	                    if (valid_move) {
	                        snprintf(longNotation, 6, "%c%d=%c%d", col + 'a', row + 1, dest_col + 'a', dest_row + 1);
	                        return longNotation;
	                    }
	                }
	            }
	        }

	        // If we reach here, the move is not valid
	        free(longNotation);
	        return NULL;
		}
	    case 4: {
	        // Look for an 'x' to see if it's a capture move
	        if (strchr(input, 'x') != NULL) {
	            // Handle capture moves
	            char piece = input[0];
	            int dest_col = input[2] - 'a';
	            int dest_row = input[3] - '1';
	            if (dest_col < 0 || dest_col > 7 || dest_row < 0 || dest_row > 7) {
	            	free(longNotation);
	            	return NULL;
	            }
	            PieceType pieceType = piece == 'N' ? KNIGHT : 
	            					  piece == 'B' ? BISHOP : 
	            					  piece == 'R' ? ROOK : 
	            					  piece == 'Q' ? QUEEN : 
	            					  KING == 'K' ? KING : PAWN;
	            Color color = board->turnCounter % 2 == 0 ? WHITE : BLACK;

	            if (pieceType == PAWN) {
	            	int src_col = input[0] - 'a';
	                int dest_col = input[2] - 'a';
	                int dest_row = input[3] - '1';
	                if (dest_col < 0 || dest_col > 7 || dest_row < 0 || dest_row > 7) {
            			free(longNotation);
            			return NULL;
            		}
	                Color color = board->turnCounter % 2 == 0 ? WHITE : BLACK;
	                int src_row = color == WHITE ? dest_row - 1 : dest_row + 1;
	                Piece *p = board->squares[src_row][src_col];
	                if (p && p->type == PAWN && p->color == color) {
	                    snprintf(longNotation, MAX_MOVE_CHARS, "%c%d=%c%d", src_col + 'a', src_row + 1, dest_col + 'a', dest_row + 1);
	                    return longNotation;
	                }

	                // If we reach here, the move is not valid
                	free(longNotation);
                	return NULL;
	            }

	            for (int row = 0; row < CHESS_DIM; row++) {
	                for (int col = 0; col < CHESS_DIM; col++) {
	                    Piece *p = board->squares[row][col];
	                    if (p && p->type == pieceType && p->color == color) {
	                    	// the only piece we know about is actually the opposite color
	                    	board->squares[dest_col][dest_row]->color = (p->color == WHITE) ? BLACK : WHITE;
	                        int valid_move = checkChessRules(board, p, col, row, dest_col, dest_row, 0);
	                        if (valid_move) {
	                            snprintf(longNotation, 6, "%c%d=%c%d", col + 'a', row + 1, dest_col + 'a', dest_row + 1);
	                            board->squares[dest_col][dest_row]->color = p->color;
	                            return longNotation;
	                        }
	                    }
	                }
	            }

	            // If we reach here, the move is not valid
	            free(longNotation);
	            return NULL;
	        }
	        // Otherwise, it's a non-pawn move, deterministic source square provided
	        else {
		        char piece = input[0];
		        int disambig_col = input[1] - 'a';
		        int dest_col = input[2] - 'a';
		        int dest_row = input[3] - '1';
		        if (dest_col < 0 || dest_col > 7 || dest_row < 0 || dest_row > 7) {
            		free(longNotation);
            		return NULL;
            	}
		        PieceType pieceType = piece == 'N' ? KNIGHT : piece == 'B' ? BISHOP : piece == 'R' ? ROOK : piece == 'Q' ? QUEEN : KING;
		        Color color = board->turnCounter % 2 == 0 ? WHITE : BLACK;

		        for (int row = 0; row < CHESS_DIM; row++) {
		            Piece *p = board->squares[row][disambig_col];
		            if (p && p->type == pieceType && p->color == color) {
		                int valid_move = checkChessRules(board, p, disambig_col, row, dest_col, dest_row, 0);
		                if (valid_move) {
		                    snprintf(longNotation, 6, "%c%d=%c%d", disambig_col + 'a', row + 1, dest_col + 'a', dest_row + 1);
		                    return longNotation;
		                }
		            }
		        }

		        // If we reach here, the move is not valid
		        free(longNotation);
		        return NULL;
	        }
	    }
	}
	return NULL;
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