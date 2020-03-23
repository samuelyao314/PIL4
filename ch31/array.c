// BIT 数组
// PIL, Chapter 31. User-Defined Types in C
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "lua.h"
#include "lauxlib.h"

#define BITS_PER_WORD (CHAR_BIT * sizeof(unsigned int))
#define I_WORD(i) ((unsigned int)(i) / BITS_PER_WORD)
#define I_BIT(i) (1 << ((unsigned int)(i) % BITS_PER_WORD))

typedef struct BitArray {
    int size;
    unsigned int values[1];
} BitArray;

#define checkarray(L) ((BitArray *) luaL_checkudata(L, 1, "LuaBook.array"))


void new_array(lua_State *L, int n) {
    size_t nbytes = sizeof(BitArray) + I_WORD(n-1)*sizeof(unsigned int);
    BitArray *a = (BitArray *)lua_newuserdata(L, nbytes);

    a->size = n;
    for (int i = 0; i <= I_WORD(n-1); i++) {
        a->values[i] = 0;
    }

    luaL_getmetatable(L, "LuaBook.array");
    lua_setmetatable(L, -2);
}

static int newarray (lua_State *L) {
    int i;

    int n = (int)luaL_checkinteger(L, 1);
    luaL_argcheck(L, n >= 1, 1, "invalid size");

    new_array(L, n);

    return 1;
}

static unsigned int *getparams (lua_State *L, unsigned int *mask) {
    BitArray *a = checkarray(L);
    int index = (int)luaL_checkinteger(L, 2) - 1;
    luaL_argcheck(L, 0 <= index < a->size, 2, "index out of range");
    *mask = I_BIT(index);
    return &a->values[I_WORD(index)];
}

static int setarray(lua_State *L) {
    unsigned int mask;
    unsigned int *entry = getparams(L, &mask);

    // luaL_checkany(L, 3);
    // Exercise 31.1
    luaL_argcheck(L, lua_isboolean(L, 3), 3, "argument is not boolean");

    if (lua_toboolean(L, 3)) {
        *entry |= mask;
    } else {
        *entry &= ~mask;
    }

    return 0;
}

static int getarray(lua_State *L) {
    unsigned int mask;
    unsigned int *entry = getparams(L, &mask);
    lua_pushboolean(L, *entry & mask);
    return 1;
}

static int getsize(lua_State *L) {
    BitArray *a = checkarray(L);
    lua_pushinteger(L, a->size);
    return 1;
}

static int array2string(lua_State *L) {
    BitArray *a = checkarray(L);

    // Exercise 31.4
    luaL_Buffer buffer;
    luaL_buffinit(L, &buffer);
    lua_pushfstring(L, "array(%d): ", a->size);
    luaL_addvalue(&buffer);

    for (int i = 0; i < a->size; i++) {
        if (i == 0) {
            luaL_addchar(&buffer, '[');
        }
        unsigned int mask = I_BIT(i);
        unsigned int b = a->values[I_WORD(i)] & mask;
        // printf("i = %d, mask = %x, b = %x\n", i, mask, b);
        if (b) {
            lua_pushfstring(L, "%d,", i + 1);
            luaL_addvalue(&buffer);
        }
        if (i == a->size  - 1) {
            luaL_addchar(&buffer, ']');
        }
    }
    luaL_pushresult(&buffer);

    return 1;
}


static int unionarray(lua_State *L) {
    BitArray  *a1 = (BitArray *) luaL_checkudata(L, 1, "LuaBook.array");
    BitArray  *a2 = (BitArray *) luaL_checkudata(L, 2, "LuaBook.array");

    int big_size;
    int small_size;
    BitArray *big_a;

    if (a1->size >= a2->size) {
        big_size = a1->size;
        small_size = a2->size;
        big_a = a1;
    } else {
        big_size = a2->size;
        small_size = a1->size;
        big_a = a2;
    }

    new_array(L, big_size);    // a1, a2, new_a
    BitArray *new_a = (BitArray *) luaL_checkudata(L, 3, "LuaBook.array");

    for (int i = 0; i < small_size; i++) {
        unsigned int mask = I_BIT(i);
        unsigned int b = (a1->values[I_WORD(i)] & mask) | (a2->values[I_WORD(i)] & mask);
        if (b) {
            new_a->values[I_WORD(i)] |= mask;
        }
    }

    for (int i = small_size; i < big_size; i++) {
        unsigned int mask = I_BIT(i);
        unsigned int b = big_a->values[I_WORD(i) & mask];
        if (b) {
            new_a->values[I_WORD(i)] |= mask;
        }
    }

    return 1;
}



static int intersection_array(lua_State *L) {
    BitArray  *a1 = (BitArray *) luaL_checkudata(L, 1, "LuaBook.array");
    BitArray  *a2 = (BitArray *) luaL_checkudata(L, 2, "LuaBook.array");

    int small_size;
    if (a1->size >= a2->size) {
        small_size = a2->size;
    } else {
        small_size = a1->size;
    }

    new_array(L, small_size);    // a1, a2, new_a
    BitArray *new_a = (BitArray *) luaL_checkudata(L, 3, "LuaBook.array");

    for (int i = 0; i < small_size; i++) {
        unsigned int mask = I_BIT(i);
        unsigned int b = (a1->values[I_WORD(i)] & mask) & (a2->values[I_WORD(i)] & mask);
        if (b) {
            new_a->values[I_WORD(i)] |= mask;
        }
    }


    return 1;
}


static const struct luaL_Reg arraylib_f [] = {
    {"new", newarray},
    {NULL, NULL},
};

static const struct luaL_Reg arraylib_m[] = {
    {"__index", getarray},
    {"__newindex", setarray},
    {"__len", getsize},
    {"__tostring", array2string},
    {"__add", unionarray},
    {"__mul", intersection_array},
    {NULL, NULL},
};


int luaopen_array (lua_State *L) {
    luaL_newmetatable(L, "LuaBook.array");
    lua_pushvalue(L, -1);  /* duplicate the metatable */
    lua_setfield(L, -2, "__index");  /* mt.__index = mt */
    luaL_setfuncs(L, arraylib_m, 0);  /* register metamethods */
    luaL_newlib(L, arraylib_f);    /* create lib table */
    return 1;
}
