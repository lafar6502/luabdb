local bdb = require('bdb')
local ffi = require('ffi')

local db,err = bdb.open({env=nil, file='test4.db', flags={bdb.flags.DB_CREATE}, type=bdb.flags.DB_BTREE})
assert(db ~= nil, err)

local dbi = bdb.open({env=nil, file='test4_i1.db', flags={bdb.flags.DB_CREATE}, type=bdb.flags.DB_BTREE})

print("type of db: "..type(db));

local cnt = 0
db:associate(dbi, function(k,v,sv) 
    print("in callback k="..k..", v="..v);
    cnt = cnt + 1
    return ""..k
end)


db:put("ala", "ma kota")
print("now get ala: "..db:get("ala"))
db:put("ze", "baa")
local db2 = db:testMapping()
print("type of db2: "..type(db2)..", and of _db: "..type(db2._db))




local mt = getmetatable(db2).__index
print("mt is "..type(mt))
mt.fun = function(self, y)
    print("a to y."..y)
end

db:fun("kotek")

db:put("ziutek", "ma cos innego")

mt.put2 = function(self, k, v)
    
    self:put(k, v)
end

mt.serialize = function(v)

end

mt.deserialize = function(v)

end
