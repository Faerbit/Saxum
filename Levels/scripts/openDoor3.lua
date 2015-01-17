if(triggeredOpenDoor3 == nil) then
    triggeredOpenDoor3 = false
end
function trigger(objectToChange)
    if(triggeredOpenDoor3 == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        if(not openDoorCounter) then
            openDoorCounter = 0
        end
        openDoorCounter = openDoorCounter + 1
        if (openDoorCounter == 3) then
            local strength = 1
            local xPos = 0
            local yPos = 0
            local zPos = 0
            level:moveObject(objectToChange, strength, xPos, yPos, zPos)
            print("Opening the door")
        end
        triggeredOpenDoor3undo = false
        
        triggeredOpenDoor3 = true
        print("openDoor3")
    end
end
