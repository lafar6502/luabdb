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

DB** luabdb_todb(lua_State *L, int narg)
{
    luaL_checktype(L, narg, LUA_TUSERDATA);
    DB** pp = (DB_ENV**) luaL_checkudata(L, narg, LUABDB_DB);
    if (pp == NULL) {
        luaL_typerror(L, narg, LUABDB_DB);
    }
    return pp;
}

static int db_op_close(lua_State *L)
{
    DB** pp = luabdb_todb(L, 1);
    if (*pp != NULL)
    {
        dbgprint("closing 0x%x\n", *pp);
        (*pp)->close(*pp, 0);
        *pp = NULL;
    }
    return 0;
}

///wraps DB* in userdata and pushes it to the lua stack
///returns userdata address
DB** push_db(lua_State *L, DB* db)
{
    dbgprint("push_db\n");
    DB** pd = (DB**) lua_newuserdata(L, sizeof(DB*));
    dbgprint("push_db userdata created \n");
    luaL_getmetatable(L, LUABDB_DB);
    lua_setmetatable(L, -2);
    *pd = db;
    return pd;
}

#define _(name) { #name, db_op_##name }
#define u_(name) { #name, luabdb_unimplemented }

static luaL_Reg db_funcs[] = {
    _(close),
    { NULL, NULL }
};
#undef _
#undef u_

static int db__gc(lua_State *L)
{
    lua_pushcfunction(L, db_op_close);
    lua_pushvalue(L, 1);
    lua_pcall(L, 1, 0, 0);
    return 0;
}

static int db__tostring(lua_State *L)
{
    lua_pushfstring(L, "db: %p", lua_topointer(L, 1));
    return 1;
}

int init_db_ops(lua_State *L)
{
	dbgprint("init_db_ops\n");
    luaL_getmetatable(L, LUABDB_DB);
    lua_pushcfunction(L, db__gc);
    lua_setfield(L, -2, "__gc");
    lua_pushcfunction(L, db__tostring);
    lua_setfield(L, -2, "__tostring");
    lua_getfield(L, -1, "__index");
    luaL_register(L, NULL, db_funcs);
    return 0;
}
