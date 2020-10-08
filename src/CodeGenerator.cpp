#include "Header/CodeGenerator.h"
#include <stdlib.h>     /* srand, rand */

CodeGenerator::CodeGenerator(int seed, int length, string path) {
    CodeGenerator::cstrpath = path;

    usedCodes = vector<string>();
    generateCode(seed, length);
}

void CodeGenerator::generateCode(int seed, int length) {
	// Generating new code	
    int i = 0;
    bool newCode = false;
    while(!newCode) {
        srand (seed+i); // Initialising random function with seed
        CodeGenerator::code = "";
        
        for (int i = 0; i < length; i++) {  // Generating pseudorandom code of specific length
            char symbol;
            int symbolID = rand() % 36;
            if(symbolID < 10)
                symbol = char(symbolID) + '0';
            else
                symbol = char(symbolID - 10) + 'A';
            CodeGenerator::code += symbol;
        }

		// Checking if code already exists
        newCode = true;
        for(int j = 0; j < usedCodes.size(); j++) {
            if(usedCodes[j] == CodeGenerator::code) {
                newCode = false;
                j = usedCodes.size();
            }
        }
        i++;
    }
    usedCodes.push_back(CodeGenerator::code);
    if(usedCodes.size() > 200)
        usedCodes.erase (usedCodes.begin());
}

string CodeGenerator::getCode() {
    return CodeGenerator::code;
}

bool CodeGenerator::equalsCode(string candidate) {
    std::cout << "Got code: " << candidate << std::endl;
    if(CodeGenerator::code == candidate)
        return true;
    std::cout << "Wrong" << std::endl;
    return false;
}
