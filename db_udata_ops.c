#include "luabdb_private.h"
#include <db.h>
#include <lauxlib.h>
#include <lualib.h>

///retrieves _db member from the 'db' table. Does not modify the stack.
static DB* retrieve_db(lua_State *L, int index)
{
    luaL_checktype(L, index, LUA_TTABLE);
    lua_getfield(L, index, "_db");
    dbgprint("retr_db 1\n");
    DB** pdb = luabdb_todb(L, -1);
    dbgprint("retr_db 2\n");
    lua_pop(L, 1);
    return *pdb;
}

// Nice, we have 'put', but what about associated index databases???
// It would be perfect if the BDB assoc index callback could be forwarded to a lua function
//
static int dbt_op_put(lua_State *L)
{
    DB *db = retrieve_db(L, 1);

    DB_TXN *txn = NULL;
    DBT key;
    DBT value;
    size_t key_len;
    size_t value_len;
    u_int32_t flags = 0;
    int status;

    memset(&key, 0, sizeof(DBT));
    memset(&value, 0, sizeof(DBT));

    
    key.data = (void *) luaL_checklstring(L, 2, &key_len);
    value.data = (void *) luaL_checklstring(L, 3, &value_len);
    key.size = key_len;
    value.size = value_len;
    if (lua_gettop(L) >= 4) txn = luabdb_totxn(L, 4);
    if (lua_gettop(L) >= 5) flags = luabdb_getflags(L, 5);
    //dbgprint("txn: 0x%x, flags: 0x%x, key size: %d, value size: %d\n", txn, flags, key_len, value_len);
    status = db->put(db, txn, &key, &value, flags);
    //dbgprint("put done: 0x%x\n", status);
    handle_dbexception(L, status);

    return 0;
}


static int dbt_op_get(lua_State *L)
{
    DB *db = retrieve_db(L, 1);

    DB_TXN *txn = NULL;
    DBT key;
    DBT value;
    size_t key_len;
    size_t value_len;
    u_int32_t flags = 0;
    int status;

    memset(&key, 0, sizeof(DBT));
    memset(&value, 0, sizeof(DBT));

    key.data = (void *) luaL_checklstring(L, 2, &key_len);
    key.size = key_len;
    value.flags = DB_DBT_MALLOC;
    
    if (lua_gettop(L) >= 3) txn = luabdb_totxn(L, 3);
    if (lua_gettop(L) >= 4) flags = luabdb_getflags(L, 4);
    //dbgprint("get txn: 0x%x, flags: 0x%x, key size: %d, TOP: %d\n", txn, flags, key_len, lua_gettop(L));
    status = db->get(db, txn, &key, &value, flags);
    //dbgprint("get done: 0x%x\n", status);
    if (status == 0)
    {
        lua_pushlstring(L, value.data, value.size);
        free(value.data);
        return 1;
    }
    else if (status == DB_NOTFOUND)
    {
        lua_pushnil(L);
        return 1;
    }
    else handle_dbexception(L, status);
}

///get_dbname returns 2 values: db file name and database name
static int dbt_op_get_dbname(lua_State *L)
{
    DB *db;
    const char* pfile = NULL;
    const char* pname = NULL;
    db = luabdb_todb(L, 1);
    int status;
    status = db->get_dbname(db, &pfile, &pname);
    handle_error(status);
    if (pfile != NULL)
    {
        lua_pushstring(L, pfile);
        free(pfile);
    }
    else
    {
        lua_pushnil(L);
    }
    
    if (pname != NULL)
    {
        lua_pushstring(L, pname);
        free(pname);
    }
    else
    {
        lua_pushnil(L);
    }
    return 2;
}

static int dbt_op_testMapping(lua_State *L)
{
    DB *db = retrieve_db(L, 1);
    if (db == NULL) {
        lua_pushnil(L);
        return 1;
    }
    get_lua_object_from_registry(L, db);
    return 1;
}



#define _(name) { #name, dbt_op_##name }
#define u_(name) { #name, luabdb_unimplemented }

static luaL_Reg dbt_funcs[] = {
    _(put),
    _(get),
    _(testMapping),
    u_(close),
    u_(associate),
    u_(associate_foreign),
    u_(compact),
    u_(del),
    u_(err),
    u_(exists),
    u_(fd),
    u_(get_byteswapped),
    u_(get_dbname),
    u_(get_multiple),
    u_(get_open_flags),
    u_(get_type),
    u_(join),
    u_(key_range),
    u_(remove),
    u_(rename),
    u_(set_priority),
    u_(get_priority),
    u_(stat),
    u_(stat_print),
    u_(sync),
    u_(truncate),
    u_(upgrade),
    u_(verify),
    u_(cursor),
    u_(set_alloc),
    u_(set_cachesize),
    u_(get_cachesize),
    u_(set_create_dir),
    u_(get_create_dir),
    u_(set_dup_compare),
    u_(set_encrypt),
    u_(get_encrypt_flags),
    u_(set_errcall),
    u_(set_errfile),
    u_(get_errfile),
    u_(set_errpfx),
    u_(get_errpfx),
    u_(set_feedback),
    u_(set_flags),
    u_(get_flags),
    u_(set_lorder),
    u_(get_lorder),
    u_(set_msgcall),
    u_(set_msgfile),
    u_(get_msgfile),
    u_(set_pagesize),
    u_(get_pagesize),
    u_(set_partition),
    u_(set_partition_dirs),
    u_(get_partition_dirs),
    u_(set_append_recno),
    u_(set_bt_compare),
    u_(set_bt_compress),
    u_(set_bt_minkey),
    u_(get_bt_minkey),
    u_(set_bt_prefix),
    u_(set_re_delim),
    u_(get_re_delim),
    u_(set_re_len),
    u_(get_re_len),
    u_(set_re_pad),
    u_(get_re_pad),
    u_(set_re_source),
    u_(get_re_source),
    u_(set_h_compare),
    u_(set_h_ffactor),
    u_(get_h_ffactor),
    u_(set_h_hash),
    u_(set_h_nelem),
    u_(get_h_nelem),
    u_(set_q_extentsize),
    u_(get_q_extentsize),
    u_(get_env),
    u_(get_mpf),
    u_(get_transactional),
    { NULL, NULL }
};
#undef _
#undef u_

//initialize DBT meta 
int init_dbt_ops(lua_State *L)
{
	dbgprint("init_dbt_ops\n");
    luaL_getmetatable(L, LUABDB_DBT);
    lua_getfield(L, -1, "__index");
    luaL_register(L, NULL, dbt_funcs);
    return 0;
}
