#include "AdminCommunicationHandler.h"

class AccountCommunicationHandler {
public:

	string cstrpath;

	LoginHandler l;
	
	CodeGenerator codeGen;

	AccountCommunicationHandler(string path);

	bool handle_Get(struct mg_connection *nc, std::string IP, LoginHandler *auth);

	bool handle_Post(struct mg_connection *nc, std::string IP);

	int handle_Login(struct mg_connection *nc, std::string ip);

	int handle_Registration(struct mg_connection *nc);

	int handle_NewPassword(struct mg_connection *nc, std::string code);

};
