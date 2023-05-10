#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <vector>
#include <memory>

class King;
class Piece;

// This class governs actions that change the state of the chess board

class Board {
    int rows = 8, cols = 8;
    std::unique_ptr<Piece> boardState[8][8];
    std::vector<Piece *> whitePieces;
    std::vector<Piece *> blackPieces;
    King *wKing = nullptr;
    King *bKing = nullptr;
    bool whiteKingInCheck = false;
    bool blackKingInCheck = false;
    std::string lastMove;
    bool settingUp = false;
    bool promotePawn = false;

public:
    Board();
    std::pair<int, int> coorToRC(std::string coords) const; // string coords to {row, col}, e.g. "e4" -> {4, 5}
    std::string rcToCoor(int row, int col) const; // row,col to string coords, e.g. 4,5 -> "e4"
    bool isValidCoor(std::string coords) const; // returns true if the coordinates refer to a valid tile on the board
    char getPieceType(int row, int col) const; // returns the type of a piece at a specified location
    Piece *getPieceAt(int row, int col) const; // returns the piece* at a specified location
    void setupDefault(); // sets up the board in the default setup
    void clearBoard(); // removes all pieces from the board (replace with empty pieces)

    void addPiece(char type, std::string coords); // adds a piece to the chess board
    void addPiece(char type, std::string coords, bool hasPieceMoved); // adds piece to the board then sets moved status
    void removePiece(int r, int c); // removes a piece from the chess board
    void removePiece(std::string coords); // calls removePiece(int r, int c) after converting coordinates

    void putInSetup(); // board is setting up pieces before a game
    void LeaveSetup(); // board is no longer setting up pieces
    bool canLeaveSetup(); // returns true if setup is valid, false if otherwise
    bool isInSetup(); // returns true if the board is in setup mode, false otherwise


    // makeMove makes a move on the board if valid, throws an exception otherwise
    // if the intended move includes pawn promotion, use pawnPromotion instead
    void makeMove(std::string player, std::string start, std::string final);
    // canMove returns whether a move can occur - does not provide reason for failure
    // do not use in conjunction with canMove - makeMove also checks whether moving is valid but
    // throws an exception with the reason for failure in the case that it cannot
    bool canMove(std::string player, std::string start, std::string final) const;
    // canCheckKing returns true if the piece can check the specified king after a potential move
    // the piece does not have to currently be in that location
    // type is the type of the piece being checked
    // kingcol and kingrow are the coordinates of the king in question
    // typerow and typecol are the coordinates of the piece being checked
    // emptyrow and emptycol are the coordinates of the tile from which the last piece would have moved
    // the last piece that moved can be the piece being checked
    // occupiedrow and occupiedcol are the the coordinates of the tile the last piece would have moved to
    bool canCheckKing(char type, int kingcol, int kingrow, int typerow, int typecol, int emptyrow, int emptycol, int occupiedrow, int occupiedcol) const;
    // isKingInCheck returns true if a potential move puts a king in check
    // colour refers to which king is potentially in check
    // type is the type of the piece that potentially moved
    // emptyrow and emptycol are the coordinates of the tile from which the last piece would have moved
    // occupiedrow and occupiedcol are the the coordinates of the tile the last piece would have moved to
    bool isKingInCheck(std::string colour, char type, int emptyrow, int emptycol, int occupiedrow, int occupiedcol) const;
    // checks if opposing king is in check AFTER MOVE
    bool isOpposingKingInCheck(std::string player, std::string start, std::string final) const;
    bool isCheck(std::string player) const; // returns true if player's king is in check, false otherwise
    bool isCheckmate(std::string player) const; // returns true if player's king is in checkmate, false otherwise
    bool isStalemate(std::string player) const; // returns true if the game is in a stalemate, false otherwise
    bool hasWhiteKing() const; // returns true if there is a white king on the board, false otherwise
    bool hasBlackKing() const; // returns true if there is a black king on the board, false otherwise
    // pawnPromotion makes a move that promotes a pawn to a R, N, B, or Q if valid, throws an exception otherwise
    void pawnPromotion(std::string player, std::string start, std::string dest, char newPiece);
    // castle performs a castling move if valid, throws an exception otherwise
    void castle(std::string player, std::string start, std::string dest);
    // canCastle returns whether a castling move occur - does not provide reason for failure
    // do not use in conjunction with castle - castle also checks whether castling is valid but
    // throws an exception with the reason for failure in the case that it cannot
    bool canCastle(std::string player, std::string start, std::string dest) const;
    // canCaptureEnPassant returns true if pawn can capture en passant
    // assumes the piece at start is a pawn
    bool canCaptureEnPassant(std::string player, std::string start, std::string dest) const;
    std::vector<Piece *> getWhitePieces() const; // returns Board's white pieces
    std::vector<Piece *> getBlackPieces() const; // returns Board's black pieces
    Piece *getWhiteKing() const; // returns Board's white king
    Piece *getBlackKing() const; // returns Board's black king
    ~Board();
};

#endif
