#include "FormHandler.h"

inline
std::string get_local_path()
{
	return "";
}

class AdminCommunicationHandler {
	public:
	
	string cstrpath;
	
	AdminCommunicationHandler(string path);

	bool handle_Get(struct mg_connection *nc, std::string IP, LoginHandler l);

	bool handle_Post(struct mg_connection *nc, std::string IP, LoginHandler *l);
	
	void handle_Authorization_Change(struct mg_connection *nc, std::string IP, LoginHandler *l);
	
	void handle_Account_Removal(struct mg_connection *nc, std::string IP, LoginHandler *l);
};
