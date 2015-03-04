local global = require( "global" )
if(global.triggeredDeleteObject == nil) then
    global.triggeredDeleteObject = false
end
function trigger(objectToChange)
    if(global.triggeredDeleteObject == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        level:deleteObject(objectToChange)
        global.triggeredDeleteObject = true
        print("deleteObject")
    end
end
