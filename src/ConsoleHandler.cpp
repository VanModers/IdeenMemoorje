#include "Header/ConsoleHandler.h"

ConsoleHandler::ConsoleHandler(string pathToSavefile, string pathToAddresses, string pathToFormTXT, string path) {
    addressesPath = pathToAddresses;
    formTXTPath = pathToFormTXT;
    savefilePath = pathToSavefile;
    cstrpath = path;

    output = "";
}

vector<string> splitString(string str, char seperator) {
	vector<string> elements;
	string element;
	for (int i = 0; i < str.length(); i++) {
		if (str.at(i) == seperator) {
			elements.push_back(element);
			element = "";
			while(i+1 < str.length() && str.at(i+1) == ' ')
				i++;
		}
		else {
			element += str.at(i);
		}
	}
	elements.push_back(element);
	return elements;
}

void ConsoleHandler::processRequest(string request, FormGenerator gen) {
    if(request == "countSubmissions") {
        ifstream ifs(savefilePath, ifstream::in);

        int submissions = 0;
        std::string str;
        bool openQuotation = false;
        while (getline(ifs, str)) {
			for(int i = 0; i < str.length(); i++) {
				if(str.at(i) == '"') {
					if(openQuotation)
						openQuotation = false;
					else
						openQuotation = true;
				}
			}
			if(openQuotation == false)
				submissions++;
        }
        output = to_string(submissions);
    }
    else if(request == "countAddresses") {
        ifstream ifs(addressesPath, ifstream::in);

        int addresses = 0;
        std::string str;
        while (getline(ifs, str)) {
            addresses++;
        }
        output = to_string(addresses);
    }
    else if(request == "saveNames") {
		ofstream outfile("EventData/names.txt");
		
		cout << gen.NumberOfVariables << endl;
		cout << gen.namefieldPos << endl;
		
        ifstream ifs(savefilePath, ifstream::in);
        std::string str;
        int variableID = 0;
        string name = "";
        while (getline(ifs, str)) {
			for(int i = 0; i < str.length(); i++) {
				if(str.at(i) == SEPERATOR) {
					variableID++;
					i++;
				}
				else if(variableID == gen.namefieldPos && str.at(i) != '\"') {
					name += str.at(i);
				}
			}
			if(variableID == gen.NumberOfVariables) {
				vector<string> names = splitString(name, ',');
				for(int i = 0; i < names.size(); i++)
					outfile << names[i] << endl;
				variableID = 0;
				name = "";
			}
        }
        outfile.close();
        output = "Finished successfully";
    }
}

bool ConsoleHandler::handle_Post(struct mg_connection *nc, LoginHandler *l, FormGenerator gen)
{
    try {
        string output_prefix = "/output";
        string request_prefix = "/request";

        const struct mg_request_info *req_info = mg_get_request_info(nc);
        std::string localUri = req_info->local_uri;

        const char *cookie = mg_get_header(nc, "Cookie");
        char AccountCodeChar[48];
        mg_get_cookie(cookie, "code", AccountCodeChar, sizeof(AccountCodeChar));
        string AccountCode = string(AccountCodeChar);

        if (localUri == output_prefix && l->checkLoginCode(AccountCode) && l->isAdmin(AccountCode)) {
            cout << "Sending: " << output << "\n";
            string send = output;

            mg_send_http_ok(nc, "Content-Type: text/html; charset=utf-8", strlen(send.c_str()));
            mg_printf(nc, send.c_str());
        }
        else if (localUri == request_prefix && l->checkLoginCode(AccountCode) && l->isAdmin(AccountCode)) {
            char post_data[10 * 1024];
            int post_data_len = mg_read(nc, post_data, sizeof(post_data));

            char request[1024];

            mg_get_var(post_data, post_data_len, "txt", request, sizeof(request));

            cout << "Got Request: " << request << std::endl;

            processRequest(request, gen);
            mg_send_file(nc, (cstrpath + "admin_page.html").c_str());
        }

        return true;
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Caught unknown exception." << std::endl;
    }
    return false;
}
