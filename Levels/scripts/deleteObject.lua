if(triggeredDeleteObject == nil) then
    triggeredDeleteObject = false
end
function trigger(objectToChange)
    if(triggeredDeleteObject == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        if(not physics) then
            print("No physics found in Lua!")
            return
        end
        level:deleteObject(objectToChange)
        triggeredDeleteObject = true
        print("deleteObject")
    end
end
