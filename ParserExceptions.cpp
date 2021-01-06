#include "ParserExceptions.h"

ParserException::ParserException(const char* message, int lineNumber, int columnNumber){
    _message = static_cast<std::string>(message) + "!\nline: " + std::to_string(lineNumber + 1) +
                                                   ", column: " + std::to_string(columnNumber + 1);
}

const char *ParserException::what() const noexcept{
    return _message.c_str();
}
