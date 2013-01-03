local m = require('marshal')
local bd = require('bdb')

local e = m.encode({a=1, b='ma kota', c=true})
local e2 = m.decode(e)
print(e)
local  x= m.encode("ala ma kota")
print(x)
print(m.decode(x))
x = m.encode(323)
print(x)
print(m.decode(x))


function saveDoc(collection, doc)
    local md = m.encode(doc)
end


saveDoc({_id="abc/1", name="Kotek", age=38})

