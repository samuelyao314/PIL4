local lib = require "trans"

print("gettrans", lib.gettrans())
local t =  {h="a", l="b"}
print("t", t)
lib.settrans(t)
local t2 = lib.gettrans()
print("t2", t2)
for k, v in pairs(t2) do
    print(k, v)
end
local s = lib.transliterate("helloworld")
print("transliterate", s)