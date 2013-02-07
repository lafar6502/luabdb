local ffi = require("ffi")

function string.tohex(str)
    return (str:gsub('.', function (c)
        return string.format('%02X', string.byte(c))
    end))
end

ffi.cdef[[
typedef struct { 
    char a[4];
    uint32_t origin_msisdn;
    uint32_t dest_msisdn;
    uint32_t timestamp;
    uint32_t duration_sec;
    uint32_t lac;
    uint32_t ci;
    uint64_t imei;
} cdr;
]]

local r = ffi.new("cdr")
print("size: "..ffi.sizeof("cdr"))
print("1."..ffi.string(r, ffi.sizeof("cdr")):tohex())

r.a = 'xyZ'
r.origin_msisdn = 123123123;
r.dest_msisdn = 602415455;
r.imei = 1

print("2."..ffi.string(r, ffi.sizeof("cdr")):tohex())

local ser = ffi.string(r, ffi.sizeof("cdr"))

local deser = ffi.new("cdr")
ffi.copy(deser,ser, ffi.sizeof("cdr"))

print("deser:"..tostring(deser.imei))