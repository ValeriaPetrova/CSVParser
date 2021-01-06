#ifndef CSV_CSVPARSER_H
#define CSV_CSVPARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>
#include <tuple>
#include <sstream>
#include "ParserExceptions.h"
#include "printTuple.h"

namespace csv{
    template<typename ... Args>
    class Parser{
    private:
        std::ifstream &_input;
        int _idx;
        char _columnDel;
        char _lineDel;
        char _fieldDel;
        enum class parsingState{
            Reading,
            Screening,
            EscapeScreening,
        };
    public:
        class iterator{
        private:
            Parser<Args...> &_parser;
            std::string _str;
            std::vector<std::string> _parsedLine;
            std::vector<int> _pos;
            int _currLine;
            int _currColumn = 0;
            bool _isLast = false;

            void parseLine();

            std::tuple<Args...>vectorIntoTuple(){
                _currColumn = 0;
                std::tuple<Args...>tup;
                forEach(tup, std::index_sequence_for<Args...>(), _parsedLine, _currColumn, _currLine);
                if (_currColumn != _parsedLine.size()){
                    throw ParserException("Error: tuple size != line elements", _currLine, _currColumn);
                }
                return tup;
            }

            template<std::size_t ... I>
            void forEach(std::tuple<Args...> &t, std::index_sequence<I...>, std::vector<std::string> parsedLine, int &currIndex, int &lineNumber){
                ((makeTuple(std::get<I>(t), parsedLine[I], currIndex, lineNumber)), ...);
            }

        public:
            iterator(Parser &p, int index) : _parser(p){
                _currLine = index;
                int i = 0;
                while (i < _currLine && std::getline(_parser._input, _str, _parser._lineDel)){
                    ++i;
                }
                if (!std::getline(_parser._input, _str, _parser._lineDel)){
                    throw std::runtime_error("Error: invalid line number");
                }
            }

            explicit iterator(Parser &parse, bool isLast) : _parser(parse){
                _currLine = 0;
                _isLast = isLast;
            }

            iterator operator++(){
                _currLine++;
                if (!std::getline(_parser._input, _str, _parser._lineDel)){
                    _isLast = true;
                }
                return *this;
            }

            bool operator==(iterator other){
                return this->_isLast == other._isLast;
            }

            bool operator!=(iterator other){
                return !(*this == other);
            }

            std::tuple<Args...> operator*(){
                this->parseLine();
                return this->vectorIntoTuple();
            }
        };

        iterator begin(){
            return iterator(*this, _idx);
        }

        iterator end(){
            bool isEnd = true;
            return iterator(*this, isEnd);
        }

        explicit Parser(std::ifstream &file, int index = 0,
                        char columnDel = ',', char lineDel = '\n', char fieldDel = '"') : _input(file){
            if (index <= 0) throw std::runtime_error("Error: invalid line number");
            _idx = index - 1;
            _columnDel = columnDel;
            _lineDel = lineDel;
            _fieldDel = fieldDel;
        }

        char getColumnDel(){
            return _columnDel;
        }

        char getFieldDel(){
            return _fieldDel;
        }

        template<typename T>
        static void makeTuple(T& t, std::string &str, int &currIndex, int &lineNumber){
            std::istringstream stream(str);
            if ((stream >> t).fail() || !(stream >> std::ws).eof()){
                throw ParserException("Error: incorrect field", lineNumber, currIndex);
            }
            currIndex ++;
        }

        static void makeTuple(std::string &t, std::string &str, int &currIndex, int &lineNumber){
            t = str;
            currIndex ++;
        }
    };

    template<typename... Args>
    void Parser<Args...>::iterator::parseLine(){
        parsingState state = parsingState::Reading;
        _parsedLine.clear();
        _parsedLine.emplace_back("");
        _pos.clear();
        char columnDel = _parser.getColumnDel();
        char fieldDel = _parser.getFieldDel();
        size_t i = 0; // index of the current field
        size_t curPos = 0;
        for (char c : _str){
            switch (state){
                case parsingState::Reading:
                    if (c == columnDel){
                        _parsedLine.emplace_back("");
                        _pos.push_back(curPos);
                        i++;
                    }
                    else if (c == fieldDel){
                        state = parsingState::Screening;
                    }
                    else {
                        _parsedLine[i].push_back(c);
                    }
                    break;
                case parsingState::Screening:
                    if (c == fieldDel){
                        state = parsingState::EscapeScreening;
                    }
                    else {
                        _parsedLine[i].push_back(c);
                    }
                    break;
                case parsingState::EscapeScreening:
                    if (c == columnDel){
                        _parsedLine.emplace_back("");
                        _pos.push_back(curPos);
                        i++;
                        state = parsingState::Reading;
                    }
                    else if (c == fieldDel){
                        _parsedLine[i].push_back(fieldDel);
                        state = parsingState::Screening;
                    }
                    else{
                        state = parsingState::Reading;
                    }
                    break;
                default:
                    break;
            }
            curPos++;
        }
        if (state != parsingState::Reading){
            throw ParserException("Error: wrong field", _currLine, _pos.size());
        }
        if (_pos.size() + 1 != sizeof...(Args)){
            throw ParserException("Error: tuple size != line elements", _currLine, 0);
        }
    }
}
#endif //CSV_CSVPARSER_H
