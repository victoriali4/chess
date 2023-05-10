#ifndef PIECE_H
#define PIECE_H

#include <vector>
#include <string>

// This class is an abstract superclass for all types of chess pieces, including the empty piece

class Piece {
    protected:
    int currrow;
    int currcol;
    char type;
    bool isWhite;
    bool hasMoved;

    public:
    explicit Piece(char type): type{type} {
        if (type >= 'a') isWhite = false; // set colour
        else isWhite = true;
        hasMoved = false;
    };
    // canMove checks if piece can move to a target position regardless of if pieces are in the way
        // assumed that row and col are valid coordinates
    virtual bool canMove(int row, int col) = 0;
    // getPath returns a path to get to a location as vector of coordinates
        // first element is current position, last element is target location
        // assumed that row and col are valid coordinates
    virtual std::vector<std::pair<int, int>> getPath(int row, int col) const = 0;
    // move moves piece to a new location
        // assumed that row and col are valid coordinates
    void move(int row, int col);
    bool pieceHasMoved() const ; // returns true if the piece has moved, false otherwise
    // planToStandardCapture indicates that move is intended to capture since some pieces move
    // differently when capturing
    void setPieceMoved(bool status); // sets hasMoved to status
    virtual void planToStandardCapture() = 0;
    void setCoors(char newrow, char newcol); // sets piece's coordinates
    bool isPieceWhite() const; // returns true if piece is white, false otherwise
    char getType() const; // returns type of piece
    std::pair<int, int> getPos() const; // returns coordinates of piece
    virtual ~Piece();
};

// see superclass for method descriptions
class Empty : public Piece {
public:
    explicit Empty(char type): Piece(type) {};
    std::vector<std::pair<int, int>> getPath(int row, int col) const override; // returns empty vector
    bool canMove(int row, int col) override; // returns false
    void planToStandardCapture() override {}
    ~Empty() {}
};

class Pawn : public Piece {
    bool intentToCapture;
public:
    explicit Pawn(char type): Piece(type) {
        intentToCapture = false;
    }
    std::vector<std::pair<int, int>> getPath(int row, int col) const override;
    bool canMove(int row, int col) override;
    void planToStandardCapture() override;
    ~Pawn() {}
};

class King : public Piece {
public:
    explicit King(char type): Piece(type) {}
    std::vector<std::pair<int, int>> getPath(int row, int col) const override;
    bool canMove(int row, int col) override;
    void planToStandardCapture() override {}
    ~King() {}
};

class Queen : public Piece {
public:
    explicit Queen (char type): Piece(type) {}
    std::vector<std::pair<int, int>> getPath(int row, int col) const override;
    bool canMove(int row, int col) override;
    void planToStandardCapture() override {}
    ~Queen() {}
};

class Bishop : public Piece {
public:
    explicit Bishop(char type): Piece(type) {}
    std::vector<std::pair<int, int>> getPath(int row, int col) const override;
    bool canMove(int row, int col) override;
    void planToStandardCapture() override {}
    ~Bishop() {}
};

class Rook : public Piece {
public:
    explicit Rook(char type): Piece(type) {}
    std::vector<std::pair<int, int>> getPath(int row, int col) const override;
    bool canMove(int row, int col) override;
    void planToStandardCapture() override {}
    ~Rook() {}
};

class Knight : public Piece {
public:
    explicit Knight(char type): Piece(type) {}
    std::vector<std::pair<int, int>> getPath(int row, int col) const override;
    bool canMove(int row, int col) override;
    void planToStandardCapture() override {}
    ~Knight() {}
};

#endif
