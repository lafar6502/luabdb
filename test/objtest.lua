

MyClass = {
    x=1
}
    
function MyClass:new()
    local o = {}
    setmetatable(o, {__index = self})
    for k,v in pairs(self) do
        print("k:"..k..",v:"..type(v))
    end
    return o
end

function MyClass:test()
    print("testing 123")
    print(self)
    print("X:"..self.x)
end


local mc = MyClass:new()
mc:test()

SecondClass = {
}

function SecondClass:new()
    local o = {}
    local s = self
    setmetatable(o, {__index = function(t,k)
        print("Index: "..k)
        if s[k] ~= nil then
            print("found "..k)
            return s[k]
        end
        return nil
    end})
    return o
end


local sc = SecondClass:new()
sc.testx()
