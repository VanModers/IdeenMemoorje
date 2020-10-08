#include "Header/LoginHandler.h"
#include <openssl/evp.h>

#define KEY_LEN      32
#define KEK_KEY_LEN  20
#define ITERATION     1

#define MAX_AUTH 50

// Hash function for password hashing
string pwdToHash(const char* pwd, string salt) {
    string hash_pwd;
    unsigned char *out;
    unsigned char salt_value[salt.length()];
    strcpy((char*) salt_value, salt.c_str());

    out = (unsigned char *) malloc(sizeof(unsigned char) * KEK_KEY_LEN);

    if( PKCS5_PBKDF2_HMAC_SHA1(pwd, strlen(pwd), salt_value, sizeof(salt_value), ITERATION, KEK_KEY_LEN, out) != 0 )
    {
        std::stringstream s;
        for (int i = 0; i < KEK_KEY_LEN; ++i)
            s << std::hex << std::setfill('0') << std::setw(2) << (unsigned short) out[i];
        hash_pwd = s.str();
    }
    else
    {
        fprintf(stderr, "PKCS5_PBKDF2_HMAC_SHA1 failed\n");
    }

    return hash_pwd;
}

LoginHandler::LoginHandler(string loginpath) {
    path = loginpath + "/LOGINDATA.data";
    cout << "Path: " << path << "\n";
    cout << "Start LoginHandler\n";

    // if there is no password file, one is created
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        cout << "No LOGININFO-file in " << path << " found. Creating one.\n";
        gen.generateCode(time(NULL), 7);
        ofstream outfile(path);
        outfile << "Admin" << endl;
        outfile << pwdToHash("Adminpas", gen.getCode()) << endl;
        outfile << gen.getCode() << endl;
        outfile << "1" << endl;
        outfile.close();
        string str;
        ifstream ifs(path, ifstream::in);
        cout << "File opened\n";
        while (getline(ifs, str)) {
            LoginData.push_back(str);
            str = "";
        }
    }
    else {
        // otherwise the existing password file is loaded in
        string str;
        ifstream ifs(path, ifstream::in);
        cout << "File opened\n";
        while (getline(ifs, str)) {
            LoginData.push_back(str);
            str = "";
        }
    }

}
// Structure of LOGININFO-file
// 1. line: username
// 2. line: hashed password
// 3. line: salt for password
// 4. line: authorisation level of user

// Function for password checking
bool LoginHandler::checkPassword(string uname, string password) {
    for (int i = 0; i < int(LoginData.size()); i += 4) {
        if (uname == LoginData[i] && pwdToHash(password.c_str(), LoginData[i + 2]) == LoginData[i + 1])
            return true;
    }

    return false;
}

// adds authorisation code
int LoginHandler::initAuthorisationCode(string code) {
    cout << "Authorisating..." << endl;
    AuthorisationCodes.push_back(code);

    if(AuthorisationCodes.size() > MAX_AUTH)
        AuthorisationCodes.erase (AuthorisationCodes.begin());

    return 0;
}

// removes old authorisation codes from array
int LoginHandler::removeAuthorisationCode(string code) {
    for(int i = 0; i < AuthorisationCodes.size(); i++) {
        if(code == AuthorisationCodes[i]) {
            AuthorisationCodes.erase (AuthorisationCodes.begin()+i);
            return 0;
        }
    }
    return 1;
}

// checks login data
// handles login of password and user name are correct
int LoginHandler::checkLoginData(string uname, string password, string code) {
    for (int i = 0; i < LoginData.size(); i += 4) {
        if (uname == LoginData[i] && pwdToHash(password.c_str(), LoginData[i + 2]) == LoginData[i + 1]) {
            int error;
            if (setLoginCode(code, uname, &error)) {
                cout << "Login of " << uname << "\n";
                return 1;
            }
            else {
                return (error + 1);
            }
        }
    }

    cout << "Login of " << uname << " failed\n";
    return 0;
}

// checks whether user submitted cookie code is authorised
bool LoginHandler::checkLoginCode(string code) {
    for (int i = 0; i < AccountCodes.size(); i++) {
        if (AccountCodes[i][0] == code) {
            return true;
        }
    }

    for (int i = 0; i < AuthorisationCodes.size(); i++) {
        if (AuthorisationCodes[i] == code) {
            return true;
        }
    }

    return false;
}

// function for setting a user-specific cookie code
bool LoginHandler::setLoginCode(string code, string uname, int* error) {
    for (int i = 0; i < AccountCodes.size(); i++) {
        if (AccountCodes[i][0] == code && AccountCodes[i][1] == uname) {
            *error = 0;
            cout << "User is already locked in on this account. IP login\n";
            return true;
        }

        if (AccountCodes[i][0] == code) {
            *error = 1;
            cout << "Login failed. IP already locked in\n";
            return false;
        }
        if (AccountCodes[i][1] == uname) {
            cout << "Another user is already locked in. He's being logged out\n";
            executeLogout(AccountCodes[i][0]);
        }
    }
    AccountCodes.push_back(vector<string>(2));
    AccountCodes[AccountCodes.size() - 1][0] = code;
    AccountCodes[AccountCodes.size() - 1][1] = uname;

    cout << "IP login\n";

    return true;
}

void LoginHandler::executeLogout(string code) {
    int id;
    for (int i = 0; i < AccountCodes.size(); i++) {
        if (AccountCodes[i][0] == code) {
            cout << "Logout of " << AccountCodes[i][1] << endl;
            AccountCodes.erase(AccountCodes.begin() + i);
        }
    }
}

// saves new login data into Login-file
void LoginHandler::Update() {
    ofstream outfile(path);
    for (int i = 0; i < LoginData.size(); i++) {
        outfile << LoginData[i] << endl;
    }
    outfile.close();
}

bool LoginHandler::checkIfExists(string uname) {
    for (int i = 0; i < int(LoginData.size()); i += 4) {
        if (LoginData[i] == uname)
            return true;
    }
    return false;
}

// Registration data is stored as following:
// 1. user name
// 2. password hash
// 3. salt key (for password hashing)
// 4. authorisation level of user
int LoginHandler::addLoginData(string uname, string password, int authorisation) {
    cout << "Registration of " << uname << "\n";
    if (!checkIfExists(uname)) {
        gen.generateCode(time(NULL), 7);
        LoginData.push_back(uname);
        LoginData.push_back(pwdToHash(password.c_str(), gen.getCode()));
        LoginData.push_back(gen.getCode());
        LoginData.push_back(to_string(authorisation));
        Update();

        return 0;
    }
    else {
        cout << "Error: Username already exists\n";
        return 1;
    }
}

// removes account
bool LoginHandler::removeAccount(int id) {
    if(id >= 0 && id * 4 < LoginData.size()) {
        cout << "Deleting account called '" << getAccountName(id) << "'" << endl;
        LoginData.erase (LoginData.begin()+id*4 + 3);
        LoginData.erase (LoginData.begin()+id*4 + 2);
        LoginData.erase (LoginData.begin()+id*4 + 1);
        LoginData.erase (LoginData.begin()+id*4);
        Update();

        cout << "Ready" << endl;
        return true;
    }
    cout << "Deleting account called '" << getAccountName(id) << "' failed!" << endl;
}

// changes authorisation level of account:
// level 1 = admin
void LoginHandler::setAccountAuthorizationLevel(int level, int id) {
    if(id >= 0 && id * 4 < LoginData.size()) {
        LoginData[id * 4 + 3] = to_string(level);
        cout << "Changed authorization level of " << LoginData[id * 4] << " to " << to_string(level) << endl;
        Update();
    }
    else {
        cout << "Changed authorization level of " << LoginData[id * 4] << " to " << to_string(level) << " failed!" << endl;
    }
}

// changes password of account with ID 'id'
bool LoginHandler::newPassword(int id, string password) {
    if(id >= 0 && id * 4 < LoginData.size()) {
        LoginData[id * 4 + 1] = pwdToHash(password.c_str(), LoginData[id * 4 + 2]);
        Update();
        return true;
    }
    else {
        return false;
    }
}

// returns account id based on name
int LoginHandler::getAccountID(string name) {
    for(int i = 0; i < LoginData.size(); i += 4) {
        if(LoginData[i] == name)
            return i/4;
    }
    return -1;
}

// return account name based on authorisation code
string LoginHandler::getAccountName(string code) {
    for (int i = 0; i < AccountCodes.size(); i++) {
        if (AccountCodes[i][0] == code) {
            cout << "User: " << AccountCodes[i][1] << endl;
            return AccountCodes[i][1];
        }
    }
    return "";
}

int LoginHandler::getNumberOfAccounts() {
    return LoginData.size() / 4;
}

int LoginHandler::getAccountAuthorizationLevel(int id) {
    if(id >= 0 && id * 4 < LoginData.size())
        return stoi(LoginData[4 * id + 3]);
    else
        return -1;
}

string LoginHandler::getAccountName(int id) {
    cout << id << endl;
    if(id >= 0 && id * 4 < LoginData.size())
        return LoginData[4 * id];
    else
        return "";
}

// function that checks if a specific user has admin rights (level = 1)
bool LoginHandler::isAdmin(string code) {
    int ID = getAccountID(getAccountName(code));
    bool admin = false;
    if(ID > -1 && ID * 4 < LoginData.size() && stoi(LoginData[4 * ID + 3]) == 1)
        admin = true;
    return admin;
}
