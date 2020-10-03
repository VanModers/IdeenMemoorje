#include "PDFCreator.h"

class ConsoleHandler : public CivetHandler {
	public:
	
	string addressesPath, formTXTPath, savefilePath;
	
	string cstrpath;
	
	string output;
	
	ConsoleHandler(string pathToSavefile, string pathToAddresses, string pathToFormTXT, string path);
	
	void processRequest(string request, FormGenerator gen);
	
	bool handle_Post(struct mg_connection *nc, LoginHandler *l, FormGenerator gen);
};
