local global = require("global")
if(global.triggeredFristCheckpoint == nil) then
    global.triggeredFristCheckpoint = false
end

function trigger(objectToChange, printDebug)
    if(global.triggeredFristCheckpoint == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        global.triggeredFristCheckpoint = true
        if(printDebug) then
            print("firstCheckpoint")
        end
    end
end    
