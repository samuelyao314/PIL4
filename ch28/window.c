#include <stdio.h>
#include <string.h>
#include  "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "util.h"

int getglobint(lua_State *L, const char *var) {
    int isnum, result;
    lua_getglobal(L, var);
    result = (int) lua_tointegerx(L, -1, &isnum);
    if (!isnum)
        error(L, "'%s' should be a number\n", var);
    lua_pop(L, 1);
    return result;
}

void load(lua_State *L, const char *fname, int *w, int *h) {
    if (luaL_loadfile(L, fname) || lua_pcall(L, 0, 0, 0))
        error(L, "cannot run config. file: %s", lua_tostring(L, -1));
    *w = getglobint(L, "width");
    *h = getglobint(L, "height");
}

#define MAX_COLOR 255

struct ColorTable {
    char *name;
    unsigned char red, green, blue;
} colortable[] = {
    {"WHITE", MAX_COLOR, MAX_COLOR, MAX_COLOR},
    {"RED", MAX_COLOR, 0, 0},
    {"GREEN", 0, MAX_COLOR, 0},
    {"BLUE", 0, 0, MAX_COLOR},
    {NULL, 0, 0, 0}
};

/* assume that table is on the top of the stack */
int getcolorfield(lua_State *L, const char *key) {
    int result, isnum;
    // lua_pushstring(L, key);
    // lua_gettable(L, -2);      /* get background[key] */
    lua_getfield(L, -1, key);
    result = (int)(lua_tonumberx(L, -1, &isnum) * MAX_COLOR);
    if (!isnum)
        error(L, "invalid componet '%s' in color", key);
    lua_pop(L, 1);   /*remove number */
    return result;
}

void setcolorfield(lua_State *L, const char *index, int value) {
    lua_pushstring(L, index);
    lua_pushnumber(L, (double)value / MAX_COLOR);
    lua_settable(L, -3);
}

void setcolor(lua_State *L, struct ColorTable *ct) {
    lua_newtable(L);
    setcolorfield(L, "red", ct->red);
    setcolorfield(L, "green", ct->green);
    setcolorfield(L, "blue", ct->blue);
    lua_setglobal(L, ct->name);
}

/* call a function 'f' defined in Lua */
double f(lua_State *L, double x, double y) {
    int isnum;
    double z;

    lua_getglobal(L, "f");
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    
    /* do the call (2 arguments, 1 result) */
    if (lua_pcall(L, 2, 1, 0) != LUA_OK) {
        error(L, "error running function 'f': %s\n", lua_tostring(L, -1));
    }
    z = lua_tonumberx(L, -1, &isnum);
    if (!isnum) {
        error(L, "function 'f' should return number\n");
    }
    lua_pop(L, 1);
    return z;
}

int main(int argc, char *argv[]) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    int w, h;

    load(L, argv[1], &w, &h);

    printf("width: %d\n", w);
    printf("height: %d\n", h);

    int red, green, blue;
    lua_getglobal(L, "background");
    if (lua_isstring(L, -1)) {
        const char *name = lua_tostring(L, -1);
        int i = 0;
        for (i = 0; colortable[i].name != NULL; i++) {
            if (strcmp(name, colortable[i].name) == 0) {
                break;
            }
        }
        red = colortable[i].red;
        green = colortable[i].green;
        blue = colortable[i].blue;
    } else if (lua_istable(L, -1)) {
        red = getcolorfield(L, "red");
        green = getcolorfield(L, "green");
        blue = getcolorfield(L, "blue");
    } else {
        error(L, "invalid value for 'background'");
    }

    printf("red: %d, green: %d, blue: %d\n", red, green, blue);

    printf("\n\ncall function 'f'\n");
    double x, y;
    printf("enter x: ");
    scanf("%lf", &x);
    printf("enter y: ");
    scanf("%lf", &y);
    double z = f(L, x, y);
    printf("f(%.2f, %.2f) = %.2f\n", x, y, z);


    printf("\n\ngeneric call function 'f'\n");
    double result = 0.0;
    call_va(L, "f", "dd>d", x, y, &result);
    printf("f(%.2f, %.2f) = %.2f\n", x, y, result);

    lua_close(L);
    return 0;
}

