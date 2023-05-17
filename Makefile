CC = gcc
CFLAGS = -Wall -Werror
TARGET = Chess
OBJS = Chess.o ChessBoard.o ChessParser.o ChessPiece.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

Chess.o: Chess.c
	$(CC) $(CFLAGS) -c Chess.c

ChessBoard.o: ChessBoard.c
	$(CC) $(CFLAGS) -c ChessBoard.c

ChessParser.o: ChessParser.c
	$(CC) $(CFLAGS) -c ChessParser.c

ChessPiece.o: ChessPiece.c
	$(CC) $(CFLAGS) -c ChessPiece.c

clean:
	rm -f $(TARGET) $(OBJS)
