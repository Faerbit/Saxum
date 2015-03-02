local global = require( "global" )
if(global.triggeredOpenThirdDoor == nil) then
    global.triggeredOpenThirdDoor = false
end
function trigger(objectToChange)
    if(global.triggeredOpenThirdDoor == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        local strength = 100
        local xPos = -172.5
        local yPos = 33
        local zPos = 58.5
        level:moveObject(objectToChange, strength, xPos, yPos, zPos)
        
        global.triggeredOpenThirdDoorUndo = false
        
        global.triggeredOpenThirdDoor = true
        print("openThirdDoor")
    end
end
