#include <iostream>
#include <sstream>
#include <SocketHandler.h>
#include <ListenSocket.h>
#include <Utility.h>

#include <TcpSocket.h>
#include <ISocketHandler.h>

#define THDBG(x)

class DisplaySocket : public TcpSocket
{
public:
  DisplaySocket(ISocketHandler& );

  void OnAccept();
  void OnDetached();
  void OnDisconnect();
  void OnRawData(const char *buf,size_t len);
//   void OnLine(const std::string& strLine);
private:
//   std::string _strBuf;
};

// the constant TCP_BUFSIZE_READ is the maximum size of the standard input
// buffer of TcpSocket
#define RSIZE TCP_BUFSIZE_READ

DisplaySocket::DisplaySocket(ISocketHandler& h) : TcpSocket(h)
{
}
void DisplaySocket::OnAccept()
{
  if (!Detach()) {
    fprintf(stderr, "\nDetach failed\n");
  }
	
}
void DisplaySocket::OnDisconnect() {
  std::stringstream stmTmp;
  stmTmp << "Thread ID: " << pthread_self() << " " << __FUNCTION__ << " Number of sockets in list : " << Handler().GetCount() << std::endl;
//   stmTmp << __FUNCTION__ << ": Thread ID: " << pthread_self() << std::endl;
//     << "Number of sockets in list : " << Handler().GetCount() << std::endl 
//     << "Uptime : " << Uptime() << std::endl; 
  std::cout <<  stmTmp.str();

}
void DisplaySocket::OnDetached() {
  std::stringstream stmTmp;
  stmTmp << "Thread ID: " << pthread_self() << " " << __FUNCTION__ << " Number of sockets in list : " << Handler().GetCount() << std::endl;
//   stmTmp << __FUNCTION__ << ":Thread ID: " << pthread_self() << std::endl
//     << "Number of sockets in list : " << Handler().GetCount() << std::endl 
//     << "& : " << &stmTmp << std::endl; 
  std::cout << stmTmp.str();
// 	Send("Local hostname : " + Utility::GetLocalHostname() + "\n");
// 	Send("Local address : " + Utility::GetLocalAddress() + "\n");
// 	Send("Number of sockets in list : " + Utility::l2string(Handler().GetCount()) + "\n");
// 	Send("Thread ID: " + pthread_self() + "\n");
  THDBG(Send(stmTmp.str()));
}
// void DisplaySocket::OnRawData(const char *buf,size_t len) {
//   std::stringstream stmTmp;
//   stmTmp << "Thread ID: " << pthread_self() << std::endl;
//   if (len ==2) { //CR?
//     fprintf(stdout, "%x %x\n", buf[0], buf[1]);
//   }
//   if ((len == 2) && (buf[0] == 0xd) && (buf[1] == 0xa)) { //CR
//     std::cout << "(End-of-Line)" << std::endl;
//   }
//   std::cout << "Line (len=" << len << "): " << std::string(buf, len) <<  stmTmp.str();
//   _strBuf += std::string(buf, len);
//   std::cout << "_strBuf: " << _strBuf << std::endl;
// }
// postfix sends whole buffer at once 0x0a at end of line - 0x0a 0x0a empty line
void DisplaySocket::OnRawData(const char *buf,size_t len) {
  std::stringstream stmTmp;
  stmTmp << "Thread ID: " << pthread_self() << " " << __FUNCTION__ << " recvd " << len << std::endl;
  std::cout << "Line (len=" << len << "): " << std::string(buf, len) <<  stmTmp.str();
  std::cout << stmTmp.str();
  Send("action=dunno\n\n");
}
// void DisplaySocket::OnRawData(const char *buf,size_t len) {
// //   std::stringstream stmTmp;
// //   stmTmp << "Thread ID: " << pthread_self() << std::endl;
//   std::cout << "Line (len=" << len << "): " << std::string(buf, len);
//   for (int i=0;i < len; i++) {
//     fprintf(stdout, "%2x ", buf[i]);
//   }
//   std::cout << std::endl;
//   if (len == 2) { //CR?
//     fprintf(stdout, "%x %x\n", buf[0], buf[1]);
//   }
//   if ((len == 2) && (buf[0] == 0xd) && (buf[1] == 0xa)) { //CR
//     std::cout << "_strBuf: " << _strBuf << std::endl;
//     std::cout << "(End-of-Line)" << std::endl;
//     Send("action=dunno\n\n");
//     return;
//   }
// //   std::cout << "Line (len=" << len << "): " << std::string(buf, len) <<  stmTmp.str();
//   _strBuf += std::string(buf, len);
// //   std::cout << "_strBuf: " << _strBuf << std::endl;
// }



// void DisplaySocket::OnRead()
// {
// 	// OnRead of TcpSocket actually reads the data from the socket
// 	// and moves it to the input buffer (ibuf)
// 	TcpSocket::OnRead();
// 	// get number of bytes in input buffer
// 	size_t n = ibuf.GetLength();
// 	if (n > 0)
// 	{
// 		char tmp[RSIZE]; // <--- tmp's here
// 		ibuf.Read(tmp,n);
// 		printf("Read %d bytes:\n",n);
// 		for (size_t i = 0; i < n; i++)
// 		{
// 			printf("%c",isprint(tmp[i]) ? tmp[i] : '.');
// 		}
// 		printf("\n");
// 	}
// }
static	bool quit = false;

int main()
{
	SocketHandler h;
	ListenSocket<DisplaySocket> l(h);

	if (l.Bind(2222))
	{
		exit(-1);
	}
	h.Add(&l);
	h.Select(1,0);
	while (!quit)
	{
		h.Select(1,0);
	}
}

