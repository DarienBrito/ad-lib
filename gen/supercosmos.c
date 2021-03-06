#include <math.h>
#include <float.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#ifdef WINDOWS
#include <malloc.h>
#endif
#ifdef WINDOWS
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif
#include <luajit-2.0/lauxlib.h>
#include <luajit-2.0/lua.h>
#include <luajit-2.0/lualib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#ifdef GLEW
#include <GL/glew.h>
#elif defined DARWIN
#include <OpenGL/gl.h>
#elif defined IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#define GLES
#endif
#include "code/misc.h"
#include "code/vecmat.h"
#include "code/glts.h"
#include "code/util.h"
#include "code/SDL.h"
#include "code/bronze.h"
#include "code/socket.h"
#include "code/hot.h"
#include "code/hot_player.h"
#include "gen/misc.h"
#include "gen/vecmat.h"
#include "gen/libs.h"
#include "gen/glts.h"
#include "gen/core.h"
#include "gen/util.h"
#include "gen/bronze.h"
#include "gen/socket.h"
#include "gen/hot.h"
#include "gen/hot_player.h"
#ifdef HOTREMOTE
#include "gen/hot_network.h"
#endif
#include "gen/cosmos.h"
#include "code/misc.c"
#include "code/vecmat.c"
#include "code/SDL.c"
#include "code/glts.c"
#include "code/util.c"
#include "code/bronze.c"
#include "code/core.c"
#include "code/socket.c"
#include "code/hot.c"
#include "code/hot_player.c"
#include "code/cosmos.c"
#ifdef HOTREMOTE
#include "code/hot_network.c"
#endif

int main(int argc, char * argv[]) {return cosmos(argc,argv);}
