if(triggeredSCRIPTNAME == nil) then
    triggeredSCRIPTNAME = false
end
function trigger(objectToChange)
    if(triggeredSCRIPTNAME == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        --enter the scripts code here and replace all occurences of SCRIPTNAME with the name of the script.
        
        triggeredSCRIPTNAME = true
        print("SCRIPTNAME")
    end
end
