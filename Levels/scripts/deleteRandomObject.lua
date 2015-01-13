function trigger(objectToChange)
    print("Hello from Lua!")
    --if(not this_level) then
        print("No level found!")
        return
    --end
    a = this_level:getObjectCount()
    rand = math.random(0, a - 1)
    this_level:deleteObject(rand)
    print("Triggered from Lua!")
end
