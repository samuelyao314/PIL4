local mylib = require "mylib"

print("######  sin ########")
print(mylib.sin(3))

print("\n######  dir ########")
local res = mylib.dir('.')
for _, v in ipairs(res) do
    print(v)
end


-- Exercise 29.1
print("\n######  summation ########")
local summation = mylib.summation
print(summation()) --> 0
print(summation(2.3, 5.4)) --> 7.7
print(summation(2.3, 5.4, -34)) --> -26.3
--print(summation(2.3, 5.4, {}))

-- Exercise 29.2
print("\n######  pack ########")
local t = mylib.pack(3, 4, nil, 5)
--local t = table.pack(3, 4, nil, 5)
for k, v in pairs(t) do
    print(k, v)
end

-- Exercise 29.3
print("\n#### reverse #########")
local reverse = mylib.reverse
print(reverse(1, "hello", 20))
print(reverse(1, 2, 3, 4))


-- Exercise 29.4
print("\n#### foreach #########")
local foreach = mylib.foreach
foreach({x = 10, y = 20}, print)

-- Exercise 29.5
print("\n#### foreach_2 #########")
local foreach = mylib.foreach_2
local myprint = function (name, value) 
    coroutine.yield()
    print(name, value)
end
local co = coroutine.create(function () foreach({x = 10, y = 20}, myprint) end)
coroutine.resume(co)
coroutine.resume(co)
coroutine.resume(co)
coroutine.resume(co)


print("\n#### map #########")
local a = {1, 2, 3, 4}
mylib.map(a, function (x) return x*x end)
for i, v in ipairs(a) do
    print(i, v)
end
print()

print("\n#### split #########")
local t = mylib.split("hi:ho:there", ":")
for i, v in ipairs(t) do
    print(i, v)
end
print()

print("\n#### upper #########")
print(mylib.upper("hello"))

print("\n#### concat #########")
print(mylib.concat({"hello ", "world"}))


print("\n###### new counter ##########")
local newCounter = mylib.newCounter
local c1 = newCounter()
c1(); c1()
local c2 = newCounter()
print("c1", c1())
print("c2", c2())


-- Exercise 30.1
print("\n##########  filter #############")
local t = mylib.filter({1, 3, 20, -4, 5}, function (x) return x < 5 end)
for i, v in ipairs(t) do
    print(i, v)
end


-- Exercise 30.2
print("\n########### .split_2  ##############")
local t = mylib.split_2("h\0i:h\0o:there", ":")
for i, v in ipairs(t) do
    print(i, v, #v)
end
print()


-- Exercise 30.1
print("\n############### transliterate ##########")
local s = mylib.transliterate("helloworld", {h="a", l="b"})
print(s)