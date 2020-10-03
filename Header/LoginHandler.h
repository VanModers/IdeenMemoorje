#include "CodeGenerator.h"

class LoginHandler {
public:

	CodeGenerator gen;

	vector<string> LoginData;
	
	vector<vector<string>> AccountCodes;
	
	vector<string> AuthorisationCodes;
	
	vector<string> AccountLanguages;

	string path;

	LoginHandler() {}

	LoginHandler(string loginpath);

	bool checkPassword(string uname, string password);
	
	int initAuthorisationCode(string code);
	
	int removeAuthorisationCode(string code);

	int checkLoginData(string uname, string password, string code);

	bool checkLoginCode(string code);

	bool setLoginCode(string code, string uname, int* error);
	
	void setAccountAuthorizationLevel(int level ,int id);

	void executeLogout(string code);

	void Update();

	bool checkIfExists(string uname);

	int addLoginData(string uname, string password, int authorisation);
	
	bool removeAccount(int id);

	bool newPassword(int id, string password);

	int getAccountID(string name);

	string getAccountName(string code);

	int getNumberOfAccounts();
	
	int getAccountAuthorizationLevel(int id);
	
	string getAccountName(int id);

	bool isAdmin(string ip);

};
