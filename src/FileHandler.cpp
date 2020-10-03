#include "Header/FileHandler.h"

namespace fs = std::experimental::filesystem;

int FileHandler::findChar(char* array) {
    cout << strlen(array) << endl;
    for(int i = 0; i < sizeof(array); i++) {
        int c = array[i];
        switch (c) {
        case '\n':
            return i;
        case '\r':
            return i;
        }
    }
    return -1;
}

void FileHandler::initHandler(LoginHandler *l, string p) {
    login = l;
    path = p;

    updateFiles();
}

// functon for updating files in specific directory
void FileHandler::updateFiles() {
    Files = vector<string>(0);
    for (const auto & entry : fs::directory_iterator(path)) {
        Files.push_back(entry.path().filename().string());
    }
}

bool FileHandler::handleGet(CivetServer *server, struct mg_connection *nc) {
    string get_files_prefix = "/get_file";

    const char *cookie = mg_get_header(nc, "Cookie");
    char AccountCodeChar[48];
    mg_get_cookie(cookie, "code", AccountCodeChar, sizeof(AccountCodeChar));
    string AccountCode = string(AccountCodeChar);

    const struct mg_request_info *req_info = mg_get_request_info(nc);
    std::string localUri = req_info->local_uri;

	// handles the listing of available files
    if (has_prefix(&localUri, &get_files_prefix)) {
        if (login->checkLoginCode(AccountCode)) {
			// checks if user is admin
            if (login->isAdmin(AccountCode)) {
                int ID = getNumber(localUri, get_files_prefix, '.');

                if(ID == 0)
                    updateFiles();

                string sent = "End";
                if(ID >= 0 && ID < Files.size())
                    sent = Files[ID];

                mg_send_http_ok(nc, "Content-Type: text/html; charset=utf-8", strlen(sent.c_str()));
                mg_printf(nc, sent.c_str());
                return true;
            }
        }
    }
    return false;
}

bool FileHandler::handlePost(CivetServer *server, struct mg_connection *nc) {
    try {
		// handles manipulation of files
        string upload_files_prefix = "/upload";
        string download_files_prefix = "/download_file";
        string delete_files_prefix = "/delete_file";

        const char *cookie = mg_get_header(nc, "Cookie");
        char AccountCode[48];
        mg_get_cookie(cookie, "code", AccountCode, sizeof(AccountCode));

        const struct mg_request_info *req_info = mg_get_request_info(nc);
        std::string localUri = req_info->local_uri;

        if(login->isAdmin(string(AccountCode))) {
			// uploading files
            if(localUri == upload_files_prefix) {
                uploadFiles(nc);
                updateFiles();
            }
            // downloading files
            else if(has_prefix(&localUri, &download_files_prefix)) {
                int ID = getNumber(localUri, download_files_prefix, '.');
                cout << "Downloading file: " << Files[ID] << endl;
                mg_send_mime_file2(nc, (path + "/" + Files[ID]).c_str(), NULL, ("Content-Disposition: attachment; filename=\"" + Files[ID] + "\"").c_str());
            }
            // removing files
            else if(has_prefix(&localUri, &delete_files_prefix)) {
                int ID = getNumber(localUri, delete_files_prefix, '.');

                if(ID >= 0 && ID < Files.size()) {
                    remove((path + "/" + Files[ID]).c_str());
                    updateFiles();
                    mg_send_http_redirect(nc, "files.html", 303);
                }
            }
        }
    }
    catch (int e)
    {
        cout << "Execption..." << endl;
    }
    catch(...)
    {
        std::exception_ptr p = std::current_exception();
        std::clog <<(p ? p.__cxa_exception_type()->name() : "null") << std::endl;
    }
    return true;
}
FILE * f;
bool fileOpen = false;

#define fopen_recursive fopen
int field_found( const char *key, const char *filename, char *path, size_t pathlen, void *user_data ) {
    char path_buf[1024];

    cout << "Uploading File:" << endl;

	// file name is checked and new file is created to store content of uploaded file
    if(strlen(filename) > 0 && filename != "") {
        cout << "Continue:" << endl;
        cout << filename << endl;

        (void)key;
        path = const_cast<char*>( ("EventData/"+string(filename)).c_str());
        pathlen = strlen(path);

        fileOpen = true;

#ifdef _WIN32
        _snprintf(path_buf, sizeof(path_buf), "./EventData/%s", filename);
        buf[sizeof(path_buf)-1] = 0;
        if (strlen(path_buf)>255) {
            /* Windows will not work with path > 260 (MAX_PATH), unless we use
             * the unicode API.*/
            f = NULL;
        } else {
            f = fopen_recursive(path_buf, "wb");
        }
#else
        snprintf(path_buf, sizeof(path_buf), "./EventData/%s", filename);
        path_buf[sizeof(path_buf)-1] = 0;
        if (strlen(path_buf)>1020) {
            /* The string is too long and probably truncated. Make sure an
             * UTF-8 string is never truncated between the UTF-8 code bytes.*/
            std::cout << "Fail1" << std::endl;
            f = NULL;
        } else {
            f = fopen_recursive(path_buf, "w");
        }
#endif

        return MG_FORM_FIELD_STORAGE_GET;
    }
    else {
        return -23;
    }
}

int field_get( const char *key, const char *value, size_t valuelen, void *user_data ) {
	// data is written to new file until no more data is left
    if(valuelen == 0) {
        fclose(f);
        return MG_FORM_FIELD_HANDLE_ABORT;
    }
    fwrite(value, sizeof(char), valuelen, f);
    return MG_FORM_FIELD_HANDLE_GET;
}

void FileHandler::uploadFiles(struct mg_connection *nc) {
    cout << "Started upload:" << endl;

    try {
        struct mg_form_data_handler fdh = {field_found,
                   field_get,
                   0,
                   0
        };
        int code = mg_handle_form_request(nc, &fdh);
        cout << code << endl;
        mg_send_http_redirect(nc, "files.html", 303);
        cout << "End" << endl;
        if(fileOpen) {
            fclose(f);
            fileOpen = false;
        }
    }
    catch (int e)
    {
        cout << "Execption..." << endl;
    }
    catch(...)
    {
        std::exception_ptr p = std::current_exception();
        std::clog <<(p ? p.__cxa_exception_type()->name() : "null") << std::endl;
    }

}
