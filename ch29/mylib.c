#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "lua.h"
#include "lauxlib.h"

#include "util.h"


// Exercise 29.1
static int l_summation(lua_State *L) {
    double s;

    int i;
    for (i = 1; i <= lua_gettop(L); i++) {
        double d = luaL_checknumber(L, i);
        s = d + s;
    }

    lua_pushnumber(L, s);
    return 1;
}

// Exercise 29.2
static int l_pack(lua_State *L) {
    int n = lua_gettop(L);

    lua_newtable(L);
    int i;
    for (i = 1; i <= n; i++) {
        lua_pushvalue(L, i);
        lua_seti(L, -2, i);
    }

    lua_pushinteger(L, n);
    lua_setfield(L, -2, "n");

    return 1;
}

static int l_sin(lua_State *L) {
    double d = luaL_checknumber(L, 1);
    lua_pushnumber(L, sin(d));
    return 1;
}

static int l_dir(lua_State *L) {
    DIR *dir;
    struct dirent *entry;
    int i;
    const char *path = luaL_checkstring(L, 1);

    dir = opendir(path);
    if (dir == NULL) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }
    lua_newtable(L);
    i = 1;
    while ((entry = readdir(dir)) != NULL) {
        lua_pushinteger(L, i++);
        lua_pushstring(L, entry->d_name);
        lua_settable(L, -3);
    }

    closedir(dir);
    return 1;
}

static int l_reverse(lua_State *L) {
    int n = lua_gettop(L);

    int i;
    for (i = 1; i <= n/2; i++) {
        int j = (n+1-i);
        // swap
        lua_pushvalue(L, i);
        lua_pushvalue(L, j);
        lua_replace(L, i);
        lua_replace(L, j);
    }

    return n;
}


typedef struct foreach_ctx {
    int idx;  // 当前处于遍历第几个 (k, v) 对
    int len;   // 表的大小
} foreach_ctx_t;

// lua table 是无序的, 每次迭代都是不一样的, 通过以下方式保持有序
// table.sort(keys);  for key in keys:  fun(key) 
int new_table_with_keys(lua_State *L) {
    lua_newtable(L);
    int n = 1;
    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
        lua_pushvalue(L, -2);
        lua_rawseti(L, -4, n);    
        lua_pop(L, 1);
        n++;
    }
    return n - 1;
}

int k(lua_State *L, int status, lua_KContext c) {
    foreach_ctx_t *ctx = (foreach_ctx_t *) c;
    ctx->idx++;
    //printf("idx: %d, len: %d\n", ctx->idx, ctx->len);

    if (status == LUA_YIELD) {
        new_table_with_keys(L); 
    }

    if (ctx->idx <= ctx->len) {
        if (status == LUA_OK || status == LUA_YIELD) {
            lua_geti(L, -1, ctx->idx);   // get t2[idx]， 得到 key
            lua_pushvalue(L, -1);
            lua_gettable(L, 1);
            lua_pushvalue(L, 2);   // 复制 func
            lua_insert(L, -3);     // func, key, value
            return k(L, lua_pcallk(L, 2, 0, 0, c, k), c);
        }
    }

    if (ctx) {
        free(ctx);
        ctx = NULL;
    }

    return 0;
}

// Exercise 29.5
static int l_foreach_2(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    int len = new_table_with_keys(L);    // 执行后:  t, func,  t2 (保存有序的keys)
    if (len == 0) {
        return 0;   // 空表
    }

    foreach_ctx_t *ctx  = malloc(sizeof(foreach_ctx_t));
    ctx->idx = 0;   // 开始遍历第一个
    ctx->len = len;
    return k(L, LUA_OK, (lua_KContext) ctx);
}

// Exercise 29.4
static int l_foreach(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
        lua_pushvalue(L, 2);
        lua_pushvalue(L, 3);
        lua_pushvalue(L, 4);
        int error  = lua_pcall(L, 2, 0, 0);
        if (error) {
            lua_error(L);
        }
        lua_pop(L, 1);
    }

    return 0;
}

// 30.1
static int l_map(lua_State *L) {
    int i, n;

    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    n = luaL_len(L, 1);
    for (i = 1; i <= n; i++) {
        lua_pushvalue(L, 2);
        lua_geti(L, 1, i);
        lua_call(L, 1, 1);
        lua_seti(L, 1, i);
    }
    return 0;
}

// 30.2
static int l_split(lua_State *L) {
    const char *s = luaL_checkstring(L, 1);   /* subject */
    const char *sep = luaL_checkstring(L, 2); /* separator */

    lua_newtable(L);
    char *e;
    int i = 1;
    while ((e = strchr(s, *sep)) != NULL) {
        lua_pushlstring(L, s, e - s);
        lua_rawseti(L, -2, i++);
        s = e + 1;
    }
    lua_pushstring(L, s);
    lua_rawseti(L, -2, i);
    return 1;
}

// 30.3
static int l_upper(lua_State *L) {
    size_t l;
    size_t i;
    luaL_Buffer b;
    const char *s = luaL_checklstring(L, 1, &l);
    char *p = luaL_buffinitsize(L, &b, l);
    for (i = 0; i < l; i++) {
         p[i] = toupper(s[i]);
    }
    luaL_pushresultsize(&b, l);
    return 1;
}

// 30.4
// A simplified implementation for table.concat
static int l_concat(lua_State *L) {
    int i, n;
    luaL_checktype(L, 1, LUA_TTABLE);
    n = luaL_len(L, 1);
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    for (i = 1; i <= n; i++) {
        lua_geti(L, 1, i);
        luaL_addvalue(&b);
    }
    luaL_pushresult(&b);
    return 1;
}

static int l_counter(lua_State *L) {
    int val = lua_tointeger(L, lua_upvalueindex(1));
    lua_pushinteger(L, ++val);
    lua_copy(L, -1, lua_upvalueindex(1));   /* update upvalue */
    return 1;
}

static int newCounter(lua_State *L) {
    lua_pushinteger(L, 0);
    lua_pushcclosure(L, &l_counter, 1);
    return 1;
}


// Exercise 30.1
static int l_filter(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    lua_newtable(L);
    int n = 1;
    for (int i = 1; i < luaL_len(L, 1); i++) {
        lua_pushvalue(L, 2);
        lua_geti(L, 1, i);
        lua_call(L, 1, 1);
        int b = lua_toboolean(L, -1);
        lua_pop(L, 1);
        if (b) {
            lua_geti(L, 1, i);
            lua_seti(L, 3, n++);
        }
    }
    lua_replace(L, 1);
    lua_settop(L, 1);
    return 1; 
}


// Exercise 30.2
static int l_split_2(lua_State *L) {
    size_t l;
    const char *s = luaL_checklstring(L, 1, &l);   /* subject */
    const char *sep = luaL_checkstring(L, 2); /* separator */

    lua_newtable(L);
    char *e;
    int i = 1;
    while ((e = memchr(s, *sep, l)) != NULL) {
        lua_pushlstring(L, s, e - s);
        lua_rawseti(L, -2, i++);
        l = l - (e - s);
        s = e + 1;
    }
    lua_pushlstring(L, s, l);
    lua_rawseti(L, -2, i);
    return 1;
}


// Exercise 30.3
static int l_transliterate(lua_State *L) {
    const char *s = luaL_checkstring(L, 1);
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

static const struct luaL_Reg mylib[] = {
    {"dir", l_dir},
    {"sin", l_sin},
    {"summation", l_summation},
    {"pack", l_pack},
    {"reverse", l_reverse},
    {"foreach", l_foreach},
    {"foreach_2", l_foreach_2},
    {"map", l_map},
    {"split", l_split},
    {"upper", l_upper},
    {"concat", l_concat},
    {"newCounter", newCounter},
    {"filter", l_filter},
    {"split_2", l_split_2},
    {"transliterate", l_transliterate},
    {NULL, NULL},
};


int luaopen_mylib (lua_State *L) {
    luaL_newlib(L, mylib);
    return 1;
}
