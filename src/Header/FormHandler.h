#include "FileHandler.h"
#include <ctype.h>

#define JUST_ONE_TRY 1

inline
void sendRedirectWithCookie(struct mg_connection *nc, string redirect_url, string cookie) {
    mg_printf(nc,
              ("HTTP/1.1 303 OK\r\nSet-Cookie: " + cookie + "\r\n" + "Location: " + redirect_url + "\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n").c_str());
}

class FormHandler : public CivetHandler  {
	public:
	
	LoginHandler auth;
	
	CodeGenerator generator;
	
	CodeGenerator authCodeGen;
	
	FormGenerator form;
	
	CodeGenerator gen;
	
	GarbageCollector garbage;
	
	string cstrpath, savefilePath, emailAddressesPath;
	
	std::vector<string> EmailAddresses;
	
	std::vector<string> AccountIDs;
	
	std::vector<string> inactiveAccountIDs;
	
	std::vector<string> AccountEmailAddresses;
	
	std::vector<string> oldAccountEmailAddresses;
	
	std::vector<std::vector<string>> variableData;
	
	std::vector<string> allVariableData;
	
	std::vector<string> PDFFiles;
	
	bool initHandler(string pathToSavefile, string pathToAddresses, string pathToFormTXT, string pathToFormExplanationTXT, string cstrpath, CodeGenerator *gen);
	
	void updateAddresses();
	
	bool handleGet(CivetServer *server, struct mg_connection *nc);
	
	bool handlePost(CivetServer *server, struct mg_connection *nc);
	
	int checkAccountID(struct mg_connection *nc);
	
	int checkInactiveAccountID(struct mg_connection *nc);
	
	bool checkEmail(struct mg_connection *nc);
	
	void deleteEmail(string emailAddress);
	
	void handleForm(struct mg_connection *nc, int id, string lang);
	
	void handleConfirmForm(struct mg_connection *nc, int id);
	
	void saveInSavefile();
};
