#include "ConsoleHandler.h"

inline
static int has_prefix(string *uri, string *prefix) {
	return uri->size() >= prefix->size() && memcmp(uri->c_str(), prefix->c_str(), prefix->size()) == 0;
}

inline
string getVariable(string message_head, string variable, char seperator) {
	string name = "";
	size_t pos = message_head.rfind(variable);
	pos += variable.size();
	for (int i = pos; i < message_head.length(); i++) {
		if (message_head.at(i) == seperator)
			break;
		else
			name += message_head.at(i);
	}
	return name;
}

inline
int getNumber(string message, string head, char seperator) {
	string Number = "";
	for (int i = head.length(); i < message.length(); i++) {
		if (message.at(i) == seperator)
			break;
		else
			Number += message.at(i);
	}
	return stoi(Number);
}

class FileHandler : public CivetHandler {
	public:
	
	LoginHandler *login;
	
	vector<string> Files;
	
	string path;
	
	void initHandler(LoginHandler *l, string path);
	
	void updateFiles();
	
	void uploadFiles(struct mg_connection *nc);
	
	bool handleGet(CivetServer *server, struct mg_connection *nc);
	
	bool handlePost(CivetServer *server, struct mg_connection *nc);
	
	int findChar(char* array);

};
