#include <iostream>
#include "Header/src/CivetServer.h"
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define DOCUMENT_ROOT "./HTML_Files"
#define PORT "80"
#define EXAMPLE_URI "/example"
#define EXIT_URI "/exit"

volatile bool exitNow = false;

class HTMLHandler : public CivetHandler
{
	public:
	bool
	handleGet(CivetServer *server, struct mg_connection *nc)
	{
		const struct mg_request_info *req_info = mg_get_request_info(nc);
		std::string localUri = req_info->local_uri;
		std::cout << localUri << std::endl;
		//if(localUri == "hui")
		localUri.erase (0,1);
		std::cout << localUri << std::endl;
		
		//mg_send_http_redirect(nc, .c_str(), 0);
		mg_send_file(nc, ("HTML_Files/"+localUri).c_str());
		return true;
	}
	
};

class ExampleHandler : public CivetHandler
{
  public:
	bool
	handleGet(CivetServer *server, struct mg_connection *conn)
	{
		const struct mg_request_info *req_info = mg_get_request_info(conn);
		std::string localUri = req_info->local_uri;
		mg_send_http_redirect(conn, "login_admin.html", 0);
		/*mg_printf(conn,
		          "HTTP/1.1 200 OK\r\nContent-Type: "
		          "text/html\r\nConnection: close\r\n\r\n");
		mg_printf(conn, "<html><body>\r\n");
		mg_printf(conn,
		          "<h2>This is an example text from a C++ handler</h2>\r\n");
		mg_printf(conn,
		          "<p>To see a page from the A handler <a "
		          "href=\"a\">click here</a></p>\r\n");
		mg_printf(conn,
                  "<form action=\"a\" method=\"get\">"
                  "To see a page from the A handler with a parameter "
                  "<input type=\"submit\" value=\"click here\" "
                  "name=\"param\" \\> (GET)</form>\r\n");
        mg_printf(conn,
                  "<form action=\"a\" method=\"post\">"
                  "To see a page from the A handler with a parameter "
                  "<input type=\"submit\" value=\"click here\" "
                  "name=\"param\" \\> (POST)</form>\r\n");
		mg_printf(conn,
		          "<p>To see a page from the A/B handler <a "
		          "href=\"a/b\">click here</a></p>\r\n");
		mg_printf(conn,
		          "<p>To see a page from the *.foo handler <a "
		          "href=\"xy.foo\">click here</a></p>\r\n");
		mg_printf(conn,
		          "<p>To see a page from the WebSocket handler <a "
		          "href=\"ws\">click here</a></p>\r\n");
		mg_printf(conn,
		          "<p>To exit <a href=\"%s\">click here</a></p>\r\n",
		          EXIT_URI);
		mg_printf(conn, "</body></html>\r\n");*/
		return true;
	}
	
	bool
	handlePost(CivetServer *server, struct mg_connection *conn)
	{
		std::cout << "Hoi2" << std::endl;
		const struct mg_request_info *req_info = mg_get_request_info(conn);
		std::string localUri = req_info->local_uri;
		mg_send_http_redirect(conn, "login_admin.html", 303);
		return true;
	}
};

/* Server context handle */
struct mg_context *ctx;

int main() {
	std::cout << "Starting Server..." << std::endl;

    const char *options[] = {
	    "document_root", DOCUMENT_ROOT, "listening_ports", PORT, 0};
    
    std::vector<std::string> cpp_options;
    for (int i=0; i<(sizeof(options)/sizeof(options[0])-1); i++) {
        cpp_options.push_back(options[i]);
    }
    
    CivetServer server(cpp_options);
    
    
    ExampleHandler h_ex;
	server.addHandler("/login$", h_ex);
	
	HTMLHandler h_html;
    server.addHandler("/login$", h_html);
    server.addHandler("/registration_page$", h_html);
    server.addHandler("/logout_page$", h_html);
    server.addHandler("/new_password$", h_html);
    server.addHandler("/main_page.html$", h_html);
    server.addHandler("/new_password.html$", h_html);
    server.addHandler("/get_device", h_html);
    server.addHandler("/change_device", h_html);
	
	while (!exitNow) {
#ifdef _WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}
	
	return 0;
}
