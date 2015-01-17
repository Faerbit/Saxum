if(triggeredDeleteRandomObject == nil) then
    triggeredDeleteRandomObject = false
end
function trigger(objectToChange)
    if(triggeredDeleteRandomObject == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        a = level:getObjectCount()
        rand = math.random(0, a - 1)
        level:deleteObject(rand)
        triggeredDeleteRandomObject = true
        print("deleteRandomObject")
    end
end
