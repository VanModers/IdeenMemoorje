#include "GarbageCollector.h"
static string SEPARATOR = "$";

class FormGenerator {
	public:
	
	int namefieldPos, NumberOfVariables;
	
	string cstrpath;
	
	std::vector<string> form_options, explanationText;
	
	FormGenerator(std::vector<string> options, std::vector<string> explanationText, string cstrpath);
	
	FormGenerator(string pathToForm, string pathToFormExplanation, string cstrpath);
	
	FormGenerator() {};
	
	bool saveHTML(string filepath);
	
	std::vector<string> SplitString(string str, string seperator);
	
	void generateHTMLFile(string path, std::vector<string> options, std::vector<string> explanationText);
	
	void generateHTMLCheckFile(string path, std::vector<string> variables, string lang);
	
};

std::istream& safeGetline(std::istream& is, std::string& t);
