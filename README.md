This chess application is built with love by Emmet Hayes. I have written this program in C from the ground up to be as small as I thought would be possible. 




Anybody with a C compiler and a terminal should be able to build this Chess app on their system. However it unfortunately doesn't look quite as good on Windows, as there are some issues on that platform with displaying the unicode properly in the Command Prompt, Powershell, and other shells available there.


The board state consists of 64 pointers to structures containing the state at that board square, meaning on an 64-bit OS architecture, the board size is a grand total of $64 \cdot 8$ + 16 bytes. The extra 16 bytes account for the pointer to the structure containing the last move (needed for en passant verification), and the move counter (needed to verify move order). 



The mechanism for recording chess moves writes moves played in the game in a well-known notation in the chess community known as "Long Algebraic Notation" (LAN). Here is a brief example of a few moves:

	d2=d4
	g8=f6
	c2=c4
	g7=g6

In this notation, the starting square is specified to the left of the = sign, and the destination square is on the right.

I am currently working on making the input parser capable of reading the more standardly-used by players today call "Algebraic Notation" (AN) or aka "Short Algebraic Notation" (SAN), in which the same sequence of moves could be written as:
	d4
	Nf6
	c4
	g6

Notice the differences in the two notations, we are adding char signifiers in the beginning of the notation to identify the piece to move. Therefore, we need a mechanism in which to calculate the starting square of the knight that moves to f6 based on N. These are the char signifiers for each piece (note that lower-case doesnt work, because then b pawns and bishops become ambiguous in a taking move):
	N = knight
	B = bishop
	R = rook
	Q = queen
	K = king
	(none) = pawn


Note that pawn moves will ALWAYS be written as two characters, and other piece moves will ALWAYS be written as three characters.
That reminds me of another important example in this notation:

	d4
	d5
	Nf3
	Nf6
	e3
	Nbd7

Notice in the last move in this sequence, we had to specify which knight we wanted, which will ALWAYS take four character. Therefore, we need a mechanism in the case there is a hint in the input about which knight to move.