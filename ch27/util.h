#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "lua.h"

void error(lua_State *L, const char *fmt, ...);
void stackDump(lua_State *L);
void call_va(lua_State *L, const char *func, const char *sig, ...);
