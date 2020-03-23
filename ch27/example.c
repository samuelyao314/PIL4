#include <stdio.h>
#include "lua.h"
#include "lauxlib.h"

#include "util.h"

int main(void) {
    lua_State *L = luaL_newstate();

    lua_pushboolean(L, 1);
    lua_pushnumber(L, 10);
    lua_pushnil(L);
    lua_pushstring(L, "hello");
    stackDump(L);

    lua_pushvalue(L, -4); 
    stackDump(L);

    lua_replace(L, 3); 
    stackDump(L);

    lua_settop(L, 6);
    stackDump(L);

    lua_rotate(L, 3, 1);
    stackDump(L);
    
    lua_remove(L, -3); 
    stackDump(L);

    lua_settop(L, -5); 
    stackDump(L);

    lua_settop(L, 0);
    printf("\nExercise 27.2\n");
    lua_pushnumber(L, 3.5);
    lua_pushstring(L, "hello");
    lua_pushnil(L);
    lua_rotate(L, 1, -1);
    lua_pushvalue(L, -2);
    lua_remove(L, 1);
    lua_insert(L, -2);
    stackDump(L);

    lua_close(L);
    return 0;
}

