#define DOCUMENT_ROOT "./HTML_Files"

#include <openssl/dh.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>

#include "Header/AccountCommunicationHandler.h"

#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

//static const char *s_ssl_cert = "cert.pem";
//static const char *s_ssl_key = "privkey.pem";

std::string PORT = "80r,443s";

std::string cstrpath = "HTML_Files/";
volatile bool exitNow = false;
char str[1024];

AccountCommunicationHandler account(cstrpath);
AdminCommunicationHandler admin(cstrpath);
ConsoleHandler console("EventData/EventsSavefile.csv", "EventData/addresses.txt", "EventData/Form.txt", cstrpath);
CodeGenerator generator(1744, 32, cstrpath);
FormHandler h_form;

std::string filename;

static int is_equal(string *s1, string *s2) {
    return s1->size() == s2->size() && memcmp(s1, s2, s2->size()) == 0;
}

/* taken from https://github.com/civetweb/civetweb/blob/master/examples/embedded_c/embedded_c.c */
DH *
get_dh2236()
{
    static unsigned char dh2236_p[] = {
        0x0E, 0x97, 0x6E, 0x6A, 0x88, 0x84, 0xD2, 0xD7, 0x55, 0x6A, 0x17, 0xB7,
        0x81, 0x9A, 0x98, 0xBC, 0x7E, 0xD1, 0x6A, 0x44, 0xB1, 0x18, 0xE6, 0x25,
        0x3A, 0x62, 0x35, 0xF0, 0x41, 0x91, 0xE2, 0x16, 0x43, 0x9D, 0x8F, 0x7D,
        0x5D, 0xDA, 0x85, 0x47, 0x25, 0xC4, 0xBA, 0x68, 0x0A, 0x87, 0xDC, 0x2C,
        0x33, 0xF9, 0x75, 0x65, 0x17, 0xCB, 0x8B, 0x80, 0xFE, 0xE0, 0xA8, 0xAF,
        0xC7, 0x9E, 0x82, 0xBE, 0x6F, 0x1F, 0x00, 0x04, 0xBD, 0x69, 0x50, 0x8D,
        0x9C, 0x3C, 0x41, 0x69, 0x21, 0x4E, 0x86, 0xC8, 0x2B, 0xCC, 0x07, 0x4D,
        0xCF, 0xE4, 0xA2, 0x90, 0x8F, 0x66, 0xA9, 0xEF, 0xF7, 0xFC, 0x6F, 0x5F,
        0x06, 0x22, 0x00, 0xCB, 0xCB, 0xC3, 0x98, 0x3F, 0x06, 0xB9, 0xEC, 0x48,
        0x3B, 0x70, 0x6E, 0x94, 0xE9, 0x16, 0xE1, 0xB7, 0x63, 0x2E, 0xAB, 0xB2,
        0xF3, 0x84, 0xB5, 0x3D, 0xD7, 0x74, 0xF1, 0x6A, 0xD1, 0xEF, 0xE8, 0x04,
        0x18, 0x76, 0xD2, 0xD6, 0xB0, 0xB7, 0x71, 0xB6, 0x12, 0x8F, 0xD1, 0x33,
        0xAB, 0x49, 0xAB, 0x09, 0x97, 0x35, 0x9D, 0x4B, 0xBB, 0x54, 0x22, 0x6E,
        0x1A, 0x33, 0x18, 0x02, 0x8A, 0xF4, 0x7C, 0x0A, 0xCE, 0x89, 0x75, 0x2D,
        0x10, 0x68, 0x25, 0xA9, 0x6E, 0xCD, 0x97, 0x49, 0xED, 0xAE, 0xE6, 0xA7,
        0xB0, 0x07, 0x26, 0x25, 0x60, 0x15, 0x2B, 0x65, 0x88, 0x17, 0xF2, 0x5D,
        0x2C, 0xF6, 0x2A, 0x7A, 0x8C, 0xAD, 0xB6, 0x0A, 0xA2, 0x57, 0xB0, 0xC1,
        0x0E, 0x5C, 0xA8, 0xA1, 0x96, 0x58, 0x9A, 0x2B, 0xD4, 0xC0, 0x8A, 0xCF,
        0x91, 0x25, 0x94, 0xB4, 0x14, 0xA7, 0xE4, 0xE2, 0x1B, 0x64, 0x5F, 0xD2,
        0xCA, 0x70, 0x46, 0xD0, 0x2C, 0x95, 0x6B, 0x9A, 0xFB, 0x83, 0xF9, 0x76,
        0xE6, 0xD4, 0xA4, 0xA1, 0x2B, 0x2F, 0xF5, 0x1D, 0xE4, 0x06, 0xAF, 0x7D,
        0x22, 0xF3, 0x04, 0x30, 0x2E, 0x4C, 0x64, 0x12, 0x5B, 0xB0, 0x55, 0x3E,
        0xC0, 0x5E, 0x56, 0xCB, 0x99, 0xBC, 0xA8, 0xD9, 0x23, 0xF5, 0x57, 0x40,
        0xF0, 0x52, 0x85, 0x9B,
    };
    static unsigned char dh2236_g[] = {
        0x02,
    };
    DH *dh;

    if ((dh = DH_new()) == NULL)
        return (NULL);
    //DH_set0_pqg(dh, BN_bin2bn(dh2236_p, sizeof(dh2236_p), NULL), NULL, BN_bin2bn(dh2236_g, sizeof(dh2236_g), NULL));
    dh->p = BN_bin2bn(dh2236_p, sizeof(dh2236_p), NULL);
    dh->g = BN_bin2bn(dh2236_g, sizeof(dh2236_g), NULL);
    //const BIGNUM *p;
    //const BIGNUM *g;
    //DH_get0_pqg(dh, &p, NULL, &g);
    if ((dh->p == NULL) || (dh->g== NULL)) {
        DH_free(dh);
        return (NULL);
    }
    return (dh);
}

/* taken from https://github.com/civetweb/civetweb/blob/master/examples/embedded_c/embedded_c.c */
int
init_ssl(void *ssl_ctx, void *user_data)
{
    /* Add application specific SSL initialization */
    struct ssl_ctx_st *ctx = (struct ssl_ctx_st *)ssl_ctx;

    /* example from https://github.com/civetweb/civetweb/issues/347 */
    DH *dh = get_dh2236();
    if (!dh)
        return -1;
    if (1 != SSL_CTX_set_tmp_dh(ctx, dh))
        return -1;
    DH_free(dh);

    EC_KEY *ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!ecdh)
        return -1;
    if (1 != SSL_CTX_set_tmp_ecdh(ctx, ecdh))
        return -1;
    EC_KEY_free(ecdh);

    printf("ECDH ciphers initialized\n");
    return 0;
}

// Handler for account related messages
class AccountHandler : public CivetHandler
{
    public:
        bool
        handleGet(CivetServer *server, struct mg_connection *nc)
        {
            try {
                const struct mg_request_info *req_info = mg_get_request_info(nc);
                char IP[48];
                strncpy(IP,req_info->remote_addr, 48);
                std::cout << "GET: " << std::endl;

                if (account.handle_Get(nc, IP, &h_form.auth)) {                                       // For account
                    std::cout << "GET Account Data" << std::endl;
                }
                else if (admin.handle_Get(nc, IP, account.l)) {
                    std::cout << "GET Admin Data" << std::endl;
                }
                else {
                    return false;
                }
            }
            catch (std::exception& ex) {
                std::cerr << ex.what() << std::endl;
            }
            catch (...) {
                std::cerr << "Caught unknown exception." << std::endl;
            }

            return true;
        }

        bool
        handlePost(CivetServer *server, struct mg_connection *nc)
        {
            try {
                const struct mg_request_info *req_info = mg_get_request_info(nc);
                char IP[48];
                strncpy(IP,req_info->remote_addr, 48);
                std::cout << "POST: " << std::endl;

                if (account.handle_Post(nc, IP))                                  // Account
                    std::cout << "POST Account Data" << std::endl;
                else if (admin.handle_Post(nc, IP, &account.l))
                    std::cout << "POST Admin Data" << std::endl;
                else
                    return false;
            }
            catch (std::exception& ex) {
                std::cerr << ex.what() << std::endl;
            }
            catch (...) {
                std::cerr << "Caught unknown exception." << std::endl;
            }


            return true;
        }
};

// Handler for console related messages
class ConsoleCommunicationHandler : public CivetHandler
{
    public:
        bool
        handlePost(CivetServer *server, struct mg_connection *nc)
        {
            try {
                std::cout << "POST: " << std::endl;
                if (console.handle_Post(nc, &account.l, h_form.form))
                    std::cout << "POST Console Data" << std::endl;
                else
                    return false;
            }
            catch (std::exception& ex) {
                std::cerr << ex.what() << std::endl;
            }
            catch (...) {
                std::cerr << "Caught unknown exception." << std::endl;
            }
            return true;
        }
};

// Handler for restart of whole system (just possible with admin permission)
class RestartHandler : public CivetHandler
{
    public:
        bool
        handlePost(CivetServer *server, struct mg_connection *nc)
        {
            const char *cookie = mg_get_header(nc, "Cookie");
            char AccountCodeChar[48];
            mg_get_cookie(cookie, "code", AccountCodeChar, sizeof(AccountCodeChar));
            string AccountCode = string(AccountCodeChar);
            if (account.l.checkLoginCode(AccountCode)) {
                h_form.initHandler("EventData/EventsSavefile.csv", "EventData/addresses.txt", "EventData/Form.txt", "EventData/FormExplanation.txt", cstrpath, &generator);
                mg_send_http_redirect(nc, "admin_page.html", 303);
            }
            else {
                mg_send_http_redirect(nc, "index.html", 303);
            }
            return true;
        }
};

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int main(int argc, char** argv) {
    bool ssl = true;
    std::cout << "Init" << endl;

    if(argc > 1 && is_number(argv[1])) {
        cout << argv[1] << endl;
        generator = CodeGenerator(std::stoi(argv[1]), 32, cstrpath);
    }
    else {
        generator = CodeGenerator(1744, 32, cstrpath);
    }

    if(argc > 2 && std::string(argv[2]) == "1") {
        ssl = false;
        cout << "Listening to HTTP only" << endl;
        PORT = "80,443r";
    }

    std::cout << "Starting Server..." << std::endl;
    std::cout << "Code: " << generator.getCode() << std::endl;
    std::cout << "https://projektwoche.ddns.net/login_form_user?code=" << generator.getCode() << std::endl;

    const char *options[] = {
        "document_root", DOCUMENT_ROOT, "listening_ports", PORT.c_str(),
        "error_log_file",
        "error.log",
        "ssl_certificate",
        "certificate.pem",
        "ssl_protocol_version",
        "3",
        "ssl_cipher_list",
        "ALL",
        0
    };

    std::vector<std::string> cpp_options;
    for (int i=0; i<(sizeof(options)/sizeof(options[0])-1); i++) {
        cpp_options.push_back(options[i]);
    }

    struct CivetCallbacks callbacks;
    callbacks.init_ssl = init_ssl;
    CivetServer server(cpp_options, &callbacks);


    // Initialising handlers

    h_form.initHandler("EventData/EventsSavefile.csv", "EventData/addresses.txt", "EventData/Form.txt", "EventData/FormExplanation.txt", cstrpath, &generator);
    server.addHandler("/email$", h_form);
    server.addHandler("/form.html$", h_form);
    server.addHandler("/wrong_email_main_page", h_form);
    server.addHandler("/form$", h_form);
    server.addHandler("/confirm$", h_form);
    server.addHandler("/download$", h_form);
    server.addHandler("/wrong_email_main_page", h_form);
    server.addHandler("/login_form_user", h_form);
    server.addHandler("/endform", h_form);
    server.addHandler("/2lang", h_form);
    server.addHandler("/name", h_form);

    AccountHandler h_ac;
    server.addHandler("/login$", h_ac);
    server.addHandler("/registration_page$", h_ac);
    server.addHandler("/logout_page$", h_ac);
    server.addHandler("/new_password$", h_ac);
    server.addHandler("/index", h_ac);
    server.addHandler("/user_login_failed_password", h_ac);
    server.addHandler("/admin_page.html$", h_ac);
    server.addHandler("/account_page.html$", h_ac);
    server.addHandler("/main_page", h_ac);
    server.addHandler("/register.html$", h_ac);
    server.addHandler("/new_password.html$", h_ac);
    server.addHandler("/get_device", h_ac);
    server.addHandler("/change_device", h_ac);
    server.addHandler("/delete_device", h_ac);
    server.addHandler("/lang", h_ac);
    server.addHandler("/$", h_ac);

    FileHandler h_up;
    h_up.initHandler(&account.l, "EventData");
    server.addHandler("/upload", h_up);
    server.addHandler("/get_file", h_up);
    server.addHandler("/download_file", h_up);
    server.addHandler("/delete_file", h_up);

    ConsoleCommunicationHandler h_co;
    server.addHandler("/output", h_co);
    server.addHandler("/request", h_co);

    RestartHandler h_res;
    server.addHandler("/restart$", h_res);

    while (!exitNow) {
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    return 0;
}

