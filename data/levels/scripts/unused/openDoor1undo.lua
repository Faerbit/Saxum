local global = require( "global" )
if(global.triggeredOpenDoor1undo == nil) then
    global.triggeredOpenDoor1undo = true
end
function trigger(objectToChange)
    if(global.triggeredOpenDoor1undo == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        if(global.triggeredOpenDoor1) then
            global.openDoorCounter = global.openDoorCounter - 1
            global.triggeredOpenDoor1 = false
        end
        
        global.triggeredOpenDoor1undo = true
        print("openDoor1undo")
    end
end
