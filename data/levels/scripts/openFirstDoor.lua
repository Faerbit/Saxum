--I have no idea why this hack from the internet works, but it does...
package.path = debug.getinfo(1,"S").source:match[[^@?(.*[\/])[^\/]-$]] .."?.lua;".. package.path
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
        local xPos = 81.5
        local yPos = 35
        local zPos = 17.5
        level:moveObject(objectToChange, strength, xPos, yPos, zPos)
        
        global.triggeredOpenFirstDoorUndo = false
        
        global.triggeredOpenFirstDoor = true
        print("openFirstDoor")
    end
end
