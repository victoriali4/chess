#include "computer.h"
#include "piece.h"
#include "board.h"
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

void Computer::clearVectors() {
    // clear all vectors in the field after every time the computer player moves
    allMovesW.clear();
    allMovesB.clear();
    safeMovesB.clear();
    safeMovesW.clear();
    capturingMoveW.clear();
    avoidingMoveW.clear();
    checkingMoveW.clear();
    capturingMoveB.clear();
    avoidingMoveB.clear();
    checkingMoveB.clear();
}

Computer::~Computer() {}

// coordToString converts integer coordinates into string coordinates
string Computer::coordToString(pair<int, int> coord){
    int newrow = 8 - coord.first;
    int newcol = coord.second + 1;
    char strcol = newcol + 'a' - 1;
    char strrow = newrow + '0';
    char arr[] = {strcol, strrow, '\0'};
    string strcoords = arr;
    return strcoords;
}

// stringToCoord converts string coordinates into int coordinates
pair<int,int> Computer::stringToCoord(string coord) {
    pair<int, int> newpair;
    try {
        // convert using ascii values
        newpair.second = coord[0]-'a';
        newpair.first = 8 - (coord[1]-'0');
    } catch (...) {
        // return negative numbers if coors is invalid
        newpair.first = -1;
        newpair.second = -1;
    }
    return newpair;
}

int Computer::getLevel() const{
    return level;
}

// The gameEngine calls this function to return a string for the computer's turn to move
string Computer::move(const Board *b, string colour) {
    if (level == 1) {
        clearVectors();
        return levelOne(b, colour);
    } else if (level == 2) {
        clearVectors();
        return levelTwo(b, colour);
    } else if (level == 3) {
        clearVectors();
        return levelThree(b, colour);
    } else if (level == 4) {
        clearVectors();
        return levelFour(b, colour);
    }
    return "No a possible level";
}

// addMove function adds all legal moves of piece p on board b into the allMoves lists and sorts out the capturing moves
// assign value to each type of piece
// pawn promotion
void Computer::addMove(const Board *b, Piece* p, string colour) {
    vector<string> moves;
    vector<string> capturingMoves;
    // loop through every cell of the board *b
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            // records the piece's position
            string moveFrom = coordToString(p->getPos());
            // records the possible cell's position
            string moveTo = coordToString({i, j});
            // add black pawn promotion
            if (p->getType() == 'p' && i == 7) {
                if (p->canMove(i,j) && b->getPieceAt(i,j)->getType() == ' '){
                    moves.emplace_back("move " + moveFrom + " " + moveTo + " q");
                } else {
                    unique_ptr<Pawn> pawn = make_unique<Pawn>('p');
                    pawn->setCoors(p->getPos().first, p->getPos().second);
                    pawn->planToStandardCapture();
                    if (pawn->canMove(i,j) && b->getPieceAt(i,j)->getType() > 'A' && b->getPieceAt(i,j)->getType() < 'Z'){
                        moves.emplace_back("move " + moveFrom + " " + moveTo + " q");
                    }
                }
            } else if (p->getType() == 'P' && i == 0) {
                // add white pawn promotion
                if (p->canMove(i,j) && b->getPieceAt(i,j)->getType() == ' '){
                    moves.emplace_back("move " + moveFrom + " " + moveTo + " Q");
                } else {
                    unique_ptr<Pawn> pawn = make_unique<Pawn>('P');
                    pawn->setCoors(p->getPos().first, p->getPos().second);
                    pawn->planToStandardCapture();
                    if (pawn->canMove(i,j) && b->getPieceAt(i,j)->getType() > 'a' && b->getPieceAt(i,j)->getType() < 'z'){
                        moves.emplace_back("move " + moveFrom + " " + moveTo + " Q");
                    }
                }
            } else if (b->canMove(colour, moveFrom, moveTo)) {
                // if the piece can move to the cell and the cell is empty, then it is a normal move
                // else if it can move to the cell but the cell is not empty, it is a capturing move
                if (b->getPieceType(i, j) == ' ') {
                    moves.emplace_back("move " + moveFrom + " " + moveTo);
                } else {
                    capturingMoves.emplace_back("move " + moveFrom + " " + moveTo);
                    moves.emplace_back("move " + moveFrom + " " + moveTo);
                }
            } else if (b->canCastle(colour, moveFrom, moveTo)) {
                // add castling moves
                moves.emplace_back("move " + moveFrom + " " + moveTo);
            }
        }
    }
    // updates the allMoves and capturingMove vectors of colour
    if (colour == "white") {
        for (auto move: moves) {
            allMovesW.emplace_back(move);
        }
        for (auto move: capturingMoves) {
            capturingMoveW.emplace_back(move);
        }
    } else if (colour == "black") {
        for (auto p: moves) {
            allMovesB.emplace_back(p);
        }
        for (auto p: capturingMoves) {
            capturingMoveB.emplace_back(p);
        }
    }
}

// getFirstCoord gets where the piece is moving from
// returns a string coord (ex. "e4")
string Computer::getFirstCoord(string move) {
    istringstream convert{move};
    string segment;
    convert >> segment;
    convert >> segment;
    return segment;
}

// getSecondCoord gets where the piece is moving to
// returns a string coord (ex. "e4")
string Computer::getSecondCoord(string move) {
    istringstream convert{move};
    string segment;
    convert >> segment;
    convert >> segment;
    convert >> segment;
    return segment;
}

// pieceToValue returns the number value of piece p
int Computer::pieceToValue(Piece *p) const {
    char pieceType = p->getType();
    if (pieceType == 'p' || pieceType == 'P') {
        return 1;
    } else if (pieceType == 'n' || pieceType == 'N' || pieceType == 'b' || pieceType == 'B') {
        return 3;
    } else if (pieceType == 'r' || pieceType == 'R') {
        return 5;
    } else if (pieceType == 'q' || pieceType == 'Q') {
        return 9;
    } else if (pieceType == 'k' || pieceType == 'K') {
        return 10;
    }
    return 0;
}

// returns true if piece p can be captured on the board b right now
// colour is the colour of the piece p
bool Computer::pieceCanBeCaptured(const Board *b, const Piece *p, string colour) {
    vector<Piece *> pieceToGoThrough; // opponent's pieces
    string opp; // opponent's colour
    if (colour == "white" || colour == "White") {
        pieceToGoThrough = b->getBlackPieces();
        opp = "black";
    } else if (colour == "black" || colour == "Black") {
        pieceToGoThrough = b->getWhitePieces();
        opp = "white";
    }

    for (auto oppPiece: pieceToGoThrough) {
        if (b->canMove(opp, coordToString(oppPiece->getPos()), coordToString(p->getPos()))) {
            return true;
        }
    }
    return false;
}

// dangerAfterMove checks if any piece of colour is in danger caused by p moving from {emptyRow, emptyCol} to {newRow, newCol}
// colour is the colour of p
bool Computer::dangerAfterMove(const Board *b, const Piece *p, int emptyRow, int emptyCol, int newRow, int newCol, string colour) const{
    vector<Piece *> pieceToGoThrough; // opponent's pieces
    string opp; // opponent's colour
    if (colour == "black") {
        pieceToGoThrough = b->getWhitePieces();
        opp = "white";
    } else if (colour == "white") {
        pieceToGoThrough = b->getBlackPieces();
        opp = "black";
    }

    for(auto piece : pieceToGoThrough) {
        // if piece is captured after move, then we don't need to check if piece will cause danger
        if (piece->getPos().first == newRow && piece->getPos().second == newCol){
            continue;
        }
        char type = piece->getType();
        unique_ptr<Piece> targetPiece; // to check if target piece can cause danger (capture any piece)
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

        // set the position of targetPiece to the position of piece
        targetPiece->setCoors(piece->getPos().first, piece->getPos().second);
        // the piece is move as it is planning to capture
        targetPiece->planToStandardCapture();

        // if piece that is potentially in danger is potentially moving
        if (p->getPos().first == emptyRow && p->getPos().second == emptyCol) {
            // check if targetPiece can move to {newRow, newCol} which will be occupied after move
            if (targetPiece->canMove(newRow, newCol)){
                vector<pair<int,int>> path = targetPiece->getPath(newRow, newCol);
                bool inDanger = true;
                
                // check if there is any piece in the way
                for (int i = path.size() - 2; i >= 1; i--) {
                    // check if there is a piece in the way
                    if (b->getPieceAt(path.at(i).first, path.at(i).second)->getType() != ' ') {
                        if (path.at(i).first == emptyRow && path.at(i).second == emptyCol) {
                            continue; // if the piece in the way is the moved piece
                        } else if ((b->getPieceAt(path.at(i).first, path.at(i).second)->isPieceWhite() && opp == "black") ||
                                   (!b->getPieceAt(path.at(i).first, path.at(i).second)->isPieceWhite() && opp == "white")) {
                            inDanger = true;
                            break;
                        }
                        inDanger = false;
                        break;
                    }
                }
                if (inDanger) {
                    return true;
                }
            }
        } /*else if (b->canMove(opp, coordToString(targetPiece->getPos()), coordToString(p->getPos()))) {
            return true;
        }*/
        // if targetPiece can move to p and p is not potentially moving, check if there is any piece in the way
        else if (targetPiece->canMove(p->getPos().first, p->getPos().second)){
            vector<pair<int,int>> path = targetPiece->getPath(p->getPos().first, p->getPos().second);
            bool inDanger = true;
            for (int i = path.size() - 2; i >= 1; i--) {
                if (b->getPieceAt(path.at(i).first, path.at(i).second)->getType() != ' ') {
                    if (path.at(i).first == emptyRow && path[i].second == emptyCol) {
                        continue; // if the piece in the way is the hypothetical piece
                    } else if (path.at(i).first == newRow && path.at(i).second == newCol) { // moved piece is in the way
                        inDanger = true;
                        break;
                    } else {
                        inDanger = false;
                        break;
                    }
                }
            }
            if (inDanger) {
                return true;
            }
        }
    }
    return false;
}
// maxCapture returns the capturingMove that captures the opponent's highest value piece
string Computer::maxCapture(const Board *b, string colour) const{
    vector<string> moveToGoThrough;
    int minValueMove = 10;
    int maxValueCapture = 0;
    int valueOfMoved;
    int valueOfCaptured;
    string maxValueMove = "";
    if (colour == "black") {
        moveToGoThrough = capturingMoveB;
    } else if (colour == "white") {
        moveToGoThrough = capturingMoveW;
    }
    for (auto move: moveToGoThrough) {
        pair<int,int> pos = stringToCoord(getFirstCoord(move)); // position of potentially moved piece
        pair<int,int> capturedPos = stringToCoord(getSecondCoord(move)); // position of the piece that is going to be captured
        Piece *movedPiece = b->getPieceAt(pos.first, pos.second); // get moved piece
        Piece *capturedPiece = b->getPieceAt(capturedPos.first, capturedPos.second); // get piece that can be captured
        valueOfMoved = pieceToValue(movedPiece); // store the value of the potentially moved piece
        valueOfCaptured = pieceToValue(capturedPiece); // store the value of the piece that can be captured
        // choose the move that capture the highest value piece
        // if the value of the piece it can capture is equal to the maxValueCapture,
        // then choose the one the moves the lowest value piece, so if it is going to be captured the player loses least
        if (valueOfCaptured > maxValueCapture) {
            maxValueMove = move;
            maxValueCapture = valueOfCaptured;
            minValueMove = valueOfMoved;
        } else if (valueOfCaptured == maxValueCapture) {
            if (minValueMove > valueOfMoved) {
                maxValueMove = move;
                maxValueCapture = valueOfCaptured;
                minValueMove = valueOfMoved;
            }
        }
    }
    return maxValueMove;
}
// maxAvoid returns the avoiding move that moves the maximum value piece
string Computer::maxAvoid (const Board *b, string colour) const {
    vector<string> avoids;
    if (colour == "white") {
        avoids = avoidingMoveW;
    } else if (colour == "black") {
        avoids = avoidingMoveB;
    }
    int maxValue = 0;
    string maxValueMove = "";

    // check if all the pieces that are avoiding are the same piece
    string movedPiece = getFirstCoord(avoids.at(0));
    bool samePiece = true;
    for (auto move: avoids) {
        if (getFirstCoord(move) != movedPiece) {
            samePiece = false;
            break;
        }
    }
    //if all the pieces that are avoiding are the same piece, return a random move from avoiding moves
    if (samePiece) {
        srand((unsigned) time(0));
        int i = rand() % avoids.size();
        return avoids.at(i);
    }

    // find the maximum value piece
    for (auto move: avoids) {
        pair<int,int> movedFrom = stringToCoord(getFirstCoord(move));
        Piece *movedPiece = b->getPieceAt(movedFrom.first, movedFrom.second);
        int value = pieceToValue(movedPiece);
        if (value > maxValue) {
            maxValueMove = move;
            maxValue = value;
        }
    }

    return maxValueMove;
}

// addAvoid adds avoiding moves to colour's avoidingMoves vector
// the avoiding moves are moves that avoids potential capture by the opponent
// it won't put piece in danger after moving
// If there is still piece in danger after the move, it will be a valid avoiding move only if the piece in danger has lower value than he piece that is moving
// If the opponent's piece that is causing danger can be captured by colour's piece, and capturing this opponent's piece won't be a dangerous move
// it is also a valid avoiding move
void Computer::addAvoid(const Board *b, string colour) {
    vector<string> oppCaptures; // opponent's potential capturing moves
    vector<string> allMoves; // colour's all moves
    vector<string> captures; // Colour's capturing moves
    vector<string> avoids; // colour's avoiding moves
    string opp; // opponent's colour
    vector<Piece *> piecesToGoThrough; //colour's pieces
    if (colour == "white") {
        opp = "black";
        oppCaptures = capturingMoveB;
        allMoves = allMovesW;
        captures = capturingMoveW;
        piecesToGoThrough = b->getWhitePieces();
    } else if (colour == "black") {
        opp = "black";
        oppCaptures = capturingMoveW;
        allMoves = allMovesB;
        captures = capturingMoveB;
        piecesToGoThrough = b->getBlackPieces();
    }

        // loop through the opponent's capturing moves to see which pieces the opponent's can capture
        for (auto capture: oppCaptures) {
            string movedPiece = getFirstCoord(capture);
            string capturedPiece = getSecondCoord(capture);
            // prioritize avoid while capture
            for (auto move: captures) {
                string movedFrom = getFirstCoord(move);
                string movedTo = getSecondCoord(move);
                bool danger = false;

                if (movedPiece == movedTo) {
                    // if the capturing move of colour can capture the movedPiece of opponent's capture
                    int valueOfCaptured = pieceToValue(b->getPieceAt(stringToCoord(movedTo).first, stringToCoord(movedTo).second));
                    // check if any piece is in danger caused by this move
                    for (auto piece: piecesToGoThrough){
                        int valueOfPiece = pieceToValue(piece);
                        if (!(piece != b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second) && pieceCanBeCaptured(b, piece, colour))
                            && dangerAfterMove(b, piece,stringToCoord(movedFrom).first, stringToCoord(movedFrom).second,
                                               stringToCoord(movedTo).first, stringToCoord(movedTo).second, colour)
                            && valueOfCaptured <= valueOfPiece) {
                            danger = true;
                            break;
                        }
                    }
                    if (!danger) {
                        avoids.emplace_back(move);
                    }
                } else if (capturedPiece == movedFrom) {
                    // if the piece being moved can be captured by the opponent if not moving it
                    // then prioritize it as avoidingMove
                    int valueOfCaptured = pieceToValue(b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second));
                    // If there is still piece in danger after the move, check if the piece has higher value that the piece that is potentially being captured
                    // If not, then it is also a valid avoiding move
                    for (auto piece: piecesToGoThrough){
                        int valueOfPiece = pieceToValue(piece);
                        if (!(piece != b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second) && pieceCanBeCaptured(b, piece, colour))
                        && dangerAfterMove(b, piece,stringToCoord(movedFrom).first, stringToCoord(movedFrom).second,
                                            stringToCoord(movedTo).first, stringToCoord(movedTo).second, colour)
                            && valueOfCaptured <= valueOfPiece) {
                            danger = true;
                            break;
                        }
                    }
                    if (!danger) {
                        avoids.emplace_back(move);
                    }
                }
            }
            // if either there is no avoid while capture move or all of the moves in avoidingMove
            //  would cause the piece still able to be captured, then search for avoiding moves in allMoves
            if (avoids.empty()) {
                // prioritize avoid while capture
                for (auto move: allMoves) {
                    string movedFrom = getFirstCoord(move);
                    string movedTo = getSecondCoord(move);
                    bool danger = false;
                    // if the piece being moved can be captured by the opponent if not moving it
                    // then prioritize it as avoidingMove
                    if (capturedPiece == movedFrom) {
                        int valueOfCaptured = pieceToValue(b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second));
                        for (auto piece: piecesToGoThrough){
                            int valueOfPiece = pieceToValue(piece);
                            if (!(piece != b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second) && pieceCanBeCaptured(b, piece, colour))
                            && dangerAfterMove(b, piece,stringToCoord(movedFrom).first, stringToCoord(movedFrom).second,
                                               stringToCoord(movedTo).first, stringToCoord(movedTo).second, colour)&& valueOfCaptured <= valueOfPiece) {
                                danger = true;
                                break;
                            }
                        }
                        if (!danger) {
                                avoids.emplace_back(move);
                        }
                    }
                }
            }
            if (colour == "black") {
                for (auto move: avoids) {
                    avoidingMoveB.emplace_back(move);
                }
                avoids.clear();
            } else if (colour == "white") {
                capturingMoveB.clear();
                for (auto move: avoids) {
                    avoidingMoveW.emplace_back(move);
                }
                avoids.clear();
            }
        }

}

// addAvoidBetter is for level 4
// it sorts out avoiding moves from all the moves, and adds to the colour's avoidingMove list
// it also sort safe moves from allMoves and capturingMoves
void Computer::addAvoidBetter(const Board *b, string colour) {
    vector<string> oppCaptures;
    vector<string> allMoves;
    vector<string> captures;
    vector<string> checks;
    vector<string> avoids;
    vector<string> safes;
    vector<Piece *> piecesToGoThrough;
    if (colour == "white") {
        for (auto move: capturingMoveB){
            string moveFrom = getFirstCoord(move);
            string moveTo = getSecondCoord(move);
            Piece *p = b->getPieceAt(stringToCoord(moveFrom).first, stringToCoord(moveFrom).second);
            Piece *captured = b->getPieceAt(stringToCoord(moveTo).first, stringToCoord(moveTo).second);
            bool danger = dangerAfterMove(b, p, stringToCoord(moveFrom).first, stringToCoord(moveFrom).second,
                                          stringToCoord(moveTo).first, stringToCoord(moveTo).second, "black");
            if ((pieceToValue(p) < pieceToValue(captured) && danger) || !danger){
                oppCaptures.emplace_back(move);
            }
        }

        allMoves = allMovesW;
        captures = capturingMoveW;
        checks = checkingMoveW;
        piecesToGoThrough = b->getWhitePieces();
    } else if (colour == "black") {
        for (auto move: capturingMoveW){
            string moveFrom = getFirstCoord(move);
            string moveTo = getSecondCoord(move);
            Piece *p = b->getPieceAt(stringToCoord(moveFrom).first, stringToCoord(moveFrom).second);
            Piece *captured = b->getPieceAt(stringToCoord(moveTo).first, stringToCoord(moveTo).second);
            bool danger = dangerAfterMove(b, p, stringToCoord(moveFrom).first, stringToCoord(moveFrom).second,
                                          stringToCoord(moveTo).first, stringToCoord(moveTo).second, "white");
            if ((pieceToValue(p) < pieceToValue(captured) && danger) || !danger){
                oppCaptures.emplace_back(move);
            }
        }

        allMoves = allMovesB;
        checks = checkingMoveB;
        captures = capturingMoveB;
        piecesToGoThrough = b->getBlackPieces();
    }

    if (oppCaptures.empty()) {
        // prioritizes safe check moves
        for (auto move: checks) {
            string movedFrom = getFirstCoord(move);
            string movedTo = getSecondCoord(move);
            bool danger = false;
            for (auto piece: piecesToGoThrough){
                if (!(piece != b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second) && pieceCanBeCaptured(b, piece, colour))
                && dangerAfterMove(b, piece,stringToCoord(movedFrom).first, stringToCoord(movedFrom).second,
                                    stringToCoord(movedTo).first, stringToCoord(movedTo).second, colour)) {
                    danger = true;
                    break;
                }
            }
            if (!danger) {
                safes.emplace_back(move);
            }
        }
        // if there is no check moves or all check moves are dangerous
        // prioritizes safe capturing moves
        if (safes.empty()) {
            for (auto move: captures) {
                string movedFrom = getFirstCoord(move);
                string movedTo = getSecondCoord(move);
                bool danger = false;
                for (auto piece: piecesToGoThrough){
                    if (!(piece != b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second) && pieceCanBeCaptured(b, piece, colour))
                        && dangerAfterMove(b, piece,stringToCoord(movedFrom).first, stringToCoord(movedFrom).second,
                                        stringToCoord(movedTo).first, stringToCoord(movedTo).second, colour)) {
                        danger = true;
                        break;
                    }
                }
                if (!danger) {
                    safes.emplace_back(move);
                }
            }
        }
        // if there is no check and capturing moves or all check moves are dangerous
        // prioritizes safe moves
        if (safes.empty()) {
            for (auto move: allMoves) {
                string movedFrom = getFirstCoord(move);
                string movedTo = getSecondCoord(move);
                bool danger = false;
                for (auto piece: piecesToGoThrough){
                    if (!(piece != b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second) && pieceCanBeCaptured(b, piece, colour))
                        && dangerAfterMove(b, piece,stringToCoord(movedFrom).first, stringToCoord(movedFrom).second,
                                        stringToCoord(movedTo).first, stringToCoord(movedTo).second, colour)) {
                        danger = true;
                        break;
                    }
                }
                if (!danger) {
                    safes.emplace_back(move);
                }
            }
        }
        if (colour == "black") {
            for (auto move: safes) {
                safeMovesB.emplace_back(move);
            }
        } else if (colour == "white") {
            for (auto move: safes) {
                safeMovesW.emplace_back(move);
            }
        }
    } else {
        for (auto capture: oppCaptures) {
            string movedPiece = getFirstCoord(capture);
            string capturedPiece = getSecondCoord(capture);
            // prioritize avoid while capture
            for (auto move: captures) {
                string movedFrom = getFirstCoord(move);
                string movedTo = getSecondCoord(move);
                bool danger = false;

                if (movedPiece == movedTo) {
                    // if the capturing move can capture the movedPiece of opponent's capture
                    int valueOfCaptured = pieceToValue(
                            b->getPieceAt(stringToCoord(movedTo).first, stringToCoord(movedTo).second));
                    // check if any piece is in danger caused by this move
                    for (auto piece: piecesToGoThrough) {
                        int valueOfPiece = pieceToValue(piece);
                        if (!(piece != b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second) &&
                              pieceCanBeCaptured(b, piece, colour))
                            &&
                            dangerAfterMove(b, piece, stringToCoord(movedFrom).first, stringToCoord(movedFrom).second,
                                            stringToCoord(movedTo).first, stringToCoord(movedTo).second, colour)
                            && valueOfCaptured <= valueOfPiece) {
                            danger = true;
                            break;
                        }
                    }
                    if (!danger) {
                        avoids.emplace_back(move);
                    }
                } else if (capturedPiece == movedFrom) {
                    // if the piece being moved can be captured by the opponent if not moving it
                    // then prioritize it as avoidingMove
                    int valueOfCaptured = pieceToValue(
                            b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second));
                    for (auto piece: piecesToGoThrough) {
                        int valueOfPiece = pieceToValue(piece);
                        if (!(piece != b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second) &&
                              pieceCanBeCaptured(b, piece, colour))
                            &&
                            dangerAfterMove(b, piece, stringToCoord(movedFrom).first, stringToCoord(movedFrom).second,
                                            stringToCoord(movedTo).first, stringToCoord(movedTo).second, colour)
                            && valueOfCaptured <= valueOfPiece) {
                            danger = true;
                            break;
                        }
                    }
                    if (!danger) {
                        avoids.emplace_back(move);
                    }
                }
            }
            // if either there is no avoid while capture move or all of the moves in avoidingMove
            //  would cause the piece still able to be captured, then search for avoiding moves in allMoves
            if (avoids.empty()) {
                // prioritize avoid while capture
                for (auto move: allMoves) {
                    string movedFrom = getFirstCoord(move);
                    string movedTo = getSecondCoord(move);
                    bool danger = false;
                    // if the piece being moved can be captured by the opponent if not moving it
                    // then prioritize it as avoidingMove
                    if (capturedPiece == movedFrom) {
                        int valueOfCaptured = pieceToValue(
                                b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second));
                        for (auto piece: piecesToGoThrough) {
                            int valueOfPiece = pieceToValue(piece);
                            if (!(piece !=
                                  b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second) &&
                                  pieceCanBeCaptured(b, piece, colour))
                                && dangerAfterMove(b, piece, stringToCoord(movedFrom).first,
                                                   stringToCoord(movedFrom).second,
                                                   stringToCoord(movedTo).first, stringToCoord(movedTo).second,
                                                   colour) && valueOfCaptured <= valueOfPiece) {
                                danger = true;
                                break;
                            }
                        }
                        if (!danger) {
                            avoids.emplace_back(move);
                        }
                    }
                }
            }
            if (colour == "black") {
                for (auto move: avoids) {
                    avoidingMoveB.emplace_back(move);
                }
                avoids.clear();
            } else if (colour == "white") {
                for (auto move: avoids) {
                    avoidingMoveW.emplace_back(move);
                }
                avoids.clear();
            }
        }

        if (colour == "black") {
            avoids = avoidingMoveB;
        } else if (colour == "white") {
            avoids = avoidingMoveW;
        }
        // if there is pieces in danger, but there is no safe avoiding moves
        // prioritize safe checks, captures and moves
        if (avoids.empty()) {
            // prioritizes safe check moves
            for (auto move: checks) {
                string movedFrom = getFirstCoord(move);
                string movedTo = getSecondCoord(move);
                bool danger = false;
                for (auto piece: piecesToGoThrough){
                    if (!(piece != b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second) && pieceCanBeCaptured(b, piece, colour))
                        && dangerAfterMove(b, piece,stringToCoord(movedFrom).first, stringToCoord(movedFrom).second,
                                        stringToCoord(movedTo).first, stringToCoord(movedTo).second, colour)) {
                        danger = true;
                        break;
                    }
                }
                if (!danger) {
                    safes.emplace_back(move);
                }
            }
            // if there is no check moves or all check moves are dangerous
            // prioritizes safe capturing moves
            if (safes.empty()) {
                for (auto move: captures) {
                    string movedFrom = getFirstCoord(move);
                    string movedTo = getSecondCoord(move);
                    bool danger = false;
                    for (auto piece: piecesToGoThrough){
                        if (!(piece != b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second) && pieceCanBeCaptured(b, piece, colour))
                            && dangerAfterMove(b, piece,stringToCoord(movedFrom).first, stringToCoord(movedFrom).second,
                                            stringToCoord(movedTo).first, stringToCoord(movedTo).second, colour)) {
                            danger = true;
                            break;
                        }
                    }
                    if (!danger) {
                        safes.emplace_back(move);
                    }
                }
            }
            // if there is no check and capturing moves or all check moves are dangerous
            // prioritizes safe moves
            if (safes.empty()) {
                // << "case 2" << endl;
                for (auto move: allMoves) {
                    string movedFrom = getFirstCoord(move);
                    string movedTo = getSecondCoord(move);
                    bool danger = false;
                    for (auto piece: piecesToGoThrough){
                        if (!(piece != b->getPieceAt(stringToCoord(movedFrom).first, stringToCoord(movedFrom).second) && pieceCanBeCaptured(b, piece, colour))
                            && dangerAfterMove(b, piece,stringToCoord(movedFrom).first, stringToCoord(movedFrom).second,
                                            stringToCoord(movedTo).first, stringToCoord(movedTo).second, colour)) {
                            danger = true;
                            break;
                        }
                    }
                    if (!danger) {
                        safes.emplace_back(move);
                    }
                }
            }
            if (colour == "black") {
                for (auto move: safes) {
                    safeMovesB.emplace_back(move);
                }
            } else if (colour == "white") {
                for (auto move: safes) {
                    safeMovesW.emplace_back(move);
                }
            }
        }
    }
    if (colour == "black") {
        allMovesW.clear();
        capturingMoveW.clear();
    } else if (colour == "white") {
        allMovesB.clear();
        capturingMoveB.clear();
    }
}

// addCheckMoves adds moves to checkingMove lists where all the check moves are
void Computer::addCheckMoves(const Board *b, string colour){
    vector<string> checks; // check moves of colour
    vector<string> captures; // capturing moves of colour
    vector<string> moves; // all moves of colour
    string opp; // opponent's colour
    if (colour == "white") {
        captures = capturingMoveW;
        moves = allMovesW;
        opp = "black";
    } else if (colour == "black") {
        captures = capturingMoveB;
        moves = allMovesB;
        opp = "white";
    }

    for (auto move: captures) {
        pair<int,int> movedTo = stringToCoord(getSecondCoord(move));
        pair<int,int> movedFrom = stringToCoord(getFirstCoord(move));
        Piece *p = b->getPieceAt(movedFrom.first, movedFrom.second);
        // check after moving the piece to position movedTo, if it can check the king or not
        if (b->isKingInCheck(opp, p->getType(), movedFrom.first, movedFrom.second,
                             movedTo.first, movedTo.second)) {
            checks.emplace_back(move);
        }
    }

    // if there is no capture while checking moves, go through all moves to see if there is a check move
    if (checks.empty()) {
        for (auto move: moves) {
            pair<int,int> movedTo = stringToCoord(getSecondCoord(move));
            pair<int,int> movedFrom = stringToCoord(getFirstCoord(move));
            Piece *p = b->getPieceAt(movedFrom.first, movedFrom.second);
            if (b->isKingInCheck(opp, p->getType(), movedFrom.first, movedFrom.second,
                                 movedTo.first, movedTo.second)) {
                checks.emplace_back(move);
            }
        }
    }

    if (colour == "white") {
        for (auto move: checks) {
            checkingMoveW.emplace_back(move);
        }
    } else if (colour == "black") {
        for (auto move: checks) {
            checkingMoveB.emplace_back(move);
        }
    }
}

//maxValueCheck returns a check move that is the most valuable
// if all the check moves are moving the same piece, it returns a random check move
string Computer::maxValueCheck (const Board *b, string colour) const {
    vector<string> checks;
    if (colour == "white") {
        checks = checkingMoveW;
    } else if (colour == "black") {
        checks = checkingMoveB;
    }
    int minValue = 10;
    string maxValueMove = "";
    string movedPiece = getFirstCoord(checks.at(0));
    // check if all check moves are moving the same piece
    bool samePiece = true;
    for (auto move: checks) {
        if (getFirstCoord(move) != movedPiece) {
            samePiece = false;
            break;
        }
    }
    // if all check moves are moving the same piece, return a random check move
    if (samePiece) {
        srand((unsigned) time(0));
        int i = rand() % checks.size();
        return checks.at(i);
    }

    for (auto move: checks) {
        pair<int,int> movedFrom = stringToCoord(getFirstCoord(move));
        Piece *movedPiece = b->getPieceAt(movedFrom.first, movedFrom.second);
        int value = pieceToValue(movedPiece);
        if (value < minValue) {
            maxValueMove = move;
            minValue = value;
        }
    }

    return maxValueMove;
}

// the levelOne computer player returns a random legal move
string Computer::levelOne (const Board *b, string colour) {
    vector<Piece *> pieceToGoThrough;
    if (colour == "white") {
        pieceToGoThrough = b->getWhitePieces();
    } else if (colour == "black") {
        pieceToGoThrough = b->getBlackPieces();
    }

    srand((unsigned)time(0));
    int i,j;
    // loop through all pieces of the colour, generate a random row and column
    // check if the piece can move to that random cell, if so return
    for (auto p: pieceToGoThrough) {
        i = rand() % 8;
        j = rand() % 8;
        string moveFrom = coordToString(p->getPos());
        string moveTo = coordToString({i,j});
        if (b->canMove(colour, moveFrom, moveTo)) {
            return "move " + moveFrom + " " + moveTo;
        }
    }
    // if not returned from the above, add every possible moves into the allMoves vector
    // generate a random number less than the number of all moves and return the random move
    for (auto p: pieceToGoThrough) {
        addMove(b, p, colour);
    }
    int numOfMoves;
    if (colour == "black") {
        // prioritize pawn promotion
        for (auto move: allMovesB) {
            if (move.size() > 10) return move;
        }
        numOfMoves = allMovesB.size();
        if (numOfMoves == 0) {
            return "No possible moves";
        }
        i = (rand() % numOfMoves);
        return allMovesB.at(i);
    } else if (colour == "white") {
        // prioritize pawn promotion
        for (auto move: allMovesW) {
            if (move.size() > 10) return move;
        }
        numOfMoves = allMovesW.size();
        if (numOfMoves == 0) {
            return "No possible moves";
        }
        i = (rand() % numOfMoves);
        return allMovesW.at(i);
    }
}

// the levelTwo computer player prioritizes capturing moves, checking moves
string Computer::levelTwo(const Board *b, string colour){
    vector<Piece *> pieceToGoThrough;
    vector<string> checks;
    vector<string> captures;
    vector<string> moves;
    // set pieceToGoThrough according to piece's colour
    if (colour == "white") {
        pieceToGoThrough = b->getWhitePieces();
    } else if (colour == "black") {
        pieceToGoThrough = b->getBlackPieces();
    }

    // loop through all pieces of the colour, call the addMove function to
    // add all possible moves to allMoves vector
    for (auto p: pieceToGoThrough) {
        addMove(b, p, colour);
    }

    // add possible check moves by calling the addCheckMoves function
    addCheckMoves(b, colour);

    // copy all vectors over
    if (colour == "white") {
        checks = checkingMoveW;
        captures = capturingMoveW;
        moves = allMovesW;
    } else if (colour == "black") {
        checks = checkingMoveB;
        captures = capturingMoveB;
        moves = allMovesB;
    }

    srand((unsigned)time(0));
    int i;

    if (!checks.empty()) {
        if (checks.size() == 1) {
            return checks.at(0);
        } else {
            return maxValueCheck(b, colour);
        }
    } else if (!captures.empty()) {
        if (captures.size() == 1) {
            return captures.at(0);
        } else {
            return maxCapture(b,colour);
        }
    } else {
        if (moves.empty()) return "No possible moves";
        else {
            // prioritize pawn promotion
            for (auto move: moves) {
                if (move.size() > 10) return move;
            }
            i = (rand() % moves.size());
            return moves.at(i);
        }
    }
}

// the levelThree computer player prioritizes avoiding moves, then checking moves, then capturing moves
string Computer::levelThree(const Board *b, string colour) {
    vector<Piece *> pieceToGoThrough;
    vector<string> checks;
    vector<string> avoids;
    vector<string> captures;
    vector<string> moves;
    // set pieceToGoThrough according to piece's colour
    if (colour == "white") {
        pieceToGoThrough = b->getWhitePieces();
    } else if (colour == "black") {
        pieceToGoThrough = b->getBlackPieces();
    }

    // loop through all pieces of the colour, call the addMove function to
    // add all possible moves to allMoves vector
    for (auto p: b->getBlackPieces()) {
        addMove(b, p, "black");
    }

    for (auto p: b->getWhitePieces()) {
        addMove(b, p, "white");
    }

    // add possible check moves by calling the addCheckMoves function
    addCheckMoves(b, colour);
    addAvoid(b, colour);

    if (colour == "white") {
        checks = checkingMoveW;
        captures = capturingMoveW;
        moves = allMovesW;
        avoids = avoidingMoveW;
        //safes = safeMovesW;
    } else if (colour == "black") {
        checks = checkingMoveB;
        captures = capturingMoveB;
        moves = allMovesB;
        avoids = avoidingMoveB;
        //safes = safeMovesB;
    }

    srand((unsigned) time(0));
    int i;

    if (!avoids.empty()) {
        if (avoids.size() == 1) {
            return avoids.at(0);
        } else {
            return maxAvoid(b, colour);
        }
    } else if (!checks.empty()) {
        if (checks.size() == 1) {
            return checks.at(0);
        } else {
            return maxValueCheck(b, colour);
        }
    } else if (!captures.empty()) {
        if (captures.size() == 1) {
            return captures.at(0);
        } else {
            return maxCapture(b, colour);
        }
    } else {
        if (moves.empty()) return "No possible moves";
        else {
            for (auto move: moves) {
                if (move.size() > 10) return move;
            }
            i = (rand() % moves.size());
            return moves.at(i);
        }
    }
}

// levelfour computer player prioritizes avoiding moves, safe checking moves, safe capturing moves
string Computer::levelFour(const Board *b, string colour) {
    vector<Piece *> pieceToGoThrough;
    vector<string> checks;
    vector<string> avoids;
    vector<string> captures;
    vector<string> safes;
    vector<string> moves;
    // set pieceToGoThrough according to piece's colour
    if (colour == "white") {
        pieceToGoThrough = b->getWhitePieces();
    } else if (colour == "black") {
        pieceToGoThrough = b->getBlackPieces();
    }

    // loop through all pieces of the colour, call the addMove function to
    // add all possible moves to allMoves vector
    for (auto p: b->getBlackPieces()) {
        addMove(b, p, "black");
    }

    for (auto p: b->getWhitePieces()) {
        addMove(b, p, "white");
    }

    // add possible check moves by calling the addCheckMoves function
    addCheckMoves(b, colour);
    addAvoidBetter(b, colour);

    if (colour == "white") {
        checks = checkingMoveW;
        captures = capturingMoveW;
        moves = allMovesW;
        avoids = avoidingMoveW;
        safes = safeMovesW;
    } else if (colour == "black") {
        checks = checkingMoveB;
        captures = capturingMoveB;
        moves = allMovesB;
        avoids = avoidingMoveB;
        safes = safeMovesB;
    }

    srand((unsigned) time(0));
    int i;

    if (!avoids.empty()) {
        if (avoids.size() == 1) {
            return avoids.at(0);
        } else {
            return maxAvoid(b, colour);
        }
    } else if (!safes.empty()){
        i = (rand() % safes.size());
        return safes.at(i);
    } else if (!checks.empty()) {
        if (checks.size() == 1) {
            return checks.at(0);
        } else {
            return maxValueCheck(b, colour);
        }
    } else if (!captures.empty()) {
        if (captures.size() == 1) {
            return captures.at(0);
        } else {
            return maxCapture(b, colour);
        }
    } else {
        if (moves.empty()) return "No possible moves";
        else {
            for (auto move: moves) {
                if (move.size() > 10) return move;
            }
            i = (rand() % moves.size());
            return moves.at(i);
        }
    }
}
