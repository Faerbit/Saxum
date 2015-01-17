if(triggeredOpenDoor2undo == nil) then
    triggeredOpenDoor2undo = false
end
function trigger(objectToChange)
    if(triggeredOpenDoor2undo == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        if(triggeredOpenDoor2) then
            openDoorCounter = openDoorCounter - 1
            triggeredOpenDoor2 = false
        end
        
        triggeredOpenDoor2undo = true
        print("openDoor2undo")
    end
end
