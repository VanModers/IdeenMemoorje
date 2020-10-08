#include "Header/AdminCommunicationHandler.h"

AdminCommunicationHandler::AdminCommunicationHandler(string path) {
    cstrpath = path;
}

bool AdminCommunicationHandler::handle_Get(struct mg_connection *nc, std::string IP, LoginHandler l) {
    string get_devices_prefix = "/get_device";

    const char *cookie = mg_get_header(nc, "Cookie");
    char AccountCodeChar[48];
    mg_get_cookie(cookie, "code", AccountCodeChar, sizeof(AccountCodeChar));
    string AccountCode = string(AccountCodeChar);

    const struct mg_request_info *req_info = mg_get_request_info(nc);
    std::string localUri = req_info->local_uri;

    // return list of accounts (also called 'devices')
    if (has_prefix(&localUri, &get_devices_prefix)) {
        if (l.checkLoginCode(AccountCode)) {
            if (l.isAdmin(AccountCode)) {
                int ID = getNumber(localUri, "/get_device", '.');
                string sent = "End";
                if(ID < l.getNumberOfAccounts())
                    sent = l.getAccountName(ID) + "|" + to_string(l.getAccountAuthorizationLevel(ID));

                mg_send_http_ok(nc, "Content-Type: text/html; charset=utf-8", strlen(sent.c_str()));
                mg_printf(nc, sent.c_str());
                return true;
            }
        }
    }
    return false;
}

bool AdminCommunicationHandler::handle_Post(struct mg_connection *nc, std::string IP, LoginHandler *l) {
    string change_device_prefix = "/change_device";
    string delete_device_prefix = "/delete_device";

    const char *cookie = mg_get_header(nc, "Cookie");
    char AccountCodeChar[48];
    mg_get_cookie(cookie, "code", AccountCodeChar, sizeof(AccountCodeChar));
    string AccountCode = string(AccountCodeChar);

    const struct mg_request_info *req_info = mg_get_request_info(nc);
    std::string localUri = req_info->local_uri;

    // handles modification of account authorisation level
    if(l->checkLoginCode(AccountCode)) {
        if(l->isAdmin(AccountCode)) {
            if (has_prefix(&localUri, &change_device_prefix)) {
                handle_Authorization_Change(nc, IP, l);
                mg_send_http_redirect(nc, "authorization.html", 303);
                return true;
            }
            else if (has_prefix(&localUri, &delete_device_prefix)) {
                handle_Account_Removal(nc, IP, l);
                mg_send_http_redirect(nc, "authorization.html", 303);
                return true;
            }
        }
    }

    return false;
}

void AdminCommunicationHandler::handle_Authorization_Change(struct mg_connection *nc, std::string IP, LoginHandler *l) {
    const struct mg_request_info *req_info = mg_get_request_info(nc);
    std::string localUri = req_info->local_uri;

    int ID = getNumber(localUri, "/change_device", '.');

    char post_data[10 * 1024];
    int post_data_len = mg_read(nc, post_data, sizeof(post_data));

    char param1[1024];
    mg_get_var(post_data, post_data_len, "authorization_level", param1, sizeof(param1));

    l->setAccountAuthorizationLevel(std::stoi(param1), ID);
}

void AdminCommunicationHandler::handle_Account_Removal(struct mg_connection *nc, std::string IP, LoginHandler *l) {
    const struct mg_request_info *req_info = mg_get_request_info(nc);
    std::string localUri = req_info->local_uri;

    int ID = getNumber(localUri, "/delete_device", '.');

    char post_data[10 * 1024];
    int post_data_len = mg_read(nc, post_data, sizeof(post_data));

    l->removeAccount(ID);
}
