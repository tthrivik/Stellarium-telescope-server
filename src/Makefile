# for cross compilation with mingw export the path to your cross compiler:
# export CROSS_GPP=/usr/x86_64-pc-linux-gnu/i686-mingw32/gcc-bin/4.3.2/i686-mingw32-g++

#   RTS2 only:
# TelescopeServerRTS2 TelescopeServerRTS2.exe are not included in the all,exe
# targets because they require an external lib.
# You can build them like this:
#make TelescopeServerRTS2
# You will need to provide path for xmlrpc++ library from RTS2 distribution. Please get it with:
# svn co https://rts-2.svn.sourceforge.net/svnroot/rts-2/trunk/rts-2/src/xmlrpc/xmlrpc++
#export XMLRPCPP=/pathtoxmlrpclib++_from_rts2


TelescopeServerDummy_CPP = Connection.cpp Listener.cpp Socket.cpp \
  Server.cpp ServerDummy.cpp LogFile.cpp MainDummy.cpp

TelescopeServerDummy_O = $(TelescopeServerDummy_CPP:.cpp=.o)
TelescopeServerDummy_OBJ = $(TelescopeServerDummy_CPP:.cpp=.obj)

TelescopeServerLx200_CPP = Connection.cpp Listener.cpp Socket.cpp \
  SerialPort.cpp Lx200Connection.cpp Lx200Command.cpp \
  Server.cpp ServerLx200.cpp LogFile.cpp MainLx200.cpp

TelescopeServerLx200_O = $(TelescopeServerLx200_CPP:.cpp=.o)
TelescopeServerLx200_OBJ = $(TelescopeServerLx200_CPP:.cpp=.obj)

TelescopeServerNexStar_CPP = Connection.cpp Listener.cpp Socket.cpp \
  SerialPort.cpp NexStarConnection.cpp NexStarCommand.cpp \
  Server.cpp ServerNexStar.cpp LogFile.cpp MainNexStar.cpp

TelescopeServerNexStar_O = $(TelescopeServerNexStar_CPP:.cpp=.o)
TelescopeServerNexStar_OBJ = $(TelescopeServerNexStar_CPP:.cpp=.obj)

TelescopeServerRTS2_CPP = Connection.cpp Listener.cpp Socket.cpp \
  Server.cpp ServerRTS2.cpp LogFile.cpp MainRTS2.cpp

TelescopeServerRTS2_O = $(TelescopeServerRTS2_CPP:.cpp=.o)
TelescopeServerRTS2_OBJ = $(TelescopeServerRTS2_CPP:.cpp=.obj)

CCFLAGS = -O4 -Wall -Wextra -Wno-unused-parameter -Werror -DDEBUG3 -DDEBUG4 -DDEBUG5
LDFLAGS = -O4 -s

CCFLAGS = -O4 -Wall -Wextra -Wno-unused-parameter -Werror -DDEBUG3 -DDEBUG4 -DDEBUG5

ifneq ($(XMLRPCPP),)
  CCFLAGS += -I$(XMLRPCPP) -lrts2xmlrpc++
endif


.SUFFIXES: .cpp .o .obj

.cpp.o:
	g++ $(CCFLAGS) $< -c -o $@

.cpp.obj:
	$(CROSS_GPP) $(CCFLAGS) $< -c -o $@

all: TelescopeServerDummy TelescopeServerLx200 TelescopeServerNexStar

exe: TelescopeServerDummy.exe TelescopeServerLx200.exe TelescopeServerNexStar.exe

install_linux: TelescopeServerDummy TelescopeServerLx200 TelescopeServerNexStar
	install -d ~/.stellarium/servers
	install TelescopeServerDummy ~/.stellarium/servers
	install TelescopeServerLx200 ~/.stellarium/servers
	install TelescopeServerNexStar ~/.stellarium/servers

TelescopeServerDummy: $(TelescopeServerDummy_O)
	g++ $(LDFLAGS) $(TelescopeServerDummy_O) -o $@

TelescopeServerDummy.exe: $(TelescopeServerDummy_OBJ)
	$(CROSS_GPP) $(LDFLAGS) $(TelescopeServerDummy_OBJ) -o $@ \
	-lws2_32

TelescopeServerLx200: $(TelescopeServerLx200_O)
	g++ $(LDFLAGS) $(TelescopeServerLx200_O) -o $@

TelescopeServerLx200.exe: $(TelescopeServerLx200_OBJ)
	$(CROSS_GPP) $(LDFLAGS) $(TelescopeServerLx200_OBJ) -o $@ \
	-lws2_32

TelescopeServerNexStar: $(TelescopeServerNexStar_O)
	g++ $(LDFLAGS) $(TelescopeServerNexStar_O) -o $@

TelescopeServerNexStar.exe: $(TelescopeServerNexStar_OBJ)
	$(CROSS_GPP) $(LDFLAGS) $(TelescopeServerNexStar_OBJ) -o $@ \
	-lws2_32

TelescopeServerRTS2: $(TelescopeServerRTS2_O)
	g++ $(LDFLAGS) $(TelescopeServerRTS2_O) $(XMLRPCPP)/libxmlrpc_rts2.a -o $@

TelescopeServerRTS2.exe: $(TelescopeServerRTS2_OBJ)
	$(CROSS_GPP) $(LDFLAGS) $(TelescopeServerRTS2_OBJ) $(XMLRPCPP)/libxmlrpc_rts2.a -o $@ \
	-lws2_32

depend:
	makedepend -Y -- $(CCFLAGS) -- -f- >make.dep 2>/dev/null \
	$(TelescopeServerDummy_CPP) $(TelescopeServerLx200_CPP) \
	$(TelescopeServerNexStar_CPP) $(TelescopeServerRTS2_CPP)
	sed -e 's/\.o/\.obj/g' make.dep > make.cross_dep

clean:
	rm *.o *.obj

make.dep:
	touch make.dep

make.cross_dep:
	touch make.cross_dep

include make.dep
include make.cross_dep
