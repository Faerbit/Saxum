local global = require( "global" )
if(global.triggeredSCRIPTNAME == nil) then
    global.triggeredSCRIPTNAME = false
end
if(global.triggeredSCRIPTNAMEUndo == nil) then
    global.triggeredSCRIPTNAMEUndo = false
end

function trigger(objectToChange, printDebug)
    if(global.triggeredSCRIPTNAME == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        --enter the scripts code here and replace all occurences of SCRIPTNAME with the name of the script.
        
        global.triggeredSCRIPTNAME = true
        if(printDebug) then
            print("SCRIPTNAME")
        end
    end
end

function triggerUndo(objectToChange, printDebug)
    if(global.triggeredSCRIPTNAMEUndo == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        --enter the scripts undo code here
        
        global.triggeredSCRIPTNAMEUndo = true
        if(printDebug) then
            print("SCRIPTNAMEUndo")
        end
    end
end
