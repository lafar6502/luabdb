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
DB_TXN** check_txn(lua_State *L, int index)
{
    DB_TXN** ptx;
    luaL_checktype(L, index, LUA_TUSERDATA);
    ptx = (DB_TXN**) luaL_checkudata(L, index, LUABDB_TXN);
    if (ptx == NULL) {
            luaL_typerror(L, index, LUABDB_TXN);
    }
    //dbgprint("check_txn: 0x%x\n", *ptx);
    return ptx;
}

DB_TXN *luabdb_totxn(lua_State *L, int narg)
{
    DB_TXN** ptx = check_txn(L, narg);
    if (ptx != NULL)
    {
        return *ptx;
    }
    return NULL;
}

///wrap a transaction in userdata and push it to the stack
DB_TXN* push_txn(lua_State *L, DB_TXN* tx)
{
    DB_TXN** ptx = (DB_TXN**) lua_newuserdata(L, sizeof(DB_TXN*));
    *ptx = tx;
    luaL_getmetatable(L, LUABDB_TXN);
    lua_setmetatable(L, -2);
    return *ptx;
}

static int txn_op_getId(lua_State *L)
{
    DB_TXN** ptx = check_txn(L, 1);
    DB_TXN* tx = *ptx;
    lua_Number id = tx->id(tx);
    lua_pushnumber(L, id);
    return 1;
}

static int txn_op_abort(lua_State *L)
{
    DB_TXN** ptx = check_txn(L, 1);
    DB_TXN* tx = *ptx;
    int status;
    if (tx != NULL)
    {
        dbgprint("aborting tran 0x%x\n", tx);
        status = tx->abort(tx);
        handle_dbexception(L, status);
        *ptx = NULL;
    }
    return 0;
}

static int txn_op_commit(lua_State *L)
{
    DB_TXN** ptx = check_txn(L, 1);
    DB_TXN* tx = *ptx;
    u_int32_t flags = 0;
    int status;
    if (lua_gettop(L) > 1)
    {
        flags = luabdb_getflags(L, 2);
    }
    if (tx != NULL)
    {
        dbgprint("commit tran 0x%x\n", tx);
        status = tx->commit(tx, flags);
        handle_dbexception(L, status);
        *ptx = NULL;
    }
    return 0;
}

static int txn_op_setName(lua_State *L)
{
    DB_TXN** ptx = check_txn(L, 1);
    DB_TXN* tx = *ptx;
    u_int32_t flags = 0;
    int status;
    const char* name = lua_tostring(L, 2);
    if (tx != NULL)
    {
        status = tx->set_name(tx, name);
        handle_dbexception(L, status);
    }
    return 0;
}

static int txn_op_getName(lua_State *L)
{
    DB_TXN** ptx = check_txn(L, 1);
    DB_TXN* tx = *ptx;
    u_int32_t flags = 0;
    int status;
    const char* name;
    if (tx != NULL)
    {
        status = tx->get_name(tx, &name);
        handle_dbexception(L, status);
        lua_pushstring(L, name);
        return 1;
    }
    return 0;
}

#define _(name) { #name, txn_op_##name }
#define u_(name) { #name, luabdb_unimplemented }

static luaL_Reg txn_funcs[] = {
    _(close),
    _(getId),
    _(abort),
    _(commit),
    _(setName),
    _(getName),
    u_(associate),
    { NULL, NULL }
};
#undef _
#undef u_

static int txn__gc(lua_State *L)
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
