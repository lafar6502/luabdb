ld = require("ludoc")

print("mg"..ld.LDB.magic)
local d = ld.LDB:new({})


local col = d:getCollection("Documents")

for k,v in pairs(col) do
    print("-----"..k.." = "..v)
end

local doc = col:getById("12345")

