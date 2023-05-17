#include "ChessBoard.h"
#include "Chess.h"
#include <stdlib.h>
#include <stdio.h>

Board* createBoard() {
    Board* board = (Board*)malloc(sizeof(Board));
    board->turnCounter = 0;
    for (int i = 0; i < CHESS_DIM; ++i) {
        for (int j = 0; j < CHESS_DIM; ++j)
            board->squares[i][j] = NULL;
    }
    board->lastMove = (Move*)malloc(sizeof(Move));
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
        printf("| Black is winning by material 1 point       |");
    else if (wscore - bscore > 1 && wscore - bscore < 10)
        printf("| Black is winning by material %d points      |", 
               wscore - bscore);
    else if (wscore - bscore >= 10)
        printf("| Black is winning by material %d points     |", 
               wscore - bscore);
    else if (bscore - wscore == 1)
        printf("| White is winning by material 1 point       |");
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

void resetBoard(Board* board, int unicodeSupported) {
    destroyBoard(board);
    board = createBoard();
    setupBoardStandard(board);
    printf("Board was reset.\n");
    board->turnCounter = 0;
    hasCastled[0] = 0;
    hasCastled[1] = 0;
    for (int i = 0; i < 10; ++i)
        takenPiecesCounter[i] = 0;
    printBoard(board, unicodeSupported);
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