#include "board.h"
#include "piece.h"
#include "boardException.h"
#include <vector>
#include <memory>

using namespace std;

// see board.h for method descriptions

Board::Board() {
    // initialize each tile of the board to an empty piece
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            boardState[r][c] = make_unique<Empty>(' ');
            boardState[r][c]->setCoors(r, c);
        }
    }
}

pair<int, int> Board::coorToRC(string coors) const {
    pair<int, int> newpair;
    try {
        // convert using ascii values
        newpair.second = coors[0] - 'a' + 1;
        newpair.first = coors[1] - '0';
        newpair.first = rows - newpair.first;
        newpair.second = newpair.second - 1;
    } catch (...) {
        // return negative numbers if coors is invalid
        newpair.first = -1;
        newpair.second = -1;
    }
    return newpair;
}

string Board::rcToCoor(int row, int col) const {
    int newrow = rows - row;
    int newcol = col + 1;
    char strcol = newcol + 'a' - 1;
    char strrow = newrow + '0';
    char arr[] = {strcol, strrow};
    string strcoords = arr;
    return strcoords;
}

bool Board::isValidCoor(string coords) const {
    pair<int, int> rc = coorToRC(coords);
    if (!(0<=rc.first && rc.first<=7 && 0<=rc.second && rc.second<=7)) {
        return false;
    }
    return true;
}
char Board::getPieceType(int row, int col) const {
    return boardState[row][col]->getType();
}

Piece* Board::getPieceAt(int row, int col) const {
    if (!(0<=row && row<=7 && 0<=col && col<=7)) {
        throw BoardException("Coordinates out of bounds.");
    }
    return boardState[row][col].get();
}

void Board::setupDefault() {
    addPiece('p', "a7");
    addPiece('p', "b7");
    addPiece('p', "c7");
    addPiece('p', "d7");
    addPiece('p', "e7");
    addPiece('p', "f7");
    addPiece('p', "g7");
    addPiece('p', "h7");
    addPiece('r', "a8");
    addPiece('n', "b8");
    addPiece('b', "c8");
    addPiece('q', "d8");
    addPiece('k', "e8");
    addPiece('b', "f8");
    addPiece('n', "g8");
    addPiece('r', "h8");

    addPiece('P', "a2");
    addPiece('P', "b2");
    addPiece('P', "c2");
    addPiece('P', "d2");
    addPiece('P', "e2");
    addPiece('P', "f2");
    addPiece('P', "g2");
    addPiece('P', "h2");
    addPiece('R', "a1");
    addPiece('N', "b1");
    addPiece('B', "c1");
    addPiece('Q', "d1");
    addPiece('K', "e1");
    addPiece('B', "f1");
    addPiece('N', "g1");
    addPiece('R', "h1");
}


void Board::addPiece(char type, string coors) {
    pair<int, int> rc = coorToRC(coors);

    // error checking for invalid coordinates, attempt to add to an occupied position, invalid action in setup
    if (!isValidCoor(coors)) {
        throw BoardException("Invalid letter-number coordinates3. Try again.");
        return;
    } else if (boardState[rc.first][rc.second]->getType() != ' '){
        throw BoardException("Position is already occupied by another piece. Try again.");
        return;
    } else if (settingUp && type == 'K' && hasWhiteKing()) {
        throw BoardException("White King already exists. Place a different piece, remove this King, or exit.");
    } else if (settingUp && type == 'k' && hasBlackKing()) {
        throw BoardException("Black King already exists. Place a different piece, remove this King, or exit.");
    } else if (settingUp && (type == 'p' || type == 'P') && (coors[1] == '1' || coors[1] == '8')) {
        throw BoardException("Cannot place Pawn in first or last row. Place a different piece or exit.");
    }

    // add a new piece of desired type to the target position
    if (type == 'p' || type == 'P') {
        boardState[rc.first][rc.second] = make_unique<Pawn>(type);
    } else if ((type == 'n' || type == 'N')) {
        boardState[rc.first][rc.second] = make_unique<Knight>(type);
    } else if ((type == 'r' || type == 'R')) {
        boardState[rc.first][rc.second] = make_unique<Rook>(type);
    } else if ((type == 'b' || type == 'B')) {
        boardState[rc.first][rc.second] = make_unique<Bishop>(type);
    } else if ((type == 'q' || type == 'Q')) {
        boardState[rc.first][rc.second] = make_unique<Queen>(type);
    } else if ((type == 'k' || type == 'K')) {
        King *newKing = new King{type};
        boardState[rc.first][rc.second] = std::unique_ptr<Piece>{newKing};
        // set wKing or bKing pointers to indicate that a king has been placed
        if (type == 'K') wKing = newKing; 
        else bKing = newKing;
    } else {
        throw BoardException("Indicated chess piece is not a valid type. Try again.");
        return;    }
    boardState[rc.first][rc.second]->setCoors(rc.first, rc.second); // set coordinates of piece
    // add the piece to the respective colour's vector of pieces
    if (boardState[rc.first][rc.second]->isPieceWhite()) {
        whitePieces.emplace_back(boardState[rc.first][rc.second].get());
    } else {
        blackPieces.emplace_back(boardState[rc.first][rc.second].get());
    }
}

void Board::addPiece(char type, string coords, bool hasPieceMoved) {
    try {
        addPiece(type, coords);
        pair<int, int> rc = coorToRC(coords);
        boardState[rc.first][rc.second]->setPieceMoved(hasPieceMoved);
    } catch (BoardException e) {
        throw BoardException(e.what());
    }
}

void Board::removePiece(int r, int c) {
    char type = boardState[r][c]->getType();
    // check that the location of the piece to remove does not indicate an empty tile
    if (type != ' ') {
        // remove the piece from the respective colour's vector of pieces
        if (boardState[r][c]->isPieceWhite()) {
            for (int i = 0; i < whitePieces.size(); ++i) { // iterate through to find index of piece to remove
                if (whitePieces[i] == boardState[r][c].get()) {
                    whitePieces.erase(whitePieces.begin() + i); // remove from vector
                }
            }
        } else {
            for (int i = 0; i < blackPieces.size(); ++i) {
                if (blackPieces[i] == boardState[r][c].get()) {
                    blackPieces.erase(blackPieces.begin() + i);
                }
            }
        }

        boardState[r][c] = make_unique<Empty>(' ');
        boardState[r][c]->setCoors(r, c);
        // set respective king pointer to null to indicate that there is no king on the board
        if (type == 'K') wKing = nullptr;
        else if (type == 'k') bKing = nullptr;
    } else {
        throw BoardException("There is no piece to remove in this spot.");
    }
}

void Board::removePiece(string coors) {
    pair<int, int> rc = coorToRC(coors);
    // error check for invalid coordinates
    if (!isValidCoor(coors)) {
        throw BoardException("Invalid letter-number coordinates2. Try again.");
    }
    // see removePiece(int r, int c)
    removePiece(rc.first, rc.second);
}

void Board::clearBoard() {
    // replace each non-empty piece with an empty piece
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (boardState[r][c]->getType() == ' ') continue;
            removePiece(r, c);
        }
    }
}

void Board::putInSetup() {
    settingUp = true;
}

void Board::LeaveSetup() {
    settingUp = false;
}

bool Board::canLeaveSetup() {
    return hasWhiteKing() && hasBlackKing() && !isCheck("white") && !isCheck("black") && !isStalemate("white") && !isStalemate("black");
}

bool Board::isInSetup() {
    return settingUp;
}

bool Board::canMove(string player, string start, string final) const {

    // error checking
    if (!isValidCoor(start) || !isValidCoor(final)) {
        return false;
    }

    pair<int, int> startrc = coorToRC(start);
    int startrow = startrc.first;
    int startcol = startrc.second;

    char pieceType = boardState[startrow][startcol]->getType();

    if (pieceType == ' ') {
        return false;
    }
    if (boardState[startrow][startcol]->isPieceWhite() != ((player == "White") || (player == "white"))) {
        return false;
    }
    pair<int, int> destrc = coorToRC(final);
    int destrow = destrc.first;
    int destcol = destrc.second;

    // if piece type and coordinates indicate castle, do a castle
    if (toupper(pieceType) == 'K') {
        if (boardState[startrow][startcol]->isPieceWhite() && start == "e1" && (final == "c1" || final == "g1")) {
            return canCastle(player, start, final);
        } else if (!boardState[startrow][startcol]->isPieceWhite() && start == "e8" && (final == "c8" || final == "g8")) {
            return canCastle(player, start, final);
        }
    }

    // check if en passant
    bool canEnPassant = false;

    if (toupper(boardState[startrow][startcol]->getType()) == 'P') {
        canEnPassant = canCaptureEnPassant(player, start, final);
    }

    // check if moving pawn to end of board without promoting pawn
    if (toupper(pieceType) == 'P' && (final[1] == '1' || final[1] == '8') && !promotePawn) return false;


    // check if capturing own piece
    if (boardState[destrow][destcol]->getType() != ' ') {
        if (boardState[destrow][destcol]->isPieceWhite() == ((player == "White") || (player == "white"))) {
            return false;
        }
    }

    // check if destination is valid
    if (boardState[destrow][destcol]->getType() != ' ') {
        boardState[startrow][startcol]->planToStandardCapture();
    } else if (boardState[destrow][destcol]->getType() == ' ' && toupper(pieceType) == 'P' && canEnPassant) {
        boardState[startrow][startcol]->planToStandardCapture();
    }
    if (!boardState[startrow][startcol]->canMove(destrow, destcol)) {
        return false;
    }

    // check if pieces are in the way
    vector<pair<int, int>> path = boardState[startrow][startcol]->getPath(destrow, destcol);
    for (int i = 1; i < path.size()-1; ++i) {
        if (boardState[path[i].first][path[i].second]->getType() != ' ') {
            return false;
        }
    }

    // check if move puts own king is in check
    if (((player == "White") || (player == "white")) && isKingInCheck("white", pieceType, startrow, startcol, destrow, destcol)) {
        return false;
    } else if (((player == "Black") || (player == "black")) && isKingInCheck("black", pieceType, startrow, startcol, destrow, destcol)) {
        return false;
    }
    return true;
}

void Board::makeMove(string player, string start, string final) {
    if (player == "White" || player == "white") { // opposing king may no longer be in check after the previous move
        blackKingInCheck = false;
    } else if (player == "Black" || player == "black") {
        whiteKingInCheck = false;
    }

    // error checking
    if (!isValidCoor(start) || !isValidCoor(final)) {
        throw BoardException("Invalid letter-number coordinates1. Try again.");
    }

    pair<int, int> startrc = coorToRC(start);
    int startrow = startrc.first;
    int startcol = startrc.second;

    char pieceType = boardState[startrow][startcol]->getType();

    if (pieceType == ' ') {
        throw BoardException("Tile is empty. Try again.");
    }
    if (boardState[startrow][startcol]->isPieceWhite() != ((player == "White") || (player == "white"))) {
        throw BoardException("Attempted to move opponent's piece. Try again.");
    }
    pair<int, int> destrc = coorToRC(final);
    int destrow = destrc.first;
    int destcol = destrc.second;

    // if piece type and coordinates indicate castle, do a castle
    if (toupper(pieceType) == 'K') {
        if (boardState[startrow][startcol]->isPieceWhite() && start == "e1" && (final == "c1" || final == "g1")) {
            try {
                castle(player, start, final);
            }
            catch (BoardException e) {
                throw BoardException(e.what());
            }
            return;
        } else if (!boardState[startrow][startcol]->isPieceWhite() && start == "e8" && (final == "c8" || final == "g8")) {
            try {
                castle(player, start, final);
            }
            catch (BoardException e) {
                throw BoardException(e.what());
            }
            return;
        }
    }

    // check if en passant
    bool canEnPassant = false;

    if (toupper(boardState[startrow][startcol]->getType()) == 'P') {
        canEnPassant = canCaptureEnPassant(player, start, final);
    }

    // check if moving pawn to end of boad without promoting pawn
    if (toupper(pieceType) == 'P' && (final[1] == '1' || final[1] == '8') && !promotePawn) {
        throw BoardException("Tried to promote pawn but no replacement piece indicated. Try again.");
    }

    // check if capturing own piece
    if (boardState[destrow][destcol]->getType() != ' ') {
        if (boardState[destrow][destcol]->isPieceWhite() == ((player == "White") || (player == "white"))) {
            throw BoardException("Attempted to capture own piece. Try again.");
        }
    }

    // check if destination is valid
    if (boardState[destrow][destcol]->getType() != ' ') {
        boardState[startrow][startcol]->planToStandardCapture();
    } else if (boardState[destrow][destcol]->getType() == ' ' && toupper(pieceType) == 'P' && canEnPassant) {
        boardState[startrow][startcol]->planToStandardCapture();
    }
    if (!boardState[startrow][startcol]->canMove(destrow, destcol)) {
        throw BoardException("Invalid move. Try again.");
    }

    // check if pieces are in the way
    vector<pair<int, int>> path = boardState[startrow][startcol]->getPath(destrow, destcol);
    for (int i = 1; i < path.size()-1; ++i) {
        if (boardState[path[i].first][path[i].second]->getType() != ' ') {
            throw BoardException("Path is blocked by other pieces. Try again.");
        }
    }

    // check if move puts own king is in check
    if (((player == "White") || (player == "white")) && isKingInCheck("white", pieceType, startrow, startcol, destrow, destcol)) {
        throw BoardException("Move puts White King in check. Try again.");
    } else if (((player == "Black") || (player == "black")) && isKingInCheck("black", pieceType, startrow, startcol, destrow, destcol)) {
        throw BoardException("Move puts Black King in check. Try again.");
    }

    // if qualifies for en passant, capture en passant
    if (canEnPassant) {
        removePiece(startrow, destcol);
    }

    // move piece
    removePiece(start);
    if (boardState[destrow][destcol]->getType() != ' ') {
        removePiece(final);
    }
    addPiece(pieceType, final, true); // move piece to new destination

    lastMove = start + " " + final; // update last move

    if (!promotePawn) {
        if (((player == "White") || (player == "white")) && isOpposingKingInCheck(player, start, final)) {
            blackKingInCheck = true;
        } else if (((player == "Black") || (player == "black")) && isOpposingKingInCheck(player, start, final)) {
            whiteKingInCheck = true;
        }
    }
}

void Board::pawnPromotion(std::string player, std::string start, std::string dest, char newPiece) {
    // error checking
    if (dest[1] != '1' && dest[1] != '8') {
        throw BoardException("Pawn does not reach end of board. Try again.");
    }
    if (toupper(newPiece) != 'N' && toupper(newPiece) != 'R' && toupper(newPiece) != 'B' && toupper(newPiece) != 'Q') {
        throw BoardException("Pawn cannot promote to the given piece.");
    }
    if (!(isupper(newPiece) && ((player == "White") || (player == "white"))) &&
        !(!isupper(newPiece) && ((player == "Black") || (player == "black")))) {
        throw BoardException("Attempted to replace Pawn with opponent's piece.");
    }
    try {
        promotePawn = true;
        makeMove(player, start, dest); // move the piece
        removePiece(dest); // replace pawn with new piece
        addPiece(newPiece, dest, true);
        if (((player == "White") || (player == "white")) && isOpposingKingInCheck(player, start, dest)) {
            // cout << "true" << endl;
            blackKingInCheck = true;
        } else if (((player == "Black") || (player == "black")) && isOpposingKingInCheck(player, start, dest)) {
            whiteKingInCheck = true;
        }
        promotePawn = false;
    } catch (BoardException e) { // catch exceptions if they arise
        throw BoardException(e.what());
    }
}

// assumed piece is a pawn
bool Board::canCaptureEnPassant(string player, string start, string dest) const {
    if (!((abs(start[0]-dest[0]) == 1) && (abs(start[1]-dest[1]) == 1))) {
        return false;
    }

    pair<int, int> startrc = coorToRC(start); // covert to row/col
    int startrow = startrc.first;
    int startcol = startrc.second;
    pair<int, int> destrc = coorToRC(dest);
    int destrow = destrc.first;
    int destcol = destrc.second;
    if (toupper(boardState[startrow][destcol]->getType()) != 'P') { // piece beside is a pawn
        return false;
    }
    if (boardState[startrow][destcol]->isPieceWhite() == boardState[startrow][startcol]->isPieceWhite()) { // pawn beside is opponent
        return false;
    }

    string lastmovestart = lastMove.substr(0, 2);
    string lastmovedest = lastMove.substr(3, 2);

    if (lastmovedest[0] != dest[0] || lastmovedest[1] != start[1]) { // other pawn has just moved
        return false;
    }
    if (lastmovestart[0] != dest[0] || abs(lastmovestart[1]-start[1]) != 2) { // other pawn moved two spaces
        return false;
    }
    return true;
}

bool Board::canCastle(string player, string start, string dest) const {
    if (start != "e1" && start != "e8") return false;

    if (player == "white" || player == "White") {
        if (whiteKingInCheck) return false; // is king currently in check
        if (wKing->pieceHasMoved()) return false;

        if (dest == "c1") { // perform castle with white left rook
            if (boardState[7][0]->getType() != 'R' || boardState[7][0]->pieceHasMoved()) {
                return false;
            }
            for (int i = 1; i < 4; ++i) { // check that there are no pieces between king and rook
                if (boardState[7][i]->getType() != ' ') return false;
            }
            if (isKingInCheck("white", 'K', 7, 4, 7, 3)) return false; // piece between
            if (isKingInCheck("white", 'K', 7, 4, 7, 2)) return false; // destination piece
        } else if (dest == "g1") { // perform castle with white right rook
            if (boardState[7][7]->getType() != 'R' || boardState[7][7]->pieceHasMoved()) {
                return false;
            }
            for (int i = 5; i < 7; ++i) { // check that there are no pieces between king and rook
                if (boardState[7][i]->getType() != ' ') return false;
            }
            if (isKingInCheck("white", 'K', 7, 4, 7, 5)) return false; // piece between
            if (isKingInCheck("white", 'K', 7, 4, 7, 6)) return false; // destination piece
        } else return false;

    } else if (player == "black" || player == "Black") {
        if (blackKingInCheck) return false; // is king currently in check
        if (bKing->pieceHasMoved()) return false;

        if (dest == "c8") {  // perform castle with black left rook
            if (boardState[0][0]->getType() != 'r' || boardState[0][0]->pieceHasMoved()) {
                return false;
            }
            for (int i = 1; i < 4; ++i) { // check that there are no pieces between king and rook
                if (boardState[0][i]->getType() != ' ') return false;
            }
            if (isKingInCheck("black", 'k', 0, 4, 0, 3)) return false; // piece between
            if (isKingInCheck("black", 'k', 0, 4, 0, 2)) return false; // destination piece
        } else if (dest == "g8") {  // perform castle with black right rook
            if (boardState[0][7]->getType() != 'r' || boardState[0][7]->pieceHasMoved()) {
                return false;
            }
            for (int i = 5; i < 7; ++i) { // check that there are no pieces between king and rook
                if (boardState[0][i]->getType() != ' ') return false;
            }
            if (isKingInCheck("black", 'k', 0, 4, 0, 5)) return false; // piece between
            if (isKingInCheck("black", 'k', 0, 4, 0, 6)) return false; // destination piece
        } else return false;
    }
    return true;
}

void Board::castle(string player, string start, string dest) {
    if (start != "e1" && start != "e8") throw BoardException("Invalid coordinates for castling.");

    if (player == "white" || player == "White") {
        if (whiteKingInCheck) throw BoardException("Cannot castle in check."); // is king currently in check
        if (wKing->pieceHasMoved()) throw BoardException("Cannot castle. King has already moved.");

        if (dest == "c1") { // perform castle with white left rook
            if (boardState[7][0]->getType() != 'R' || boardState[7][0]->pieceHasMoved()) {
                throw BoardException("Cannot castle. Rook has moved.");
            }
            for (int i = 1; i < 4; ++i) { // check that there are no pieces between king and rook
                if (boardState[7][i]->getType() != ' ') throw BoardException("Cannot castle. There are pieces between K and R.");
            }
            if (isKingInCheck("white", 'K', 7, 4, 7, 3)) throw BoardException("Cannot castle through check."); // piece between
            if (isKingInCheck("white", 'K', 7, 4, 7, 2)) throw BoardException("Cannot castle through check."); // destination piece
            removePiece("e1"); // perform the castle
            addPiece('R', "d1", true);
            addPiece('K', "c1", true);
            removePiece("a1");
        } else if (dest == "g1") { // perform castle with white right rook
            if (boardState[7][7]->getType() != 'R' || boardState[7][7]->pieceHasMoved()) {
                throw BoardException("Cannot castle. Rook has moved.");
            }
            for (int i = 5; i < 7; ++i) { // check that there are no pieces between king and rook
                if (boardState[7][i]->getType() != ' ') throw BoardException("Cannot castle. There are pieces between K and R.");
            }
            if (isKingInCheck("white", 'K', 7, 4, 7, 5)) throw BoardException("Cannot castle through check."); // piece between
            if (isKingInCheck("white", 'K', 7, 4, 7, 6)) throw BoardException("Cannot castle through check."); // destination piece
            removePiece("e1"); // perform the castle
            addPiece('R', "f1", true);
            addPiece('K', "g1", true);
            removePiece("h1");
        } else throw BoardException("Invalid coordinates for castling.");

    } else if (player == "black" || player == "Black") {
        if (blackKingInCheck) throw BoardException("Cannot castle in check."); // is king currently in check
        if (bKing->pieceHasMoved()) throw BoardException("Cannot castle. King has already moved.");

        if (dest == "c8") {  // perform castle with black left rook
            if (boardState[0][0]->getType() != 'r' || boardState[0][0]->pieceHasMoved()) {
                throw BoardException("Cannot castle. Rook has moved.");
            }
            for (int i = 1; i < 4; ++i) { // check that there are no pieces between king and rook
                if (boardState[0][i]->getType() != ' ') throw BoardException("Cannot castle. There are pieces between K and R.");
            }
            if (isKingInCheck("black", 'k', 0, 4, 0, 3)) throw BoardException("Cannot castle through check."); // piece between
            if (isKingInCheck("black", 'k', 0, 4, 0, 2)) throw BoardException("Cannot castle through check."); // destination piece
            removePiece("e8"); // perform the castle
            addPiece('R', "d8", true);
            addPiece('K', "c8", true);
            removePiece("a8");
        } else if (dest == "g8") {  // perform castle with black right rook
            if (boardState[0][7]->getType() != 'r' || boardState[0][7]->pieceHasMoved()) {
                throw BoardException("Cannot castle. Rook has moved.");
            }
            for (int i = 5; i < 7; ++i) { // check that there are no pieces between king and rook
                if (boardState[0][i]->getType() != ' ') throw BoardException("Cannot castle. There are pieces between K and R.");
            }
            if (isKingInCheck("black", 'k', 0, 4, 0, 5)) throw BoardException("Cannot castle through check."); // piece between
            if (isKingInCheck("black", 'k', 0, 4, 0, 6)) throw BoardException("Cannot castle through check."); // destination piece
            removePiece("e8"); // perform the castle
            addPiece('R', "f8", true);
            addPiece('K', "g8", true);
            removePiece("h8");
        } else throw BoardException("Invalid coordinates for castling.");
    }
}

bool Board::hasWhiteKing() const {
    return wKing != nullptr;
}

bool Board::hasBlackKing() const {
    return bKing != nullptr;
}

bool Board::isOpposingKingInCheck(string player, string start, string dest) const {
    pair<int, int> startrc = coorToRC(start);
    int startrow = startrc.first;
    int startcol = startrc.second;

    pair<int, int> destrc = coorToRC(dest);
    int destrow = destrc.first;
    int destcol = destrc.second;

    char pieceType = boardState[destrow][destcol]->getType();

    pair<int, int> kingpos;
    if ((player == "White") || (player == "white")) {
        kingpos = bKing->getPos(); // opposing king
    } else {
        kingpos = wKing->getPos();
    }
    bool opposingKingInCheck = canCheckKing(pieceType, kingpos.first, kingpos.second, destrow, destcol, startrow, startcol, destrow, destcol);
    if (((player == "White") || (player == "white")) && opposingKingInCheck) {
        return true;
    } else if (((player == "Black") || (player == "black")) && opposingKingInCheck) {
        return true;
    }
    return false;
}

bool Board::isKingInCheck(string colour, char type, int emptyrow, int emptycol, int occupiedrow, int occupiedcol) const {
    int kingrow, kingcol;
    vector<Piece *> thePieces;
    if (toupper(type) != 'K' && (colour == "white" || colour == "White")) {
        pair<int, int> kingpos = wKing->getPos();
        kingrow = kingpos.first;
        kingcol = kingpos.second;
        thePieces = blackPieces;
    } else if (toupper(type) != 'K' && (colour == "black" || colour == "Black")) {
        pair<int, int> kingpos = bKing->getPos();
        kingrow = kingpos.first;
        kingcol = kingpos.second;
        thePieces = whitePieces;
    } else if (type == 'K') {
        kingrow = occupiedrow;
        kingcol = occupiedcol;
        thePieces = blackPieces;
    } else if (type == 'k') {
        kingrow = occupiedrow;
        kingcol = occupiedcol;
        thePieces = whitePieces;
    }
    for (int i = 0; i < thePieces.size(); ++i) {
        pair<int, int> piecepos = thePieces[i]->getPos();
        int piecerow = piecepos.first;
        int piececol = piecepos.second;
        if (piecerow == emptyrow && piececol == emptycol) {
            if (canCheckKing(thePieces[i]->getType(), kingrow, kingcol, occupiedrow, occupiedcol,
                             emptyrow, emptycol, occupiedrow, occupiedcol)) {
                return true;
            }
        }
        if (canCheckKing(thePieces[i]->getType(), kingrow, kingcol, piecerow, piececol, emptyrow, emptycol, occupiedrow, occupiedcol)) {
            return true;
        }
    }
    return false;
}

bool Board::canCheckKing(char type, int kingrow, int kingcol, int typerow, int typecol, int emptyrow, int emptycol, int occupiedrow, int occupiedcol) const {
    // check if destination is valid
    unique_ptr<Piece> targetPiece;
    if (type == 'p' || type == 'P') {
        targetPiece = make_unique<Pawn>(type);
    } else if ((type == 'n' || type == 'N')) {
        targetPiece =  make_unique<Knight>(type);
    } else if ((type == 'r' || type == 'R')) {
        targetPiece = make_unique<Rook>(type);
    } else if ((type == 'b' || type == 'B')) {
        targetPiece = make_unique<Bishop>(type);
    } else if ((type == 'q' || type == 'Q')) {
        targetPiece = make_unique<Queen>(type);
    } else { //(type == 'k' || type == 'K')
        targetPiece = make_unique<King>(type);
    }
    targetPiece->setCoors(typerow, typecol);

    targetPiece->planToStandardCapture();
    if (!targetPiece->canMove(kingrow, kingcol)) {
        return false;
    }
    // check if pieces are in the way
    vector<pair<int, int>> path = targetPiece->getPath(kingrow, kingcol);
    for (int i = 1; i < path.size()-1; ++i) {
        if (boardState[path[i].first][path[i].second]->getType() != ' ') {
            if (path[i].first == emptyrow && path[i].second == emptycol) {
                continue; // if the piece in the way is the hypothetical piece
            } else {
                return false;
            }
        } else if (path[i].first == occupiedrow && path[i].second == occupiedcol) { // moved piece is in the way
            return false;
        }
    }
    // if last move was (empty, occupied) - aka piece has already moved - return true
    if (!settingUp && !lastMove.empty()) {
        string lastmovestart = lastMove.substr(0, 2);
        string lastmovedest = lastMove.substr(3, 2);
        pair<int, int> startrc = coorToRC(lastmovestart); // covert to row/col
        int startrow = startrc.first;
        int startcol = startrc.second;
        pair<int, int> destrc = coorToRC(lastmovedest);
        int destrow = destrc.first;
        int destcol = destrc.second;
        if (emptyrow == startrow && emptycol == startcol && occupiedrow == destrow && occupiedcol == destcol) return true;
    }

    if (typerow == occupiedrow && typecol == occupiedcol) { // if the piece that can check has been captured by potential move
        if (targetPiece->getType() != boardState[emptyrow][emptycol]->getType()) {
            return false;
        }
    }
    return true;
}

bool Board::isCheck(string player) const {
    if (settingUp) {
        pair<int, int> wkingpos = wKing->getPos();
        int wkingrow = wkingpos.first;
        int wkingcol = wkingpos.second;
        pair<int, int> bkingpos = bKing->getPos();
        int bkingrow = bkingpos.first;
        int bkingcol = bkingpos.second;
        if (isKingInCheck("white", 'K', wkingrow, wkingcol, wkingrow, wkingcol)) {
            return true;
        }
        if (isKingInCheck("black", 'k', bkingrow, bkingcol, bkingrow, bkingcol)) {
            return true;
        }
        return false;
    } else {
        if ((player == "White") || (player == "white")) return whiteKingInCheck;
        else return blackKingInCheck;
    }
}

bool Board::isStalemate(string player) const {
    if (isCheck(player)) return false;
    if (hasWhiteKing() && hasBlackKing() && whitePieces.size() == 1 && blackPieces.size() == 1) return true;

    vector<Piece *> thePieces;
    if (player == "white" || player == "White") {
        thePieces = whitePieces;
    } else if (player == "black" || player == "Black") {
        thePieces = blackPieces;
    }

    for (int i = 0; i < thePieces.size(); ++i) {
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                pair<int, int> currpos = thePieces[i]->getPos();
                string start = rcToCoor(currpos.first, currpos.second);
                string dest = rcToCoor(r, c);
                if (canMove(player, start, dest)) {
                    return false;
                }
            }
        }
    }
    return true;
}


bool Board::isCheckmate(string player) const {
    if (!isCheck(player)) return false;

    vector<Piece *> thePieces;
    if (player == "white" || player == "White") {
        thePieces = whitePieces;
    } else if (player == "black" || player == "Black") {
        thePieces = blackPieces;
    }

    for (int i = 0; i < thePieces.size(); ++i) {
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                pair<int, int> currpos = thePieces[i]->getPos();
                string start = rcToCoor(currpos.first, currpos.second);
                string dest = rcToCoor(r, c);
                if (canMove(player, start, dest)) {
                    return false;
                }
            }
        }
    }
    return true;
}

vector<Piece *> Board::getWhitePieces() const { // returns Board's white pieces
    return whitePieces;
}
vector<Piece *> Board::getBlackPieces() const { // returns Board's black pieces
    return blackPieces;
}
Piece* Board::getWhiteKing() const { // returns Board's white king
    return wKing;
}
Piece* Board::getBlackKing() const { // returns Board's black king
    return bKing;
}

Board::~Board() {

}

