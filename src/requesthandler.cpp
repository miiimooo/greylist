/**   $Id: requesthandler.cpp 13 2009-08-31 22:55:52Z mimo $   **/
#include <assert.h>
#include <iostream>
#include <errno.h>
#include <string.h>
#include "requesthandler.h"
#include "utils.h"

using namespace std;
#define MAXLINE 4096
#define MAXN 16384

void TestRequestHandler::run() {
  _callback->addHandler(this);
//   _fd->close();
//   _callback->removeHandler(this);
//   return;
  //   _busy = true;
//   _requests++;
//   cout << pthread_self() << endl;
//   cout << "request to handle socket " << _fd->get()  << " handled " << _requests << endl;
  int			ntowrite;
  ssize_t		nread;
  char		line[MAXLINE], result[MAXN];
  if(_fd->getErrno()) {
    cout << "got errno " << _fd->getErrno() 
    << " error: " << strerror_r(_fd->getErrno(), line, MAXLINE) << endl;
  }
  
  for ( ; ; ) {
    if ( (nread = readline(_fd->get(), line, MAXLINE)) == 0) {
      break;
//       _fd->close();
//       _acceptor->removeBusyHandler(this);
//       return;		/* connection closed by other end */
    }
    /* 4line from client specifies #bytes to write back */
    ntowrite = atol(line);
    if ((ntowrite <= 0) || (ntowrite > MAXN)) {
      cout << "client request for " << ntowrite << " bytes" << endl;
    }
    writen(_fd->get(), result, ntowrite);
    
  }
//   assert(_fd);
  _fd->close();
  if(_fd->getErrno()) {
    cout << "got errno " << _fd->getErrno() 
      << " error: " << strerror_r(_fd->getErrno(), line, MAXLINE) << endl;
  }
  Utils::zap(_fd);
  _callback->removeHandler(this);
}

ssize_t TestRequestHandler::readline(int fd, void *vptr, size_t maxlen)
{
  ssize_t	n, rc;
  char	c, *ptr;
  
  ptr = (char*)vptr;
  for (n = 1; n < maxlen; n++) {
    if ( (rc = read(fd, &c, 1)) == 1) {
      *ptr++ = c;
      if (c == '\n')
	break;
    } else if (rc == 0) {
      if (n == 1)
	return(0);	/* EOF, no data read */
	else
	  break;		/* EOF, some data was read */
    } else
      return(-1);	/* error */
  }
  
  *ptr = 0;
  return(n);
}
/* Write "n" bytes to a descriptor. */
ssize_t TestRequestHandler::writen(int fd, const void *vptr, size_t n)
{
  size_t          nleft;
  ssize_t         nwritten;
  const char      *ptr;
  
  ptr = (const char*) vptr;
  
  nleft = n;
  while (nleft > 0) {
    if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0 && errno == EINTR)
	nwritten = 0;           /* and call write() again */
	else
	  return(-1);                     /* error */
    }
    
    nleft -= nwritten;
    ptr   += nwritten;
  }
  return(n);
}

/* end readline */

// ssize_t RequestHandler::Readline(int fd, void *ptr, size_t maxlen)
// {
//   ssize_t		n;
//   
//   if ( (n = readline(fd, ptr, maxlen)) == -1)
//     err_sys("readline error");
//   return(n);
//   
// }