if(triggereOpenFirstDoorUndo == nil) then
    triggereOpenFirstDoorUndo = false
end
function trigger(objectToChange)
    if(triggereOpenFirstDoorUndo == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        local strength = 100
        local xPos = 81.5
        local yPos = 25
        local zPos = 17.5
        level:moveObject(objectToChange, strength, xPos, yPos, zPos)
        
        triggeredOpenFirstDoor = false
        
        triggereOpenFirstDoorUndo = true
        print("openFirstDoorUndo")
    end
end
