local m = require('marshal')
local bdb = require('bdb')

g_databases = {}

function initDb(config)
    assert(config.name ~= nil, "name missing")
    if g_databases[config.name] ~= nil then 
        return g_databases[config.name]
    end
    local dbr = {
        name = config.name,
        path = './db/'..config.name,
        collections = {},
        getCollections = function() {
            return 
        }
    }
    local env, err = bdb.openenv({home=dbr.path, flags={bdb.flags.DB_CREATE, bdb.flags.DB_INIT_MPOOL}, mode=0})
    assert(env ~= nil, err)
    for i,c in ipairs(config.collections) do
        print('col:'..c.name)
        local colr = {
            name= c.name,
            file = 'col_'..c.name..'.db'
        }
        local db,err = bdb.open({env=env, file=colr.file, flags={bdb.flags.DB_CREATE}, type=bdb.flags.DB_BTREE})
        assert(db ~= nil, err)
        print('opened collection '..c.name)
        dbr.collections[#dbr.collections + 1] = colr
    end
    dbr.env = env
    g_databases[config.name] = dbr
    return dbr
end




initDb({
    name="db1",
    collections={
        {
            name="c1",
            indexes= {
                {name='name', field= 'name'},
                {   
                    name= 'text', 
                    provider= function(x) 
                        return {x.name, x.firstName, x.lastName, x.login}
                    end
                }
            }
        },
        {name="c2"}
    }
})


