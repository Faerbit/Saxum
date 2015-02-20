local global = require( "global" )
if(global.triggeredOpenDoor2undo == nil) then
    global.triggeredOpenDoor2undo = true
end
function trigger(objectToChange)
    if(global.triggeredOpenDoor2undo == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        if(global.triggeredOpenDoor2) then
            global.openDoorCounter = global.openDoorCounter - 1
            global.triggeredOpenDoor2 = false
        end
        
        global.triggeredOpenDoor2undo = true
        print("openDoor2undo")
    end
end
