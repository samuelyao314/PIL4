#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"

void error(lua_State *L, const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    vfprintf(stderr, fmt, argp);
    va_end(argp);
    lua_close(L);
    exit(EXIT_FAILURE);
}

void stackDump(lua_State *L) {
    int i;
    int top = lua_gettop(L);
    for (i = 1; i <= top; i++) {
        int t = lua_type(L, i);
        switch (t) {
            case LUA_TSTRING: {
                printf("'%s'", lua_tostring(L, i));
                break;
            }
            case LUA_TBOOLEAN: {
                printf(lua_toboolean(L, i) ? "true" : "false");
                break;
            }
            case LUA_TNUMBER: {
                if (lua_isinteger(L, i)) {
                    printf("%lld", lua_tointeger(L, i));
                } else {
                    printf("%g", lua_tonumber(L, i));
                }
                break;
            }
            default: {
                printf("%s", lua_typename(L, t));
                break;
            }
        }
        printf("   ");
    }
    printf("\n");
}


void call_va(lua_State *L, const char *func, const char *sig, ...) {
    va_list vl;
    int narg, nres;  /* number of arguments and results */

    va_start(vl, sig);
    lua_getglobal(L, func);  /* push function */

    // push and count arguments
    for (narg = 0; *sig; narg++) {
        luaL_checkstack(L, 1, "too many arguments");
        switch (*sig++) {
            case 'd':
                lua_pushnumber(L, va_arg(vl, double));
                break;
                
            case 'i':
                lua_pushinteger(L, va_arg(vl, int));
                break;

            case 's':
                lua_pushstring(L, va_arg(vl, char *));
                break;

            /* Exercise 28.2 */
            case 'b': 
                lua_pushboolean(L, va_arg(vl, int));
                break;

            case '>':
                goto endargs;

            default:
                error(L, "invalid option(%c)", *(sig - 1));
        }
    }
    endargs:

    nres = strlen(sig);    /* number of expected results */
    if (lua_pcall(L, narg, nres, 0) != 0) {
        error(L, "error calling '%s': %s", func, lua_tostring(L, -1));
    }


    // retrieve results
    nres = -nres;
    while (*sig) {
        switch (*sig++) {
            case 'd': {
                int isnum;
                double n = lua_tonumberx(L, nres, &isnum);
                if (!isnum)
                    error(L, "not number type");
                *va_arg(vl, double *) = n;
                break;
            }

            case 'i': {
                int isnum;
                int n = lua_tointegerx(L, nres, &isnum);
                if (!isnum)
                    error(L, "not integer type");
                *va_arg(vl, int *) = n;
                break;
            }

            case 's': {
                const char *s = lua_tostring(L, nres);
                if (s == NULL) {
                    error(L, "not string type");
                }
                *va_arg(vl, const char **) = s;
                break;
            }
            
            /* Exercise 28.2 */
            case 'b': {
               int b = lua_toboolean(L, nres);
               *va_arg(vl, int *) = b;
               break;
            }

            default: 
                error(L, "invalid option (%c)", *(sig - 1));
        }
    }


    va_end(vl);
}