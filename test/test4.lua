local bdb = require('bdb')


local db,err = bdb.open({env=nil, file='test4.db', flags={bdb.flags.DB_CREATE}, type=bdb.flags.DB_BTREE})
assert(db ~= nil, err)

print("type of db: "..type(db));
db:put("ala", "ma kota")
print("now get ala: "..db:get("ala"))

local db2 = db:testMapping()
print("type of db2: "..type(db2)..", and of _db: "..type(db2._db))


db._db:close()
print("clozd")