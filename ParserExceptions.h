#ifndef CSV_PARSEREXCEPTION_H
#define CSV_PARSEREXCEPTION_H

#include <iostream>
#include <exception>
#include <string>
#include <memory>

class ParserException : public std::exception{
private:
    std::string _message;
public:
    ParserException(const char* message, int lineNumber, int columnNumber);
    [[nodiscard]] const char* what() const noexcept override;
};

#endif //CSV_PARSEREXCEPTION_H
