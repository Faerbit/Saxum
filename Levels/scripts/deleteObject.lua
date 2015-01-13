function trigger(objectToChange)
    if(not level) then
        print("No level found in Lua!")
        return
    end
    if(not physics) then
        print("No physics found in Lua!")
        return
    end
    level:deleteObject(objectToChange)
end
