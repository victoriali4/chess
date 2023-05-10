#include "piece.h"
#include <stdlib.h>

using namespace std;

// see piece.h for method descriptions

// PIECE CONCRETE METHODS ////////////////////////////////////////
void Piece::setCoors(char newrow, char newcol) {
    currrow = newrow;
    currcol = newcol;
}

void Piece::move(int row, int col) {
    setCoors(row, col);
    if (!hasMoved) hasMoved = true; 
}

bool Piece::pieceHasMoved() const {
    return hasMoved;
}

void Piece::setPieceMoved(bool status) {
    hasMoved = status;
}

bool Piece::isPieceWhite() const {
    return isWhite;
}

char Piece::getType() const { return type; }

pair<int, int> Piece::getPos() const {
    std::pair<int, int> newpair{currrow, currcol};
    return newpair;
}

Piece::~Piece() {};

vector<pair<int, int>> Empty::getPath(int row, int col) const {
    vector<pair<int, int>> path;
    return path;
}
bool Empty::canMove(int row, int col) {
    return false;
}

// PAWN METHODS ///////////////////////////////////////////////////
vector<pair<int, int>> Pawn::getPath(int row, int col) const {
    vector<pair<int, int>> path;
    path.emplace_back(getPos());
    if (abs(currrow-row) == 2) { // moved two spaces - otherwise only moved one space forward or diagonally
        int middlerow = (getPos().first+row)/2;
        int middlecol = col;
        path.emplace_back(pair<int, int>{middlerow, middlecol});
    }
    path.emplace_back(pair<int, int>{row, col});
    return path;
}        
bool Pawn::canMove(int row, int col) {
    if (isWhite && currrow != 6) hasMoved = true;
    if (!isWhite && currrow != 1) hasMoved = true;
    
    if (intentToCapture) { // capturing move
        if (isWhite && (abs(currcol-col) == 1) && (currrow-row == 1)) { // white moves diagonally up 1 to capture
            intentToCapture = false;
            return true;
        } else if (!isWhite && (abs(currcol-col) == 1) && (currrow-row == -1)) { // black moves diagonally down 1 to capture
            intentToCapture = false;
            return true;       
        } else {
            intentToCapture = false;
            return false;
        }
    } else if (!hasMoved) { // first move
        if (isWhite && (currcol == col) && (currrow == row+1 || currrow == row+2)) { // white moves up 1 or 2
            return true;
        } else if (!isWhite && (currcol == col) && (currrow == row-1 || currrow == row-2)) { // black moves down 1 or 2
            return true;
        } else {
            return false;
        }
    } else { // normal move forward
        if (isWhite) return (currcol == col) && (currrow == row+1); // white moves up 1
        else return (currcol == col) && (currrow == row-1); // black moves down 1
    }
}
void Pawn::planToStandardCapture() {
    intentToCapture = true;
}

// KING METHODS ///////////////////////////////////////////////////
vector<pair<int, int>> King::getPath(int row, int col) const {
    vector<pair<int, int>> path; // king only moves one space in any direction
    path.emplace_back(getPos());
    path.emplace_back(pair<int, int>{row, col});
    return path;
}
bool King::canMove(int row, int col) {
    if (abs(row-currrow)<=1 && abs(col-currcol)<=1 && !(row==currrow && col==currcol)) { // move of 1 in any direction, not the same position
        return true;
    }
    return false;
}

// QUEEN METHODS //////////////////////////////////////////////////
vector<pair<int, int>> Queen::getPath(int row, int col) const {
    vector<pair<int, int>> path;
    path.emplace_back(getPos());   
    int difference = abs(col-currcol); // for diagonal moves
    if (currrow < row && currcol < col) { // moves diagonally towards upper left
        for (int i = 1; i < difference; ++i) {
            path.emplace_back(pair<int, int>{row-i, col-i});
        }
    } else if (currrow < row && currcol > col) { // moves diagonally towards upper right
        for (int i = 1; i < difference; ++i) {
            path.emplace_back(pair<int, int>{row-i, col+i});
        }     
    } else if (currrow > row && currcol < col) { // moves diagonally towards lower left
        for (int i = 1; i < difference; ++i) {
            path.emplace_back(pair<int, int>{row+i, col-i});
        }
    } else if (currrow > row && currcol > col) { // moves diagonally towards lower right
        for (int i = 1; i < difference; ++i) {
            path.emplace_back(pair<int, int>{row+i, col+i});
        }   
    } else if (currrow == row && currcol < col) { // moves horizontally right
        for (int i = col-1; i > currcol; --i) {
            path.emplace_back(pair<int, int>{row, i});
        }
    } else if (currrow == row && col < currcol) { // moves horizontally left
        for (int i = col+1; i < currcol; ++i) {
            path.emplace_back(pair<int, int>{row, i});
        }     
    } else if (currcol == col && currrow < row) { // moves vertically up
        for (int i = row-1; i > currrow; --i) {
            path.emplace_back(pair<int, int>{i, col});
        }
    } else {                                      // moves vertically down
        for (int i = row+1; i < currrow; ++i) {
            path.emplace_back(pair<int, int>{i, col});
        }   
    }
    path.emplace_back(pair<int, int>{row, col});
    return path;
}
bool Queen::canMove(int row, int col) {
    // (diagonal move or horizontal/vertical move) and not the same position
    if ((abs(row-currrow) == abs(col-currcol) || (row==currrow || col==currcol)) && !(row==currrow && col==currcol)) {
        return true;
    }
    return false;
}

// BISHOP METHODS /////////////////////////////////////////////////
vector<pair<int, int>> Bishop::getPath(int row, int col) const {
    vector<pair<int, int>> path;
    path.emplace_back(getPos());
    int difference = abs(col-currcol);
    if (currrow < row && currcol < col) { // moves diagonally towards upper left
        for (int i = 1; i < difference; ++i) {
            path.emplace_back(pair<int, int>{row-i, col-i});
        }
    } else if (currrow < row && currcol > col) { // moves diagonally towards upper right
        for (int i = 1; i < difference; ++i) {
            path.emplace_back(pair<int, int>{row-i, col+i});
        }     
    } else if (currrow > row && currcol < col) { // moves diagonally towards lower left
        for (int i = 1; i < difference; ++i) {
            path.emplace_back(pair<int, int>{row+i, col-i});
        }
    } else {                                      // moves diagonally towards lower right
        for (int i = 1; i < difference; ++i) {
            path.emplace_back(pair<int, int>{row+i, col+i});
        }   
    }
    path.emplace_back(pair<int, int>{row, col});
    return path;
}
bool Bishop::canMove(int row, int col) {
    if (abs(row-currrow) == abs(col-currcol) && !(row==currrow && col==currcol)) { // diagonal move, not the same position
        return true;
    }
    return false;
}

// ROOK METHODS ///////////////////////////////////////////////////
vector<pair<int, int>> Rook::getPath(int row, int col) const {
    vector<pair<int, int>> path;
    path.emplace_back(getPos());
    if (currrow == row && currcol < col) { // moves horizontally right
        for (int i = col-1; i > currcol; --i) {
            path.emplace_back(pair<int, int>{row, i});
        }
    } else if (currrow == row && col < currcol) { // moves horizontally left
        for (int i = col+1; i < currcol; ++i) {
            path.emplace_back(pair<int, int>{row, i});
        }     
    } else if (currcol == col && currrow < row) { // moves vertically up
        for (int i = row-1; i > currrow; --i) {
            path.emplace_back(pair<int, int>{i, col});
        }
    } else {                                      // moves vertically down
        for (int i = row+1; i < currrow; ++i) {
            path.emplace_back(pair<int, int>{i, col});
        }   
    }
    path.emplace_back(pair<int, int>{row, col});
    return path;
}
bool Rook::canMove(int row, int col) {
    if ((row==currrow || col==currcol) && !(row==currrow && col==currcol)) { // horizontal or vertical move, not the same position
        return true;
    }
    return false;
}

vector<pair<int, int>> Knight::getPath(int row, int col) const {
    vector<pair<int, int>> path; // knight skips over other pieces in path
    path.emplace_back(getPos());
    path.emplace_back(pair<int, int>{row, col});
    return path;
}
bool Knight::canMove(int row, int col) {
    if ((abs(row-currrow) == 1 && abs(col-currcol) == 2) || (abs(row-currrow) == 2 && abs(col-currcol) == 1)) { // l-shaped move
        return true;
    }
    return false;
}

