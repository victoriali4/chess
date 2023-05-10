#include "boardException.h"
#include <string>

using namespace std;

// see boardException.h for method descriptions

BoardException::BoardException(string message) : message(message) {}

string BoardException::what() { return message; }

