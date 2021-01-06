#include <iostream>
#include "Parser.h"

int main(){
    try{
        std::ifstream input;
        input.open("input2.csv");
        if (!input.is_open()){
            throw std::runtime_error("Cannot open the file!");
        }
        csv::Parser<int, std::string, std::string, std::string, double> parser(input, 1);
        for (std::tuple<int, std::string, std::string, std::string, double> rs : parser){
            std::cout << rs << std::endl;
        }
    }
    catch (std::exception &e){
        std::cout << e.what() << std::endl;
    }
}