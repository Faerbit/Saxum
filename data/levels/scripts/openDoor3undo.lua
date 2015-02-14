if(triggeredOpenDoor3undo == nil) then
    triggeredOpenDoor3undo = false
end
function trigger(objectToChange)
    if(triggeredOpenDoor3undo == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        if(triggeredOpenDoor3) then
            openDoorCounter = openDoorCounter - 1
            triggeredOpenDoor3 = false
        end
        
        triggeredOpenDoor3undo = true
        print("openDoor3undo")
    end
end
