if(triggereOpenFirstDoor == nil) then
    triggereOpenFirstDoor = false
end
function trigger(objectToChange)
    if(triggereOpenFirstDoor == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        local strength = 100
        local xPos = 81.5
        local yPos = 30
        local zPos = 17.5
        level:moveObject(objectToChange, strength, xPos, yPos, zPos)
        
        triggeredOpenFirstDoorUndo = false
        
        triggereOpenFirstDoor = true
        print("openFirstDoor")
    end
end
