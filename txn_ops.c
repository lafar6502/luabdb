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

static int txn_op_close(lua_State *L)
{
    return 0;
}

///retrieve the transaction pointer
DB_TXN* check_txn(lua_State *L, int index)
{
    DB_TXN** ptx;
    luaL_checktype(L, index, LUA_TUSERDATA);
    ptx = (DB_TXN**) luaL_checkudata(L, index, LUABDB_TXN);
    if (ptx == NULL) {
            luaL_typerror(L, index, LUABDB_TXN);
    }
    if (*ptx == NULL) {
            luaL_error(L, "null transaction");
    }
    dbgprint("check_txn: 0x%x\n", *ptx);
    return *ptx;
}

DB_TXN* push_txn(lua_State *L, DB_TXN* tx)
{
    DB_TXN** ptx = (DB_TXN**) lua_newuserdata(L, sizeof(DB_TXN*));
    *ptx = tx;
    luaL_getmetatable(L, LUABDB_TXN);
    lua_setmetatable(L, -2);
    return *ptx;
}
#define _(name) { #name, txn_op_##name }
#define u_(name) { #name, luabdb_unimplemented }

static luaL_Reg txn_funcs[] = {
    _(close),
    u_(associate),
    { NULL, NULL }
};
#undef _
#undef u_

static int txn__gc(lua_State *L)
{
    dbgprint("txn_gc\n");
    DB_TXN* tx;
    tx = check_txn(L, 1);
    if (tx != NULL)
    {
        dbgprint("aborting a transaction 0x%x\n", tx);
        tx->abort(tx);
    }
        //lua_pushcfunction(L, db_op_close);
    //lua_pushvalue(L, 1);
    //lua_pcall(L, 1, 0, 0);
    return 0;
}

static int txn__tostring(lua_State *L)
{
    lua_pushfstring(L, "txn: %p", lua_topointer(L, 1));
    return 1;
}


int init_txn_ops(lua_State *L)
{
	dbgprint("init_txn_ops\n");
    luaL_getmetatable(L, LUABDB_TXN);
    lua_pushcfunction(L, txn__gc);
    lua_setfield(L, -2, "__gc");
    lua_pushcfunction(L, txn__tostring);
    lua_setfield(L, -2, "__tostring");
    lua_getfield(L, -1, "__index");
    luaL_register(L, NULL, txn_funcs);

    return 0;
}
