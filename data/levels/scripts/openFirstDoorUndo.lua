local global = require( "global" )
if(global.triggeredOpenFirstDoorUndo == nil) then
    global.triggeredOpenFirstDoorUndo = true
end
function trigger(objectToChange)
    if(global.triggeredOpenFirstDoorUndo == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        local strength = 100
        local xPos = 17.5
        local yPos = 25
        local zPos = 81.5
        level:moveObject(objectToChange, strength, xPos, yPos, zPos)
        
        global.triggeredOpenFirstDoor = false
        
        global.triggeredOpenFirstDoorUndo = true
        print("openFirstDoorUndo")
    end
end
