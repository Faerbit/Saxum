local global = require( "global" )
if(global.triggeredOpenFirstDoor == nil) then
    global.triggeredOpenFirstDoor = false
end
if(global.triggeredOpenFirstDoorUndo == nil) then
    global.triggeredOpenFirstDoorUndo = true
end

function trigger(objectToChange, printDebug)
    if(global.triggeredOpenFirstDoor == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        local strength = 100
        local xPos = 17.5
        local yPos = 33.0
        local zPos = 81.5
        level:moveObject(objectToChange, strength, xPos, yPos, zPos)
        
        global.triggeredOpenFirstDoorUndo = false
        
        global.triggeredOpenFirstDoor = true
        if (printDebug) then
            print("openFirstDoor")
        end
    end
end

function triggerUndo(objectToChange, printDebug)
    if(global.triggeredOpenFirstDoorUndo == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        local strength = 100
        local xPos = 17.5
        local yPos = 25.0
        local zPos = 81.5
        level:moveObject(objectToChange, strength, xPos, yPos, zPos)
        
        global.triggeredOpenFirstDoor = false
        
        global.triggeredOpenFirstDoorUndo = true
        if(printDebug) then
            print("openFirstDoorUndo")
        end
    end
end
