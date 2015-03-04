local global = require( "global" )
if(global.triggeredOpenFirstDoor == nil) then
    global.triggeredOpenFirstDoor = false
end
function trigger(objectToChange)
    if(global.triggeredOpenFirstDoor == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        local strength = 100
        local xPos = 17.5
        local yPos = 33
        local zPos = 81.5
        level:moveObject(objectToChange, strength, xPos, yPos, zPos)
        
        global.triggeredOpenFirstDoorUndo = false
        
        global.triggeredOpenFirstDoor = true
        print("openFirstDoor")
    end
end
