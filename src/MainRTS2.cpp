/*
 * Author and Copyright of this file and of the stellarium telescope feature:
 * Johannes Gajdosik, 2006; adapted for RTS2 by Petr Kubanek, 2009
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "ServerRTS2.hpp"
#include "LogFile.hpp"

#ifdef WIN32
  #include "Socket.hpp" // winsock2
#else
  #include <signal.h>
#endif

#include <iostream>
#include <string.h>
using namespace std;


static volatile bool continue_looping = true;

#ifdef WIN32
static
BOOL signal_handler(DWORD fdwCtrlType) {
  switch (fdwCtrlType) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_SHUTDOWN_EVENT:
      continue_looping = false;
      return TRUE;
    case CTRL_LOGOFF_EVENT:
      break;
  }
  return FALSE;
}
#else
#include <signal.h>
static
void signal_handler(int signum) {
  switch (signum) {
    case SIGINT:
    case SIGQUIT:
    case SIGTERM:
      continue_looping = false;
      break;
    default:
        // just ignore
      break;
  }
}
#endif


int main(int argc,char *argv[]) {
  cout << "This is " << argv[0] << ", built at "
       << __DATE__ << ", " << __TIME__ << endl;
#ifdef WIN32
  if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)signal_handler,TRUE)) {
    cout << "SetConsoleCtrlHandler failed" << endl;
    return 127;
  }
  WSADATA wsaData;
  if (WSAStartup(0x202,&wsaData) != 0) {
    cout << "WSAStartup failed" << endl;
    return 127;
  }
#else
    // SIGPIPE is normal operation when we send while the other side
    // has already closed the socket. We must ignore it:
  signal(SIGPIPE,SIG_IGN);
  signal(SIGINT,signal_handler);
  signal(SIGTERM,signal_handler);
  signal(SIGQUIT,signal_handler);
    // maybe the user wants to continue after SIGHUP ?
  //signal(SIGHUP,signal_handler);
#endif

  int port;
  if ((argc != 3 && argc != 4) ||
      1 != sscanf(argv[1],"%d",&port) ||
      port < 0 || port > 0xFFFF) {
    cout << "Usage: " << argv[0] << " port http://user:password@server:port [logfile]" << endl;
    return 126;
  }
  char *xmlrpc_server;
  int xmlrpc_port;
  char *auth;
  // parse HTTP string..
  if (strstr(argv[2],"http://") != argv[2]) {
    cout << "XML-RPC server string must start with http://!" << endl;
    return 127;
  }
  if ((auth = strchr(argv[2],'@')) != NULL) {
    *auth = '\0';
    xmlrpc_server = auth+1;
    auth = argv[2]+7;
  }
  else {
    auth = NULL;
    xmlrpc_server = argv[2]+7;
  }
  // get port..
  char *xml_p = strchr(xmlrpc_server,':');
  if (xml_p != NULL) {
    *xml_p = '\0';
    xml_p++;
    if (1 != sscanf(xml_p,"%d",&xmlrpc_port) || xmlrpc_port < 0 ||
        xmlrpc_port > 0xFFFF) {
      cout << "Invalid XML-RPC port: " << xml_p << endl;
      return 128;
    }
  }
  else {
    xmlrpc_port = 80;
  }
  if (*xmlrpc_server == '\0') {
    cout << "You must specify at least server name!" << endl;
    return 129;
  }

  if (argc == 4) {
    SetLogFile(argv[3]);
    *log_file << Now() << "This is " << argv[0] << ", built on "
              << __DATE__ << ", " << __TIME__ << endl;
  }
#ifdef DEBUG3
  if (auth != NULL)
    *log_file << "Connecting to : http://" << auth << "@" << xmlrpc_server << ":" << xmlrpc_port << endl;
  else
    *log_file << "Connecting to : http://" << xmlrpc_server << ":" << xmlrpc_port << endl;
#endif
  try {
    ServerRTS2 server(port,xmlrpc_server,xmlrpc_port,auth);
    while (continue_looping) {
      server.step(10000);
    }
  } catch (exception &ex) {
    *log_file << "Throw " << ex.what() << endl;
  }

#ifdef WIN32
  WSACleanup();
#endif
  *log_file << Now() << "bye." << endl;
  return 0;
}
