#ifndef BOARDEXPN_H
#define BOARDEXPN_H

#include <stdexcept>
#include <string>

// this is Board's exception class

class BoardException : public std::exception {
    std::string message;

    public:
    BoardException(std::string message); // throw with a custom message
    std::string what(); // return that custom message
};

#endif
