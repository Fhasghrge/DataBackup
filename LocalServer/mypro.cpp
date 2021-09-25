#include "myoption.hpp"
using namespace std;

static char* command(int argc, char *argv[]);
static char* trim(char* s);

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") {
        s->stop_listening();
        return;
    }

    try {
        int argc1;
        char *token;
        char *argv_3[3]; 
        char *input = (char *)malloc(64);
        char whitespace[2] = " ";
        for(int i = 0; i<3 ; i++)
        {
          argv_3[i] = (char *)malloc(32);
        }

        const char* temp = msg->get_payload().c_str();
        for(int i = 0; i<strlen(temp) ; i++ )
        {
          input[i] = temp[i];
        }

        input[strlen(temp)] = '\0';
        trim(input);
        argc1 = 0;
        token = strtok(input, whitespace);
        while(token != NULL && argc1 < 3)
        {
          strcpy(argv_3[argc1++], token);
          token = strtok(NULL, whitespace);
        }
    
        char* result_str;
        result_str = command(argc1, argv_3);

        string res(result_str);
        s->send(hdl, res, msg->get_opcode());

        for(int i = 0; i<3 ; i++)
        {
          free(argv_3[i]);
        }

    } catch (websocketpp::exception const & e) {
        std::cout << "Echo failed because: "
                  << "(" << e.what() << ")" << std::endl;
    }
}

void on_http(server* s, websocketpp::connection_hdl hdl) {
  /***
	try
	{
		server::connection_ptr con = s->get_con_from_hdl(hdl);
		websocketpp::http::parser::request rt = con->get_request();
		const string& strUri = rt.get_uri();
		const string& strMethod = rt.get_method();
		const string& strBody = rt.get_body();	//只针对post时有数据
		const string& strHost = rt.get_header("host");
		const string& strVersion = rt.get_version();
		std::cout<<"接收到一个"<<strMethod.c_str()<<"请求："<<strUri.c_str()<<"  线程ID="<<::GetCurrentThreadId()<<"  host = "<<strHost<<std::endl;
		if (strMethod.compare("POST") == 0)
		{//对于post提交的，直接返回跳转
			//websocketpp::http::parser::request r;
			con->set_status(websocketpp::http::status_code::value(websocketpp::http::status_code::found));
			con->append_header("location", "http://blog.csdn.net/mfcing");
		}
		else if (strMethod.compare("GET") == 0)
		{
			if (strUri.compare("/") == 0)
			{//请求主页
				con->set_body("Hello WebsocketPP!");
				con->set_status(websocketpp::http::status_code::value(websocketpp::http::status_code::ok));
			}
			else if (strUri.compare("/favicon.ico") == 0)
			{//请求网站图标，读取图标文件，直接返回二进制数据
				static const string strIcoPath = g_strRunPath + "server\\Update.ico";
				string strBuffer;
				if (ReadFileContent(strIcoPath.c_str(), "rb", strBuffer))
				{
					con->set_body(strBuffer);
					con->append_header("Content-Type", "image/x-icon");
				}
				con->set_status(websocketpp::http::status_code::value(websocketpp::http::status_code::ok));//HTTP返回码 OK
			}
			else if (strUri.compare("/test.html") == 0)
			{//请求一个页面，读取这个页面文件内容然后返回给浏览器
				static const string strHtmlPath = g_strRunPath + "server\\test.htm";
				string strBuffer;
				int code = websocketpp::http::status_code::ok;
				if (ReadFileContent(strHtmlPath.c_str(), "r", strBuffer))
				{
					con->set_body(strBuffer);
					con->append_header("Content-Type", "text/html");
				}
				else
				{//页面不存在，返回404
					code = websocketpp::http::status_code::not_found;
					con->set_body("<html><body><div style=\"color:#F000FF;font:14px;font-family: 'Microsoft YaHei';\">温馨提示：</div><div style=\"padding-left: 80px;color:#333333;font:14px;font-family: 'Microsoft YaHei';\">页面被外星人带走啦！</div></body></html>");
				}
				con->set_status(websocketpp::http::status_code::value(code));//HTTP返回码
			}
			else if (strUri.compare("/server/test.jpg") == 0)
			{//上面的页面的HTML中配置了一张图片，因此浏览器回来服务器请求这张图
				static const string strImgPath = g_strRunPath + "server\\test.jpg";
				string strBuffer;
				int code = websocketpp::http::status_code::ok;
				if (ReadFileContent(strImgPath.c_str(), "rb", strBuffer))
				{
					con->set_body(strBuffer);
					con->append_header("Content-Type", "image/jpeg");
				}
				else
				{//页面不存在，返回404
					code = websocketpp::http::status_code::not_found;
				}
				con->set_status(websocketpp::http::status_code::value(code));//HTTP返回码
			}
			else
			{//其他未定义的页面，返回跳转
				con->set_status(websocketpp::http::status_code::value(websocketpp::http::status_code::found));
				con->append_header("location", "http://blog.csdn.net/mfcing");
			}
		}
	}
	catch (websocketpp::exception const & e) 
	{
		std::cout << "exception: " << e.what() << std::endl;
	}
	catch(std::exception &e)
	{
		std::cout << "exception: " << e.what() << std::endl;
	}
	catch(...)
	{

	}
  ***/
}

void on_open(server *server, websocketpp::connection_hdl hdl) {
  cout << "have client connected" << endl;
}

void on_close(server *server, websocketpp::connection_hdl hdl) {
  cout << "have client disconnected" << endl;
}


int main() {
    // Create a server endpoint
    server test_server;

    try {
        // Set logging settings
        test_server.set_access_channels(websocketpp::log::alevel::all);
        test_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        test_server.init_asio();

        // Register our message handler
        test_server.set_message_handler(bind(&on_message,&test_server,::_1,::_2));

        // Register our http handler
        test_server.set_http_handler(bind(&on_http,&test_server,::_1));

        test_server.set_open_handler(bind(&on_open,&test_server,::_1));
        test_server.set_close_handler(bind(&on_close,&test_server,::_1));

        // Listen on port 9003
        test_server.listen(9003);

        // Start the server accept loop
        test_server.start_accept();

        // Start the ASIO io_service run loop
        test_server.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}

static char* trim(char *s)
{
 char *p = s + strlen(s) - 1;
 while(' ' == *p || '\t' == *p || '\n'==*p || 'r'==*p)
 {
   *p = '\0';
   p--;
 } 
 return s;
}

static char* command(int argc, char *argv[])
{
  char *res = (char*) malloc(32);
  snprintf(res,32,"%d %s %s %s",argc, argv[0], argv[1], argc==3 ? argv[2] : "null");

  if(argc <= 0 || argc > 3)
  {
    snprintf(res, 32, "error");
    return res;
  }

  if(!strcmp(argv[0], "exit"))
  {
    snprintf(res, 32, "exit");
    return res;
  }

  if(argc == 1)
  {
    /*
    if(tree == NULL && strcmp(argv[0], "init")!=0)
    {
      printf("please init!\n");
      return 0;
    }
    */

  }




  return res;
}