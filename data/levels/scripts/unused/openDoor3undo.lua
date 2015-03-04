local global = require( "global" )
if(global.triggeredOpenDoor3undo == nil) then
    global.triggeredOpenDoor3undo = false
end
function trigger(objectToChange)
    if(global.triggeredOpenDoor3undo == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        if(global.triggeredOpenDoor3) then
            global.openDoorCounter = global.openDoorCounter - 1
            global.triggeredOpenDoor3 = false
        end
        
        global.triggeredOpenDoor3undo = true
        print("openDoor3undo")
    end
end
