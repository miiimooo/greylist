/// $Id: pooltest.cpp 27 2009-09-15 19:37:26Z mimo $

//: C03:StringSuite.cpp
//{L} ../TestSuite/Test ../TestSuite/Suite
//{L} TrimTest
// Illustrates a test suite for code from Chapter 3
#include <iostream>
#include "../TestSuite/Suite.h"
#include <string>
#include <cstddef>
#include <memory>

// c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <deque>

// /c
#include <gc/leak_detector.h>

#include "zthread/Thread.h"
#include "zthread/ThreadedExecutor.h"
#include "zthread/AtomicCount.h"
#include "zthread/Mutex.h"
#include "zthread/Guard.h"
#include "zthread/Condition.h"

#include "defensive.h"

using namespace std;
using namespace TestSuite;
using namespace ZThread;

#define MAXDATASIZE 100 // max number of bytes we can get at once 

AtomicCount acServerThreads;

void sigchld_handler(int s) {
  while (waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
template<class T> class TQueue {
  ZThread::Mutex lock;
  ZThread::Condition cond;
  std::deque<T> data;
  public:
    TQueue() : cond(lock) {}
    void put(T item) {
      ZThread::Guard<ZThread::Mutex> g(lock);
      data.push_back(item);
      cond.signal();
    }
    T get() {
      ZThread::Guard<ZThread::Mutex> g(lock);
      while(data.empty())
	cond.wait();
      T returnVal = data.front();
      data.pop_front();
      return returnVal;
    }
};
class LiftOffRunner : public Runnable {
  TQueue<int> _qfds;
  public:
    void add(int fd) {
      _qfds.put(fd);
    }
    void run() {
      try {
        while (!Thread::interrupted()) {
          int fd = _qfds.get();
// TODO do something with the filedescriptor	  
//           rocket->run();
        }
      } catch (Interrupted_Exception&) { /* Exit */ }
      cout << "Exiting LiftOffRunner" << endl;
    }
};

class BGThread : public ZThread::Runnable {
  public:

    void run() {
      while (true) {
        cout << "BG: " << pthread_self() << endl;
        ZThread::Thread::sleep(1000);
      }
    }
};
class DummyClient : public Runnable {
  public:
    void run() {
      int rv;
      int sockfd, numbytes;
      struct addrinfo hints, *servinfo, *p;
      memset(&hints, 0, sizeof hints);
      //       hints.ai_family = AF_UNSPEC;
      hints.ai_family = AF_INET;
      hints.ai_socktype = SOCK_STREAM;
      string port = "4096";
      if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
      }
      for (p = servinfo; p != NULL; p = p->ai_next) {
        void *addr;
        const char *ipver;
        char ipstr[INET6_ADDRSTRLEN];

        if (p->ai_family == AF_INET) { // IPv4
          struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
          addr = &(ipv4->sin_addr);
          ipver = "IPv4";
        } else { // IPv6
          struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
          addr = &(ipv6->sin6_addr);
          ipver = "IPv6";
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("dummy client  %s: %s\n", ipver, ipstr);

        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
          perror("dummy client: socket");
          return;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
          close(sockfd);
          perror("dummy client: connect");
          return;
        }
      }
    }
};
class Client  : public ZThread::Runnable {
    int _id;
    int _num;
    bool quitFlag;
  public:
    Client(int id, int num = -1) : _id(id), _num(num), quitFlag(false) { }
    void quit() {
      quitFlag = true;
    }

    void run() {
      try {
        unsigned count = 0;
        int sockfd, numbytes;
        char buf[MAXDATASIZE];
        struct addrinfo hints, *servinfo, *p;
        int rv;
        char s[INET6_ADDRSTRLEN];
        memset(&hints, 0, sizeof hints);
//       hints.ai_family = AF_UNSPEC;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        string port = "4096";


        while (quitFlag == false && (!Thread::interrupted())) {
          if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return;
          }

          // loop through all the results and connect to the first we can
          for (p = servinfo; p != NULL; p = p->ai_next) {
            void *addr;
            const char *ipver;
            char ipstr[INET6_ADDRSTRLEN];

            if (p->ai_family == AF_INET) { // IPv4
              struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
              addr = &(ipv4->sin_addr);
              ipver = "IPv4";
            } else { // IPv6
              struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
              addr = &(ipv6->sin6_addr);
              ipver = "IPv6";
            }

            inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
            printf("#%d-%d client  %s: %s\n", _id, count, ipver, ipstr);

            if ((sockfd = socket(p->ai_family, p->ai_socktype,
                                 p->ai_protocol)) == -1) {
              perror("client: socket");
              return;
              count++;
              continue;
            }

            if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
              close(sockfd);
              perror("client: connect");
              return;
              count++;
              continue;
            }

            break;
          }

          if (p == NULL) {
            fprintf(stderr, "#%d-%d client : failed to connect\n", _id, count++);
            close(sockfd);
            Thread::sleep(20);
            return;
            continue;
          }



          inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
                    s, sizeof s);
          printf("#%d-%d client: connecting to %s\n", _id, count, s);

          freeaddrinfo(servinfo); // all done with this structure

          if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
            perror("recv");
            return;
          }

          buf[numbytes] = '\0';

          printf("#%d-%d client : received '%s'\n", _id, count++, buf);

          close(sockfd);

//         Thread::sleep(500);

        }
      } catch (Interrupted_Exception&) {
        cout << "Caught Interrupted_Exception" << endl;
        // Exit the task
      }

    }
};
class LiftOff : public ZThread::Runnable {
    int countDown;
    int _id;
    int sockfd;
    bool quitFlag;
    static Mutex _acceptLock;
  public:
    LiftOff(int id, int s) : _id(id), sockfd(s), quitFlag(false) {
      ::acServerThreads++;
    }
    ~LiftOff() {
      std::cout << _id << " completed" << std::endl;
      if (::acServerThreads-- == 1) {
        close(sockfd);
      }
    }
    void quit() {
      quitFlag = true;
    }
    void run() {
      try {

        int count = 0;
        socklen_t sin_size;
        int new_fd;
        struct sockaddr_storage their_addr; // connector's address information
        int backlog = 10;
        char s[INET6_ADDRSTRLEN];
        if (listen(sockfd, backlog) == -1) {
          perror("listen");
          exit(1);
        }

//       sa.sa_handler = sigchld_handler; // reap all dead processes
//       sigemptyset(&sa.sa_mask);
//       sa.sa_flags = SA_RESTART;
//       if (sigaction(SIGCHLD, &sa, NULL) == -1) {
//         perror("sigaction");
//         exit(1);
//       }
	socklen_t clilen;
	struct sockaddr *cliaddr;

	cliaddr = Malloc(addrlen);
        printf("%d server: waiting for connections...\n", _id);

        while (!quitFlag && (!Thread::interrupted())) { // main accept() loop
          sin_size = sizeof their_addr;
          new_fd = accept(sockfd, (struct sockaddr *) & their_addr, &sin_size);
          if (quitFlag || Thread::interrupted()) {
            close(new_fd);
            close(sockfd);
            cout << "quit flag set" << endl;
            return;
          }
          if (new_fd == -1) {
            perror("accept");
            continue;
          }
          Thread::yield();
// 	close(sockfd);
          inet_ntop(their_addr.ss_family,
                    get_in_addr((struct sockaddr *)&their_addr),
                    s, sizeof s);
          printf("%d-%d server: got connection from %s\n", _id, count++, s);
//         D(pthread_self());
//         if (!fork()) { // this is the child process
//           close(sockfd); // child doesn't need the listener
          if (send(new_fd, "Hello, world!", 13, 0) == -1)
            perror("send");
// 	Thread::sleep(10000);
          close(new_fd);
          //         }
//         close(new_fd);  // parent doesn't need this
        }
      } catch (Interrupted_Exception&) {
        cout << "Caught Interrupted_Exception" << endl;
        // Exit the task
      }

    }
};
class ThreadKiller : public Runnable {
    vector<class Client*> _vc;
    vector<class LiftOff*> _vl;
  public:
    ThreadKiller(vector<class Client*> vc, vector<class LiftOff*> vl)
        : _vc(vc), _vl(vl) { }
    void run() {
      for (vector<Client*>::iterator itr = _vc.begin(); itr != _vc.end(); ++itr) {
        (*itr)->quit();
      }
      for (vector<LiftOff*>::iterator itr = _vl.begin(); itr != _vl.end(); ++itr) {
        (*itr)->quit();
// 	Thread t((*itr));
// 	t.interrupt();
      }

    }
};

class PoolTests : public TestSuite::Test {
  public:
    void run() {
      int backlog = 1;
      int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
      struct addrinfo hints, *servinfo, *p;
      struct sigaction sa;
      int yes = 1;
      int rv;

      memset(&hints, 0, sizeof hints);
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV; // use my IP

      string port = "4096";

      if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
      }

      // loop through all the results and bind to the first we can
      for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
          perror("server: socket");
          continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
          perror("setsockopt");
          exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
          close(sockfd);
          perror("server: bind");
          continue;
        }

        break;
      }

      if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return ;
      }
      void *addr;
      const char *ipver;
      char ipstr[INET6_ADDRSTRLEN];

      if (p->ai_family == AF_INET) { // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        addr = &(ipv4->sin_addr);
        ipver = "IPv4";
      } else { // IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
        addr = &(ipv6->sin6_addr);
        ipver = "IPv6";
      }

      inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
      printf("server  %s: %s\n", ipver, ipstr);

      freeaddrinfo(servinfo); // all done with this structure
      D(pthread_self());
      try {
// 	vector<Thread *> vThreads;
// 	for (int i=0; i<10 ; i++) {
// 	  vThreads.push_back(new Thread(new BGThread));
// 	  vThreads.push_back(new Thread(new LiftOff(sockfd)));
// 	}
        cout << "Press <Enter> to quit:" << endl;
        ThreadedExecutor executor;
        LiftOff *pSocketThread = new LiftOff(1, sockfd);
        executor.execute(pSocketThread);
        for (int i = 0; i < 0 ; i++) {
          LiftOff *p = new LiftOff(i + 1, sockfd);
          executor.execute(p);
        }
//         Thread t1(new LiftOff(sockfd));
// 	Thread t2(new LiftOff(sockfd));
        for (int i = 0; i < 100 ; i++) {
          Client *p = new Client(i + 1);
          executor.execute(p);
          int *ip = new int; // check leaks
        }
        cin.get();
        cout << "\n\n***************************\nQuit\n***************************" << endl;
        pSocketThread->quit();
        executor.interrupt();
        // one solution to get rid of the blocked thread waiting on accept
        executor.execute(new DummyClient);

//         Thread(new ThreadKiller(vClients, vLiftOffs));
// 	while(acServerThreads != 1) {
// 	  Thread::sleep(50);
// 	}
//         close(sockfd);
// 	for(vector<Client*>::iterator itr = vClients.begin(); itr < _vc.end(); ++itr) {
// 	  (*itr)->quit();
// 	}
        //         Thread c1(new Client);
        cout << "Waiting for LiftOff" << endl;
      } catch (Synchronization_Exception& e) {
        cerr << e.what() << endl;
      }

    }
//     std::cout<< "Returning to main" << endl;
};
#define STDIN 0  // file descriptor for standard input
class SelectTest : public Test {
  public:
    void run() {

      struct timeval tv;
      fd_set readfds;

      tv.tv_sec = 2;
      tv.tv_usec = 500000;

      FD_ZERO(&readfds);
      FD_SET(STDIN, &readfds);

      // don't care about writefds and exceptfds:
      select(STDIN + 1, &readfds, NULL, NULL, &tv);

      if (FD_ISSET(STDIN, &readfds))
        printf("A key was pressed!\n");
      else
        printf("Timed out.\n");

    }
};
class Chatserver : public Test {
  public:
    void run() {
      string port = "4096";
      fd_set master;    // master file descriptor list
      fd_set read_fds;  // temp file descriptor list for select()
      int fdmax;        // maximum file descriptor number

      int listener;     // listening socket descriptor
      int newfd;        // newly accept()ed socket descriptor
      struct sockaddr_storage remoteaddr; // client address
      socklen_t addrlen;

      char buf[256];    // buffer for client data
      int nbytes;

      char remoteIP[INET6_ADDRSTRLEN];

      int yes = 1;      // for setsockopt() SO_REUSEADDR, below
      int i, j, rv;

      struct addrinfo hints, *ai, *p;

      FD_ZERO(&master);    // clear the master and temp sets
      FD_ZERO(&read_fds);

      // get us a socket and bind it
      memset(&hints, 0, sizeof hints);
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = AI_PASSIVE;
      if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
      }

      for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
          continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
          close(listener);
          continue;
        }

        break;
      }

      // if we got here, it means we didn't get bound
      if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
      }

      freeaddrinfo(ai); // all done with this

      // listen
      if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
      }

      // add the listener to the master set
      FD_SET(listener, &master);

      // keep track of the biggest file descriptor
      fdmax = listener; // so far, it's this one

      // main loop
      for (;;) {
        read_fds = master; // copy it
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
          perror("select");
          exit(4);
        }

        // run through the existing connections looking for data to read
        for (i = 0; i <= fdmax; i++) {
          if (FD_ISSET(i, &read_fds)) { // we got one!!
            if (i == listener) {
              // handle new connections
              addrlen = sizeof remoteaddr;
              newfd = accept(listener,
                             (struct sockaddr *) & remoteaddr,
                             &addrlen);

              if (newfd == -1) {
                perror("accept");
              } else {
                FD_SET(newfd, &master); // add to master set
                if (newfd > fdmax) {    // keep track of the max
                  fdmax = newfd;
                }
                printf("selectserver: new connection from %s on "
                       "socket %d\n",
                       inet_ntop(remoteaddr.ss_family,
                                 get_in_addr((struct sockaddr*)&remoteaddr),
                                 remoteIP, INET6_ADDRSTRLEN),
                       newfd);
              }
            } else {
              // handle data from a client
              if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                // got error or connection closed by client
                if (nbytes == 0) {
                  // connection closed
                  printf("selectserver: socket %d hung up\n", i);
                } else {
                  perror("recv");
                }
                close(i); // bye!
                FD_CLR(i, &master); // remove from master set
              } else {
                // we got some data from a client
                for (j = 0; j <= fdmax; j++) {
                  // send to everyone!
                  if (FD_ISSET(j, &master)) {
                    // except the listener and ourselves
                    if (j != listener && j != i) {
                      if (send(j, buf, nbytes, 0) == -1) {
                        perror("send");
                      }
                    }
                  }
                }
              }
            } // END handle data from client
          } // END got new incoming connection
        } // END looping through file descriptors
      } // END for(;;)--and you thought it would never end!
    }
};
int main() {
  Suite suite("Pool Tests");
  suite.addTest(new PoolTests);
//   suite.addTest(new SelectTest);
//   suite.addTest(new Chatserver);
  suite.run();
  long nFail = suite.report();
  suite.free();
  CHECK_LEAKS();
  return nFail;
}
