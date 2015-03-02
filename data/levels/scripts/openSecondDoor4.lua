local global = require( "global" )
if(global.triggeredOpenSecondDoor4 == nil) then
    global.triggeredOpenSecondDoor4 = false
end
function trigger(objectToChange)
    if(global.triggeredOpenSecondDoor4 == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        if(global.openSecondDoorCounter == nil) then
            global.openSecondDoorCounter = 0
        end
        if(global.openSecondDoorCounter == 3) then
           global.openSecondDoorCounter = global.openSecondDoorCounter + 1
        end
        global.triggeredOpenSecondDoor4 = true
        
        if(global.triggeredOpenSecondDoor1 == true and global.triggeredOpenSecondDoor2 == true and global.triggeredOpenSecondDoor3 == true and global.triggeredOpenSecondDoor4 == true) then
            if(global.openSecondDoorCounter == 4) then
                local strength = 100
                local xPos = -81.5
                local yPos = 33
                local zPos = 4.5
                level:moveObject(objectToChange, strength, xPos, yPos, zPos)
            else
                global.openSecondDoorCounter = 0
                global.triggeredOpenSecondDoor1 = false
                global.triggeredOpenSecondDoor2 = false
                global.triggeredOpenSecondDoor3 = false
                global.triggeredOpenSecondDoor4 = false
            end
        end
        
        print("openSecondDoor4")
    end
end
