local array = require "array"

a = array.new(20)
for i = 1, 20 do
    a[i] =  i % 2 == 0
end
print(a[10])
print(a[11])
print(#a)
a[4] = false
print(a)


 -- Exercise 31.3
local a1 = array.new(4)
a1[1], a1[2], a1[3] = true, true, true
local a2 = array.new(6)
a2[1], a2[2], a2[5], a2[6] = true, true, true, true
local a3 = a1 + a2
print("a1 + a2 = ", a3)
local a4 = a1 * a2
print("a1 * a2 = ", a4)