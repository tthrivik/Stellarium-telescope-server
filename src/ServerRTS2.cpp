/*
The stellarium telescope library helps building
telescope server programs, that can communicate with stellarium
by means of the stellarium TCP telescope protocol.
It also contains smaple server classes (dummy, Meade LX200).

Author and Copyright of this file and of the stellarium telescope library:
Johannes Gajdosik, 2006; adapted for RTS2 by Petr Kubanek, 2009

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "ServerRTS2.hpp"
#include "Socket.hpp" // GetNow
#include "LogFile.hpp"

#include <math.h>
#include <stdio.h>

ServerRTS2::ServerRTS2(int port,const char *server,int xmlrpc_port,const char *auth)
            :Server(port) {
  xmlClient = new XmlRpc::XmlRpcClient(server,xmlrpc_port,auth);
  next_pos_time = -0x8000000000000000LL;
}

ServerRTS2::~ServerRTS2()
{
  delete xmlClient;
}

void ServerRTS2::step(long long int timeout_micros) {
  long long int now = GetNow();
  if (now >= next_pos_time) {
    next_pos_time = now + 1000000;

    XmlRpc::XmlRpcValue out, result;
    out[0] = "T0";
    out[1] = "TEL";

    xmlClient->execute("rts2.value.get",out,result);
    double ra,dec;
    const char* r = ((std::string)result).c_str();
    sscanf(r, "%lf %lf",&ra,&dec);

    const unsigned int ra_int = (unsigned int)(floor(ra*(double)0x80000000/180.0));
    const int dec_int = (int)(floor(dec*(double)0x80000000/180.0));
    const int status = 0;
    sendPosition(ra_int,dec_int,status);
  }
  Server::step(timeout_micros);
}

void ServerRTS2::gotoReceived(unsigned int ra_int,int dec_int) {
  const double ra = ra_int*(180.0/(unsigned int)0x80000000);
  const double dec = dec_int*(180.0/(unsigned int)0x80000000);

  XmlRpc::XmlRpcValue out, result;
  char radec_buf[30];
  snprintf(radec_buf,30,"%lf %lf",ra,dec);

  out[0] = "B2";
  out[1] = "ORI";
  out[2] = radec_buf;

  *log_file << Now() << "ServerRTS2::gotoReceived: " << radec_buf << endl; 

  xmlClient->execute("rts2.value.set",out,result);
}


