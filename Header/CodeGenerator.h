#include <sys/stat.h>  
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "src/CivetServer.h"
#include <cstring>

using namespace std;

class CodeGenerator {
	private: 
	string code;
	
	public:
	string cstrpath;
	vector<string> usedCodes;
	
	CodeGenerator(int seed, int length, string cstrpath);
	CodeGenerator() {};
	
	void generateCode(int seed, int length);
	
	string getCode();
	
	bool equalsCode(string cadidate);
	
};
