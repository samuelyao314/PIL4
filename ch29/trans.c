#include "lua.h"
#include "lauxlib.h"

#include <string.h>

#include "util.h"


static int l_gettrans(lua_State *L) {
    lua_pushvalue(L, lua_upvalueindex(1));
    return 1;
}

static int l_settrans(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);


    // 以下代码，无法直接覆盖library 级别的upvalue
    // lua_copy(L, 1, lua_upvalueindex(1));


    // 方法2：删掉表的内容，插入新的kv对
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushnil(L);  /* 第一个键 */
    while (lua_next(L, 2) != 0) {
        /* 使用 '键' （在索引 -2 处） 和 '值' （在索引 -1 处）*/
        printf("%s - %s\n",
               lua_typename(L, lua_type(L, -2)),
               lua_typename(L, lua_type(L, -1)));
        /* 移除 '值' ；保留 '键' 做下一次迭代 */
        lua_pop(L, 1);
        lua_pushvalue(L, -1);   // 复制 key
        lua_pushnil(L);
        lua_settable(L, 2);  // t[k] = nil
    }

    lua_pushnil(L);  /* 第一个键 */
    while (lua_next(L, 1) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);   // key, key, value
        lua_settable(L, 2);
    }

    return 0;
}

static int l_transliterate(lua_State *L) {
    const char *s = luaL_checkstring(L, 1);
    lua_settop(L, 1);
    lua_pushvalue(L, lua_upvalueindex(1));
    luaL_checktype(L, 2, LUA_TTABLE);

    luaL_Buffer b;
    luaL_buffinit(L, &b);
    for (int i = 0; i < strlen(s); i++) {
        lua_pushlstring(L, s + i, 1);
        lua_gettable(L, 2);
        if (lua_isstring(L, -1)) {
            const char *value = luaL_checkstring (L, -1);
            luaL_addvalue(&b);
        } else {
            lua_pop(L, 1);
        }
    }

    luaL_pushresult(&b);
    lua_replace(L, 1);
    lua_settop(L, 1);
    return 1;
}



static const struct luaL_Reg translib[] = {
    {"settrans", l_settrans},
    {"gettrans", l_gettrans},
    {"transliterate", l_transliterate},
    {NULL, NULL}
};


// Exercise 30.5 a library with a modification of transliterate
int luaopen_trans(lua_State *L) {
    luaL_newlibtable(L, translib);
    lua_newtable(L);
    luaL_setfuncs(L,translib, 1);    // shareing a table as upvalu

    return 1;
}