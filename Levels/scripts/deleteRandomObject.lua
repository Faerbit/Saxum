function trigger(objectToChange)
    if(not level) then
        print("No level found in Lua!")
        return
    end
    if(not physics) then
        print("No physics found in Lua!")
        return
    end
    a = level:getObjectCount()
    rand = math.random(0, a - 1)
    level:deleteObject(rand)
    print("Triggered from Lua!")
end
