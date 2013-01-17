
local bdb = require('bdb')



local env = bdb.openenv({
    home = "txndb",
    flags = {bdb.flags.DB_INIT_TXN, bdb.flags.DB_INIT_LOG, bdb.flags.DB_INIT_LOCK, bdb.flags.DB_CREATE}
})

if env == nil then
    print("failed to open env")
end

local tx = env:txn_begin({})

print("tx:"..tx:getId())
tx:setName('ala ma kota');

print("tx name: "..tx:getName())
tx:commit(bdb.flags.DB_TXN_NOSYNC)


tx = nil
