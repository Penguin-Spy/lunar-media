print("hello from lua!\nWhat is your name?")

local name = io.read()

print("hi " .. name .. "!")


dofile("startup.lua")

-- sleep(5)

print "ok bye now"
