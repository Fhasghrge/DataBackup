#include "myoption.hpp"
#include "AES.hpp"
#include "hfmzip.hpp"
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
    std::string info = msg->get_payload();

    bool res;
    JSONCPP_STRING errs;
    Json::Value root, lang, mail;
    Json::CharReaderBuilder readerBuilder;
    Json::Value response;
    Json::Value trees;
    Json::StreamWriterBuilder writerBuilder;
    std::ostringstream os;
    std::string json_str;

    std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());
    res = jsonReader->parse(info.c_str(), info.c_str()+info.length(), &root, &errs);
    if (!res || !errs.empty()) {
      std::cout << "parseJson err. " << errs << std::endl;
    }

    //对客户端数据解析，执行相应操作
    std::string method = root["method"].asString();
    if(method.compare("copy") == 0)
    {
      /* 硬链接inode结点表记录 */
      map<ino_t, string> inodeTable;

      int ret;
      string source = root["source"].asString();
      string target = root["target"].asString();
      if( (ret = copyAll(source.c_str(), target.c_str(), inodeTable)) < 0)
      {
        fprintf(stderr, "func copyAll test err\n");
      }

      /* 返回 */
      if( ret == SUCCESS_M)
      {
        response["errcode"] = 0;
      }
      else
      {
        response["errcode"] = 1;
      }

      std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter()); 
      jsonWriter->write(response, &os);
      json_str = os.str();
      s->send(hdl, json_str, msg->get_opcode());
    }

    if(method.compare("pack") == 0)
    {
      /* 硬链接inode结点表记录 */
      map<ino_t, string> inodeTable;

      int ret, targetFD;
      string source = root["source"].asString();
      string target = root["target"].asString();

      targetFD = open(target.c_str(), O_CREAT | O_RDWR | O_TRUNC, 00600);
      if( (ret = packAll(source.c_str(), targetFD, inodeTable)) < 0)
      {
        fprintf(stderr, "func packAll test err\n");
      }
      close(targetFD);

      /* 返回 */
      if( ret == SUCCESS_M)
      {
        response["errcode"] = 0;
      }
      else
      {
        response["errcode"] = 1;
      }

      std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter()); 
      jsonWriter->write(response, &os);
      json_str = os.str();
      s->send(hdl, json_str, msg->get_opcode());
    }

    if(method.compare("unpack") == 0)
    {
      /* 硬链接inode结点表记录 */
      map<ino_t, string> inodeTable;

      string source = root["source"].asString();
      string target = root["target"].asString();

      int fd = open(source.c_str(), O_RDWR);

      mkdir(target.c_str(),00700);
      while( isEndOfFile(fd) == 0 && unpackOne(target.c_str(),fd) == SUCCESS_M ) ;

      /* 返回 */
      if( isEndOfFile(fd) )
      {
        response["errcode"] = 0;
      }
      else
      {
        response["errcode"] = 1;
      }

      close(fd);

      std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter()); 
      jsonWriter->write(response, &os);
      json_str = os.str();
      s->send(hdl, json_str, msg->get_opcode());
    }

    if(method.compare("uncompress") == 0)
    {
      int ret;
      string source = root["source"].asString();
      string target = root["target"].asString();

      if( (ret = extract(source.c_str(), target.c_str())) < 0)
      {
        fprintf(stderr, "func extract test err\n");
      }

      /* 返回 */
      if( ret == 0)
      {
        response["errcode"] = 0;
      }
      else
      {
        response["errcode"] = 1;
      }

      std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter()); 
      jsonWriter->write(response, &os);
      json_str = os.str();
      s->send(hdl, json_str, msg->get_opcode());
    }

    if(method.compare("compress") == 0)
    {
      int ret;
      string source = root["source"].asString();
      string target = root["target"].asString();

      if( (ret = compress(source.c_str(), target.c_str())) < 0)
      {
        fprintf(stderr, "func compress test err\n");
      }

      /* 返回 */
      if( ret == 0)
      {
        response["errcode"] = 0;
      }
      else
      {
        response["errcode"] = 1;
      }

      std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter()); 
      jsonWriter->write(response, &os);
      json_str = os.str();
      s->send(hdl, json_str, msg->get_opcode());
    }

    if(method.compare("encrypt") == 0)
    {
      int ret;
      string source = root["source"].asString();
      string target = root["target"].asString();
      string key = root["key"].asString();

      if( (ret = encrypt(source.c_str(), target.c_str(), key.c_str())) < 0)
      {
        fprintf(stderr, "func encrypt test err\n");
      }

      /* 返回 */
      if( ret == 0)
      {
        response["errcode"] = 0;
      }
      else
      {
        response["errcode"] = 1;
      }

      std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter()); 
      jsonWriter->write(response, &os);
      json_str = os.str();
      s->send(hdl, json_str, msg->get_opcode());
    }

    if(method.compare("decrypt") == 0)
    {
      int ret;
      string source = root["source"].asString();
      string target = root["target"].asString();
      string key = root["key"].asString();

      if( (ret = decrypt(source.c_str(), target.c_str(), key.c_str())) < 0)
      {
        fprintf(stderr, "func decrypt test err\n");
      }

      /* 返回 */
      if( ret == 0)
      {
        response["errcode"] = 0;
      }
      else
      {
        response["errcode"] = 1;
      }

      std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter()); 
      jsonWriter->write(response, &os);
      json_str = os.str();
      s->send(hdl, json_str, msg->get_opcode());
    }

    if(method.compare("getPWD") == 0)
    {
      string json_str = getCWDToJson();

      s->send(hdl, json_str, msg->get_opcode());
    }

    if(method.compare("getList") == 0)
    {
      int ret;
      string source = root["path"].asString();
      string json_str = listWorkingDir(source.c_str());
      s->send(hdl, json_str, msg->get_opcode());
    }

  } catch (websocketpp::exception const & e) {
      std::cout << "failed because: "
                << "(" << e.what() << ")" << std::endl;
  }
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