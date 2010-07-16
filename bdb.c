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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabdb_private.h"

static int luabdb_open(lua_State *L)
{
    return 0;
}

static luaL_Reg luabdb_functions[] = {
    { "open", luabdb_open },
    { NULL, NULL }
};

static void init_metatables(lua_State *L)
{
}

int luabdb_unimplemented(lua_State *L)
{
    lua_Debug ar;
    lua_getstack(L, 0, &ar);
    lua_getinfo(L, "n", &ar);

    return luaL_error(L, "Function '%s' is not implemented!", ar.name);
}

int luaopen_bdb(lua_State *L)
{
    const char *libraryName = luaL_checkstring(L, 1);
    lua_newtable(L);
    luaL_register(L, libraryName, luabdb_functions);

    init_metatables(L);
    init_db_ops(L);
    init_db_config(L);
    init_cursor_ops(L);
    init_env_ops(L);
    init_env_config(L);
    init_lock_ops(L);
    init_lock_config(L);
    init_log_ops(L);
    init_log_config(L);
    init_memory_pool_ops(L);
    init_memory_pool_config(L);
    init_mutex_ops(L);
    init_mutex_config(L);
    init_replication_ops(L);
    init_replication_config(L);
    init_sequence_ops(L);
    init_sequence_config(L);
    init_txn_ops(L);
    init_txn_config(L);

    return 1;
}