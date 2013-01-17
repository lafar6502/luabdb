
local bdb = require('bdb')



local env = bdb.openenv({
    home = "txndb",
    flags = {bdb.flags.DB_INIT_TXN, bdb.flags.DB_INIT_LOG, bdb.flags.DB_INIT_LOCK, bdb.flags.DB_CREATE, bdb.flags.DB_INIT_MPOOL}
})

if env == nil then
    print("failed to open env")
end

local tx = env:txn_begin({})

print("tx:"..tx:getId())
tx:setName('ala ma kota');

local db = bdb.open({env = env, txn = tx,  file="trant1.db", flags = {bdb.flags.DB_CREATE}, type = bdb.flags.DB_BTREE})
print("tx name: "..tx:getName())
tx:commit()

print("Second TEST")
local tx2 = env:txn_begin({})
print("put data");
db:put("ala", "ma kota", tx2)
print("in tran get: "..db:get("ala"), tx2)
tx2:abort()
print("out tran get: "..db:get("ala"))




--for i = 0, 1000 do
--print("tran "..i)
--local t2 = env:txn_begin({})

--t2:commit()
--end

