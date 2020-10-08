#include "Header/FormHandler.h"

string handleQuotationMarks(string str) {
    int length = str.length();
    for(int i = 0; i < length; i++) {
        if(str.at(i) == '"') {
            str.insert(i,"\"");
            length = str.length();
            i++;
        }
    }
    return str;
}

string toLowerCase(string str) {
    string out;
    for(int i = 0; i < str.length(); i++) {
        out += tolower(str.at(i));
    }
    return out;
}

void FormHandler::saveInSavefile() {
    // saves data of submitted forms
    ofstream outfile(savefilePath);
    for(int i = 0; i < allVariableData.size(); i++) {
        outfile << allVariableData[i] << endl;
    }
    outfile.close();
}

bool FormHandler::checkEmail(struct mg_connection *nc) {
    char post_data[10 * 1024];
    int post_data_len = mg_read(nc, post_data, sizeof(post_data));

    const struct mg_request_info *req_info = mg_get_request_info(nc);
    char IP[48];
    strncpy(IP,req_info->remote_addr, 48);

    char param1[1024];

    // checks if email is correct
    mg_get_var(post_data, post_data_len, "email", param1, sizeof(param1));
    string inputEmail = toLowerCase(string(param1));
    std::size_t pos = inputEmail.find("@");

    if(pos == string::npos)
        inputEmail += "@ulricianum-aurich.de";

    for(int i = 0; i < EmailAddresses.size(); i++) {
        if(EmailAddresses[i] == inputEmail) {
            AccountEmailAddresses.push_back(EmailAddresses[i]);
            return true;
        }
    }

    return false;
}

void FormHandler::deleteEmail(string emailAddress) {
    // deletes email from memory
    for(int i = 0; i < EmailAddresses.size(); i++) {
        if(EmailAddresses[i] == emailAddress) {
            EmailAddresses.erase(EmailAddresses.begin() + i);
        }
    }
}

int FormHandler::checkAccountID(struct mg_connection *nc) {
    const char *cookie = mg_get_header(nc, "Cookie");
    char AccountID[48];
    mg_get_cookie(cookie, "id", AccountID, sizeof(AccountID));

    for(int i = 0; i < AccountIDs.size(); i++) {
        if(AccountIDs[i] == AccountID) {
            return i;
        }
    }

    return -1;
}

int FormHandler::checkInactiveAccountID(struct mg_connection *nc) {
    const char *cookie = mg_get_header(nc, "Cookie");
    char AccountID[48];
    mg_get_cookie(cookie, "id", AccountID, sizeof(AccountID));

    for(int i = 0; i < inactiveAccountIDs.size(); i++) {
        if(inactiveAccountIDs[i] == AccountID) {
            return i;
        }
    }

    return -1;
}

void FormHandler::handleForm(struct mg_connection *nc, int id, string lang) {
    char post_data[((int)form.form_options.size()-1)/2 * 1024];
    int post_data_len = mg_read(nc, post_data, sizeof(post_data));

    cout << "Received form:" << endl;

    int varID = 0;
    vector<string> variables;
    for(int i = 1; i < form.form_options.size() - 1; i+=2) {
        if(form.form_options[i] != "checkbox-end") {
            int j = 0;
            int out = 1;
            string wholeVar = "";
            while(out > 0) {
                char var[1024];
                out = mg_get_var2(post_data, post_data_len, ("var"+to_string(varID)).c_str(), var, sizeof(var), j);
                if(out > 0) {
                    if(j > 0)
                        wholeVar += ",";
                    wholeVar += string(var);
                }
                j++;
            }
            if(form.form_options[i] == "checkbox-display" && wholeVar == "")
                wholeVar = "false";
            variables.push_back(wholeVar);
            varID++;
        }
        else {
            i--;
        }
    }
    variableData[id] = variables;

    gen.generateCode(time(NULL),9);
    form.generateHTMLCheckFile("temp/"+gen.getCode()+".html", variables, lang);
    garbage.addFile("temp/"+gen.getCode()+".html");
    mg_send_file(nc, ("temp/"+gen.getCode()+".html").c_str());
}

void FormHandler::handleConfirmForm(struct mg_connection *nc, int id) {
    string data = "";
    if(variableData[id].size() > 0) {
        data = "\"" + handleQuotationMarks(variableData[id][0]);
        for(int i = 1; i < variableData[id].size(); i++) {
            data += "\"" + SEPARATOR + "\"" + handleQuotationMarks(variableData[id][i]);
        }
    }
    cout << data << endl;
    data += "\"" + SEPARATOR + AccountEmailAddresses[id];
    allVariableData.push_back(data);

    // JUST_ONE_TRY is true when every user can also submit a form once
    if(JUST_ONE_TRY) {
        inactiveAccountIDs.push_back(AccountIDs[id]);
        oldAccountEmailAddresses.push_back(AccountEmailAddresses[id]);
        deleteEmail(AccountEmailAddresses[id]);
        AccountEmailAddresses.erase(AccountEmailAddresses.begin() + id);
        AccountIDs.erase(AccountIDs.begin() + id);
        PDFCreator pdf("temp/");
        PDFFiles.push_back(pdf.generateTempPDFDocumentFromOptions(form.form_options, variableData[id]));
        cout << "Generated PDF!" << endl;
        variableData.erase(variableData.begin() + id);
        garbage.addFile(PDFFiles[PDFFiles.size()-1]);
        updateAddresses();
    }
    else {
        PDFCreator pdf("temp/");
        PDFFiles[id] = pdf.generateTempPDFDocumentFromOptions(form.form_options, variableData[id]);
        cout << "Generated PDF!" << endl;
        garbage.addFile(PDFFiles[id]);
    }

    mg_send_http_redirect(nc, "endform.html", 303);

    saveInSavefile();
}

bool FormHandler::initHandler(string pathToSavefile, string pathToAddresses, string pathToFormTXT, string pathToFormExplanationTXT, string path, CodeGenerator *gen) {
    FormHandler::savefilePath = pathToSavefile;
    FormHandler::cstrpath = path;
    FormHandler::emailAddressesPath = pathToAddresses;

    form = FormGenerator(pathToFormTXT, pathToFormExplanationTXT, cstrpath);
    generator = *gen;
    std::cout << "https://projektwoche.ddns.net/login_form_user?code=" << generator.getCode() << std::endl;

    auth = LoginHandler();

    EmailAddresses = std::vector<string>();
    AccountIDs = std::vector<string>();
    variableData = std::vector<std::vector<string>>();
    PDFFiles = std::vector<string>();
    allVariableData = std::vector<string>();
    AccountEmailAddresses = std::vector<string>();
    oldAccountEmailAddresses = std::vector<string>();
    inactiveAccountIDs = std::vector<string>();

    garbage = GarbageCollector(std::vector<string>(), 200);

    ifstream ifs(emailAddressesPath, ifstream::in);
    cout << "Email addresses file opened\n";

    std::string str;
    while (!safeGetline(ifs, str).eof()) {
        EmailAddresses.push_back(str);
    }

    ifstream savefile(savefilePath, ifstream::in);
    while (!safeGetline(savefile, str).eof()) {
        allVariableData.push_back(str);
        str = "";
    }

    return true;
}

void FormHandler::updateAddresses() {
    ofstream outfile(emailAddressesPath);
    for(int i = 0; i < EmailAddresses.size(); i++) {
        outfile << EmailAddresses[i] << endl;
    }
    outfile.close();
}

bool FormHandler::handleGet(CivetServer *server, struct mg_connection *nc) {
    try {
        string form_prefix = "/form.html";
        string email_failed_prefix = "/wrong_email_main_page";
        string user_login_prefix = "/login_form_user";
        string endform_prefix = "/endform";
        string language_prefix = "/2lang";

        const struct mg_request_info *req_info = mg_get_request_info(nc);
        std::string localUri = req_info->local_uri;
        const char *cookie = mg_get_header(nc, "Cookie");
        char FormCode[48];
        mg_get_cookie(cookie, "formcode", FormCode, sizeof(FormCode));

        // getting language code of user from cookie
        char LanguageCode[48];
        mg_get_cookie(cookie, "lang", LanguageCode, sizeof(LanguageCode));
        string languageCode = string(LanguageCode);

        if(languageCode == "ger")
            languageCode = "";

        if(localUri == form_prefix) {
            if(checkAccountID(nc) > -1)
                mg_send_file(nc, (cstrpath +"form.html").c_str());
            else
                mg_send_http_redirect(nc, "index.html", 303);
            return true;
        }
        else if (has_prefix(&localUri, &endform_prefix)) {
            mg_send_file(nc, (cstrpath +"endform" + languageCode + ".html").c_str());
            return true;
        }
        // handle login of user by code submission
        else if (has_prefix(&localUri, &user_login_prefix)) {
            char param1[1024];
            mg_get_var(req_info->query_string, strlen(req_info->query_string), "code", param1, sizeof(param1));
            if(generator.equalsCode(string(param1))) {
                authCodeGen.generateCode(time(NULL), 40);
                auth.initAuthorisationCode(authCodeGen.getCode());
                sendRedirectWithCookie(nc, "main_page" + languageCode + ".html", "formcode="+ authCodeGen.getCode() +"; Secure");
            }
            else {
                mg_send_http_redirect(nc, "user_login_failed_password.html", 303);
            }
            return true;
        }
        else if (has_prefix(&localUri, &email_failed_prefix) && auth.checkLoginCode(FormCode) && !auth.isAdmin(FormCode)) {
            mg_send_file(nc, (cstrpath +"wrong_email_main_page" + languageCode + ".html").c_str());
            return true;
        }
        else if(has_prefix(&localUri, &language_prefix)) {
            string lang = getVariable(localUri, "2lang", '.');
            languageCode = lang;
            // standard language code is German
            if(languageCode == "ger")
                languageCode = "";
            if (auth.checkLoginCode(FormCode))
                sendRedirectWithCookie(nc, "main_page" + languageCode + ".html", "lang="+ lang +"; Secure");
            else
                sendRedirectWithCookie(nc, "index" + languageCode + ".html", "lang="+ lang +"; Secure");
            return true;
        }

    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Caught unknown exception." << std::endl;
    }


    return false;
}

bool FormHandler::handlePost(CivetServer *server, struct mg_connection *nc) {
    try {
        string email_prefix = "/email";
        string form_prefix = "/form";
        string confirm_prefix = "/confirm";
        string download_prefix = "/download";
        string get_name_prefix = "/name";

        const struct mg_request_info *req_info = mg_get_request_info(nc);
        std::string localUri = req_info->local_uri;
        const char *cookie = mg_get_header(nc, "Cookie");
        char FormCode[48];
        mg_get_cookie(cookie, "formcode", FormCode, sizeof(FormCode));

        // getting language code of user from cookie
        char LanguageCode[48];
        mg_get_cookie(cookie, "lang", LanguageCode, sizeof(LanguageCode));
        string languageCode = string(LanguageCode);

        if(languageCode == "ger")
            languageCode = "";

        int id = checkAccountID(nc);

        if(localUri == email_prefix && auth.checkLoginCode(FormCode) && !auth.isAdmin(FormCode)) {
            if(checkEmail(nc)) {
                gen.generateCode(time(NULL),15); // user specific code is generated
                mg_printf(nc,
                          ("HTTP/1.1 303 OK\r\nSet-Cookie: id=" + gen.getCode() + "; Secure\r\n" + "Location: form.html" + "\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n").c_str()); // code is sent as a cookie
                AccountIDs.push_back(gen.getCode()); // cookie is saved in AccountIDs
                variableData.push_back(vector<string>(0));
                if(!JUST_ONE_TRY)
                    PDFFiles.push_back("");
            }
            else {
                mg_send_http_redirect(nc, "wrong_email_main_page.html", 303);
            }

            return true;
        }
        else if(localUri == form_prefix) {
            if(id > -1) {
                handleForm(nc, id, languageCode);
            }
            else {
                mg_send_http_redirect(nc, "index.html", 303);
            }
            return true;
        }
        else if(localUri == confirm_prefix) {
            if(id > -1)
                handleConfirmForm(nc, id);
            else
                mg_send_http_redirect(nc, "index.html", 303);
            return true;
        }
        else if(localUri == download_prefix) {
            int id = 0;
            if(JUST_ONE_TRY)
                id = checkInactiveAccountID(nc);
            else
                id = checkAccountID(nc);
            if(id > -1) {
                cout << "Download" << endl;
                mg_send_mime_file2(nc, PDFFiles[id].c_str(), NULL, ("Content-Disposition: attachment; filename=" + oldAccountEmailAddresses[id] + ".pdf").c_str());

            }
            else {
                mg_send_http_redirect(nc, "index.html", 303);
            }
        }
        /* function responsible for automated insertion of user name into form */
        else if(localUri == get_name_prefix && id > -1) {
            std::size_t pos = AccountEmailAddresses[id].find("@");
            string send = AccountEmailAddresses[id].substr(0,pos);
            if(variableData[id].size() > 0 && form.namefieldPos > -1)
                send = variableData[id][form.namefieldPos];

            mg_send_http_ok(nc, "Content-Type: text/html; charset=utf-8", strlen(send.c_str()));
            mg_printf(nc, send.c_str());
        }
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Caught unknown exception." << std::endl;
    }

    return false;
}
