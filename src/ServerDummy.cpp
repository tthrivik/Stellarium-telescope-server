/*
The stellarium telescope library helps building
telescope server programs, that can communicate with stellarium
by means of the stellarium TCP telescope protocol.
It also contains smaple server classes (dummy, Meade LX200).

Author and Copyright of this file and of the stellarium telescope library:
Johannes Gajdosik, 2006

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

#include "ServerDummy.hpp"
#include "Socket.hpp" // GetNow

#include <math.h>

ServerDummy::ServerDummy(int port)
            :Server(port)
{
	current_pos[0] = desired_pos[0] = 1.0;
	current_pos[1] = desired_pos[1] = 0.0;
	current_pos[2] = desired_pos[2] = 0.0;
	next_pos_time = -0x8000000000000000LL;
}

void ServerDummy::step(long long int timeout_micros)
{
	long long int now = GetNow();
	if (now >= next_pos_time)
	{
		next_pos_time = now + 500000;
		current_pos[0] = 3*current_pos[0] + desired_pos[0];
		current_pos[1] = 3*current_pos[1] + desired_pos[1];
		current_pos[2] = 3*current_pos[2] + desired_pos[2];
		double h = current_pos[0]*current_pos[0]
		         + current_pos[1]*current_pos[1]
		         + current_pos[2]*current_pos[2];
		
		if (h > 0.0)
		{
			h = 1.0 / sqrt(h);
			current_pos[0] *= h;
			current_pos[1] *= h;
			current_pos[2] *= h;
		}
		else
		{
			current_pos[0] = desired_pos[0];
			current_pos[1] = desired_pos[1];
			current_pos[2] = desired_pos[2];
		}
		
		const double ra = atan2(current_pos[1],current_pos[0]);
		const double dec = atan2(current_pos[2],
		                         sqrt(current_pos[0]*current_pos[0]+current_pos[1]*current_pos[1]));
		const unsigned int ra_int = (unsigned int)floor(
		                               0.5 +  ra*(((unsigned int)0x80000000)/M_PI));
		const int dec_int = (int)floor(0.5 + dec*(((unsigned int)0x80000000)/M_PI));
		const int status = 0;
		sendPosition(ra_int,dec_int,status);
	}
	
	Server::step(timeout_micros);
}

void ServerDummy::gotoReceived(unsigned int ra_int, int dec_int)
{
	const double ra = ra_int*(M_PI/(unsigned int)0x80000000);
	const double dec = dec_int*(M_PI/(unsigned int)0x80000000);
	const double cdec = cos(dec);
	desired_pos[0] = cos(ra)*cdec;
	desired_pos[1] = sin(ra)*cdec;
	desired_pos[2] = sin(dec);
}


