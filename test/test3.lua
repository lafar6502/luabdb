local m = require('marshal')
local bd = require('bdb')
 
local db,err = bdb.open({env=nil, file='test3.db', flags={bdb.flags.DB_CREATE}, type=bdb.flags.DB_BTREE})
assert(db ~= nil, err)

        

function saveDoc(db, doc)
    local md = m.encode(doc)
    assert(doc._id ~= nil)
    db:put(doc._id, md)
end

function loadDoc(db, key)
    local md = db:get(key)
    if md == nil then 
        return nil
    end
    return m.decode(md)
end

local d1 = {_id="abc/1", name="Kotek", age=38}

saveDoc(db, d1)

local d2 = loadDoc(db, d1._id)
print(d2)

local d3 = loadDoc(db, 'abc/1')
print(d3)

for i = 1,1000000 do
    saveDoc(db, {_id='abc/'..i, name='Doc_'..i, age=2 * i})
end

local fn, dn = db:get_dbname()

print('file name:'..(fn or "--none"))
print('db name:'..(dn or "--none"))


db:get(key, value, {
    flags = 3234,
    txn = nil
})

