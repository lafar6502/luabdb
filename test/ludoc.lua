module(..., package.seeall);

local Collection = {
    
}

function Collection:getById(id)
    print("GetById:"..id)
    return id
end

function Collection:new(cfg) 
    local o = {}
    print("o")
    print(o)
    print("self")
    print(self)
    print("Collection")
    print(Collection)
    setmetatable(o, self)
    return o
end





LDB = {
    _config = {},
    __index = function(w, k)
        print("index: "..k)
        print(w)
        local v = rawget(w, k)
        if v ~= nil then
            print("found:"..k)
            return v
        end
        return LDB[k]
    end,
    magic = "alamakota",
    new = function(self, o)
        o = o or {} -- create table if user does not provide one
        setmetatable(o, self)
        print("magic: "..self.magic)
        print(o)
        return o
    end
}

function LDB:getCollection(name) 
    local c = Collection:new(name)
    print("getCollection "..name)
    print(c)
    return c
end
