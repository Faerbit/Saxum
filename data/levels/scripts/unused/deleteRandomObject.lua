local global = require( "global" )
if(global.triggeredDeleteRandomObject == nil) then
    global.triggeredDeleteRandomObject = false
end
function trigger(objectToChange)
    if(global.triggeredDeleteRandomObject == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        a = level:getObjectCount()
        rand = math.random(0, a - 1)
        level:deleteObject(rand)
        global.triggeredDeleteRandomObject = true
        print("deleteRandomObject")
    end
end


