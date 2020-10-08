#include "Header/AccountCommunicationHandler.h"

AccountCommunicationHandler::AccountCommunicationHandler(string path) {
    cstrpath = path;
    l = LoginHandler(get_local_path() + "Data");
}

bool AccountCommunicationHandler::handle_Get(struct mg_connection *nc, string IP, LoginHandler *auth) {
    string index_prefix = "/index";
    string failed_prefix = "/user_login_failed_password";
    string admin_page_prefix = "/admin_page.html";
    string new_password_prefix = "/new_password.html";
    string register_prefix = "/register.html";
    string language_prefix = "/lang";
    string main_page_prefix = "/main_page";

    const char *cookie = mg_get_header(nc, "Cookie");
    char AccountCode[48];
    mg_get_cookie(cookie, "code", AccountCode, sizeof(AccountCode));

    char FormCode[48];
    mg_get_cookie(cookie, "formcode", FormCode, sizeof(FormCode));

    char LanguageCode[48];
    mg_get_cookie(cookie, "lang", LanguageCode, sizeof(LanguageCode));
    string languageCode = string(LanguageCode);

    if (languageCode == "ger")
        languageCode = "";

    const struct mg_request_info *req_info = mg_get_request_info(nc);
    std::string localUri = req_info->local_uri;

    // user login
    if (localUri == admin_page_prefix) {
        if (l.checkLoginCode(AccountCode)) {
            if (l.isAdmin(AccountCode)) {
                mg_send_file(nc, (cstrpath + "admin_page.html").c_str());
            }
            else {
                mg_send_file(nc, (cstrpath + "account_page.html").c_str());
            }
        }
        else {
            mg_send_http_redirect(nc, ("index" + languageCode + ".html").c_str(), 303);
        }
        return true;
    }
    // sends respective main_page based on authorisation level (admin gets 'admin_page')
    else if (has_prefix(&localUri, &main_page_prefix)) {
        if (l.checkLoginCode(AccountCode)) {
            if (l.isAdmin(AccountCode)) {
                mg_send_file(nc, (cstrpath + "admin_page.html").c_str());
            }
            else {
                mg_send_file(nc, (cstrpath + "account_page.html").c_str());
            }
        }
        else if (auth->checkLoginCode(FormCode)) {
            mg_send_file(nc, (cstrpath + "main_page" + languageCode + ".html").c_str());
        }
        else {
            mg_send_http_redirect(nc, ("index" + languageCode + ".html").c_str(), 303);
        }
        return true;
    }
    else if (has_prefix(&localUri, &failed_prefix)) {
        mg_send_file(nc, (cstrpath + "user_login_failed_password" + languageCode + ".html").c_str());
    }
    else if (has_prefix(&localUri, &index_prefix) || localUri == "/") {
        mg_send_file(nc, (cstrpath + "index" + languageCode + ".html").c_str());
    }
    else if (localUri == new_password_prefix) {
        if (l.checkLoginCode(AccountCode)) {
            mg_send_file(nc, (cstrpath + "new_password.html").c_str());
        }
        return true;
    }
    // registration of new accounts is only possible if user is admin
    else if (localUri == register_prefix) {
        if (l.isAdmin(AccountCode)) {
            mg_send_file(nc, (cstrpath + "register.html").c_str());
        }
        else {
            mg_send_http_redirect(nc, ("index" + languageCode + ".html").c_str(), 303);
        }
        return true;
    }
    // sends respective index page based on language code
    else if (has_prefix(&localUri, &language_prefix)) {
        string lang = getVariable(localUri, "lang", '.');
        languageCode = lang;
        if (languageCode == "ger")
            languageCode = "";
        sendRedirectWithCookie(nc, "index" + languageCode + ".html", "lang=" + lang + "; Secure");
    }
    return false;
}

bool AccountCommunicationHandler::handle_Post(struct mg_connection *nc, string IP) {

    // PC
    string login_prefix = "/login";
    string registration_prefix = "/registration_page";
    string logout_prefix = "/logout_page";
    string new_password_prefix = "/new_password";

    const char *cookie = mg_get_header(nc, "Cookie");
    char AccountCodeChar[48];
    mg_get_cookie(cookie, "code", AccountCodeChar, sizeof(AccountCodeChar));
    string AccountCode = string(AccountCodeChar);

    const struct mg_request_info *req_info = mg_get_request_info(nc);
    std::string localUri = req_info->local_uri;
    std::cout << "Uri: " << localUri << std::endl;

    // handles account login
    if (localUri == login_prefix) {
        std::cout << "IP login: " << IP << std::endl;
        int mode = handle_Login(nc, IP);
        AccountCode = codeGen.getCode();
        if (mode == 1) {
            // login page based on authorisation level
            if (l.isAdmin(AccountCode))
                sendRedirectWithCookie(nc, "login_admin.html", "code=" + AccountCode + "; Secure");
            else
                sendRedirectWithCookie(nc, "login_user.html", "code=" + AccountCode + "; Secure");
        }
        else {
            // login failed page based on reason of login failure
            if (mode == 0)
                mg_send_http_redirect(nc, "login_failed_password.html", 303);
            else if (mode == 2)
                mg_send_http_redirect(nc, "login_failed_already_locked_in.html", 303);
            else
                mg_send_http_redirect(nc, "login_failed_account_locked.html", 303);
        }
        return true;
    }
    // handles account registration (only possible if user is admin)
    else if (localUri == registration_prefix && l.isAdmin(AccountCode)) {
        int error = handle_Registration(nc);
        if (error == 0)
            mg_send_http_redirect(nc, "index.html", 303);
        else
            mg_send_http_redirect(nc, "register_failed.html", 303);

        return true;
    }
    // handles logout
    else if (localUri == logout_prefix) {
        l.executeLogout(AccountCode);
        mg_send_http_redirect(nc, "index.html", 303);
        return true;
    }
    // handles password change
    else if (localUri == new_password_prefix) {
        std::cout << "IP password change: " << IP << std::endl;
        int mode = handle_NewPassword(nc, AccountCode);
        if (mode == 1) {
            if (l.isAdmin(AccountCode))
                mg_send_http_redirect(nc, "admin_page.html", 303);
            else
                mg_send_http_redirect(nc, "account_page.html", 303);
        }
        else {
            mg_send_http_redirect(nc, "new_password_failed.html", 303);
        }
        return true;
    }

    return false;
}

int AccountCommunicationHandler::handle_Login(struct mg_connection *nc, string ip) {
    std::cout << "\n**Getting POST Data**\n\n";

    char post_data[10 * 1024];
    int post_data_len = mg_read(nc, post_data, sizeof(post_data));

    char param1[1024];
    char param2[1024];

    mg_get_var(post_data, post_data_len, "uname", param1, sizeof(param1));
    mg_get_var(post_data, post_data_len, "psw", param2, sizeof(param2));

    codeGen.generateCode(time(NULL), 40);

    return l.checkLoginData(param1, param2, codeGen.getCode());
}

int AccountCommunicationHandler::handle_Registration(struct mg_connection *nc) {
    std::cout << "\n**Getting POST Data**\n\n";

    char post_data[10 * 1024];
    int post_data_len = mg_read(nc, post_data, sizeof(post_data));

    char param1[1024];
    char param2[1024];

    mg_get_var(post_data, post_data_len, "uname", param1, sizeof(param1));
    mg_get_var(post_data, post_data_len, "psw", param2, sizeof(param2));

    int error = l.addLoginData(param1, param2, 0);

    return error;
}

int AccountCommunicationHandler::handle_NewPassword(struct mg_connection *nc, string code) {
    std::cout << "\n**Getting POST Data**\n\n";

    char post_data[10 * 1024];
    int post_data_len = mg_read(nc, post_data, sizeof(post_data));

    char param1[1024];
    char param2[1024];

    mg_get_var(post_data, post_data_len, "oldpsw", param1, sizeof(param1));
    mg_get_var(post_data, post_data_len, "newpsw", param2, sizeof(param2));

    string accountName = l.getAccountName(code);

    if (!l.checkPassword(accountName, param1))
        return 0;

    if (!l.newPassword(l.getAccountID(accountName), param2))
        return 0;

    return 1;
}
