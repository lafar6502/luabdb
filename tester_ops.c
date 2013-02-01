/*
* Copyright (c) 2010 Rob Hoelz <rob@hoelz.ro>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "luabdb_private.h"
#include <db.h>
#include <lauxlib.h>
#include <lualib.h>
#include <string.h>


typedef struct _TESTER_OBJ
{
    int id;
    struct _TESTER_OBJ *assoc1;
} TESTER_OBJ;

static int tester_op_close(lua_State *L)
{
    
}

///retrieve the transaction pointer
TESTER_OBJ** check_tester(lua_State *L, int index)
{
    TESTER_OBJ** ptx;
    luaL_checktype(L, index, LUA_TUSERDATA);
    ptx = (TESTER_OBJ**) luaL_checkudata(L, index, LUABDB_TESTER);
    if (ptx == NULL) {
            luaL_typerror(L, index, LUABDB_TESTER);
    }
    return ptx;
}

TESTER_OBJ *luabdb_totester(lua_State *L, int narg)
{
    TESTER_OBJ** ptx = check_tester(L, narg);
    if (ptx != NULL)
    {
        return *ptx;
    }
    return NULL;
}

void push_tester(lua_State *L, TESTER_OBJ* pt)
{
    TESTER_OBJ** ptx = (TESTER_OBJ**) lua_newuserdata(L, sizeof(TESTER_OBJ*));
    *ptx = pt;
    luaL_getmetatable(L, LUABDB_TESTER);
    lua_setmetatable(L, -2);
}

static int tester_op_getId(lua_State *L)
{
    TESTER_OBJ** ptx = check_tester(L, 1);
    lua_Number id = (*ptx)->id;
    lua_pushnumber(L, id);
    return 1;
}

#define _(name) { #name, tester_op_##name }
#define u_(name) { #name, luabdb_unimplemented }

static luaL_Reg tester_funcs[] = {
    _(getId),
    u_(associate),
    { NULL, NULL }
};
#undef _
#undef u_

static int tester__gc(lua_State *L)
{
    DB_TXN** ptx = check_txn(L, 1);
    DB_TXN* tx = *ptx;
    if (tx != NULL)
    {
        dbgprint("aborting a transaction 0x%x\n", tx);
        int status = tx->abort(tx);
        *ptx = NULL;
        handle_dbexception(L, status);
    }
    return 0;
}

static int tester__tostring(lua_State *L)
{
    lua_pushfstring(L, "tester: %p", lua_topointer(L, 1));
    return 1;
}


int init_tester_ops(lua_State *L)
{
	dbgprint("init_tester_ops\n");
    luaL_getmetatable(L, LUABDB_TESTER);
    lua_pushcfunction(L, tester__gc);
    lua_setfield(L, -2, "__gc");
    lua_pushcfunction(L, tester__tostring);
    lua_setfield(L, -2, "__tostring");
    lua_getfield(L, -1, "__index");
    luaL_register(L, NULL, tester_funcs);
    return 0;
}
