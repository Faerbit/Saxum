if(triggeredOpenDoor1undo == nil) then
    triggeredOpenDoor1undo = false
end
function trigger(objectToChange)
    if(triggeredOpenDoor1undo == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        if(triggeredOpenDoor1) then
            openDoorCounter = openDoorCounter - 1
            triggeredOpenDoor1 = false
        end
        
        triggeredOpenDoor1undo = true
        print("openDoor1undo")
    end
end
