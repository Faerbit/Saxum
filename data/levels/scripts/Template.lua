local global = require( "global" )
if(global.triggeredSCRIPTNAME == nil) then
    global.triggeredSCRIPTNAME = false
end
function trigger(objectToChange)
    if(global.triggeredSCRIPTNAME == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        --enter the scripts code here and replace all occurences of SCRIPTNAME with the name of the script.
        
        global.triggeredSCRIPTNAME = true
        print("SCRIPTNAME")
    end
end
