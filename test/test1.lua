bdb = require('bdb')
if bdb == nill then 
    print('bdb open failed')
end

local db,err = bdb.open({file='testdb1.db', flags={bdb.flags.DB_CREATE}, type=bdb.flags.DB_BTREE})
if db == nil then
    print('error: '..err)
end

db:put('key', 'value')
print('key:'..db:get('key'))
db:close()