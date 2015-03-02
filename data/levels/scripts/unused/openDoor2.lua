local global = require( "global" )
if(global.triggeredOpenDoor2 == nil) then
    global.triggeredOpenDoor2 = false
end
function trigger(objectToChange)
    if(global.triggeredOpenDoor2 == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        if(not global.openDoorCounter) then
            global.openDoorCounter = 0
        end
        global.openDoorCounter = global.openDoorCounter + 1
        if (global.openDoorCounter == 3) then
            local strength = 1
            local xPos = 0
            local yPos = 0
            local zPos = 0
            level:moveObject(objectToChange, strength, xPos, yPos, zPos)
            print("Opening the door")
        end
        global.triggeredOpenDoor2undo = false
        
        global.triggeredOpenDoor2 = true
        print("openDoor2")
    end
end
