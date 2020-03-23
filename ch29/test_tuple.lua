local tuple = require "tuple"

local t = tuple.new(10, "hi", {}, 3)
print(t(1))
print(t(2))
print(t())