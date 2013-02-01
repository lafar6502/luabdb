#include "luabdb_private.h"
#include <lauxlib.h>
#include <lualib.h>

static char* g_bidiMapKey = "bidimap";

///bi-directional c<-->lua mapping registry (weak table)
void create_bidi_registry(lua_State *L)
{
    dbgprint("create_bidi_registry %d\n", lua_gettop(L));
    lua_pushlightuserdata(L, &g_bidiMapKey);
    lua_newtable(L);
    
    lua_createtable(L,  0, 1);
    lua_pushstring(L,   "v");
    lua_setfield(L,     -2, "__mode");
    lua_setmetatable(L, -2);

    lua_rawset(L, LUA_REGISTRYINDEX);
    dbgprint("create_bidi_registry end %d\n", lua_gettop(L));
}

///stores a mapping (ptrKey->lua object at index) in a mapping registry
int store_bidi_in_registry(lua_State *L, void* ptrKey, int index)
{
    dbgprint("store_bidi_in_registry %d\n", lua_gettop(L));
    lua_pushlightuserdata(L, &g_bidiMapKey);
    lua_rawget(L,            LUA_REGISTRYINDEX);
    if (!lua_istable(L, -1)) {
        luaL_error(L, "not a table");
    }

    lua_pushlightuserdata(L, ptrKey);
    lua_pushvalue(L, index);
    lua_rawset(L, -3);
    lua_pop(L, 1);
    dbgprint("store_bidi_in_registry end %d\n", lua_gettop(L));
}

///retrieve lua object from the mapping registry and push it on the stack
void get_lua_object_from_registry(lua_State *L, void *ptrKey)
{
    dbgprint("get_bidi_from_registry %d\n", lua_gettop(L));
    
    lua_pushlightuserdata(L, &g_bidiMapKey);
    lua_rawget(L,            LUA_REGISTRYINDEX);
    if (!lua_istable(L, -1)) {
        luaL_error(L, "not a table");
    }

    lua_pushlightuserdata(L, ptrKey);
    lua_rawget(L, -2);
    lua_remove(L, -2);
    dbgprint("get_bidi_from_registry end %d\n", lua_gettop(L));
    
    return 1;
}
//dump current lua stack
void stackdump(lua_State* l)
{
    int i;
    int top = lua_gettop(l);
 
    printf("total in stack %d\n",top);
 
    for (i = 1; i <= top; i++)
    {  /* repeat for each level */
        int t = lua_type(l, i);
        switch (t) {
            case LUA_TSTRING:  /* strings */
                printf("string: '%s'\n", lua_tostring(l, i));
                break;
            case LUA_TBOOLEAN:  /* booleans */
                printf("boolean %s\n",lua_toboolean(l, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:  /* numbers */
                printf("number: %g\n", lua_tonumber(l, i));
                break;
            default:  /* other values */
                printf("%s\n", lua_typename(l, t));
                break;
        }
        printf("  ");  /* put a separator */
    }
    printf("\n");  /* end the listing */
}