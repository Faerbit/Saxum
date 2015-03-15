local global = require( "global" )
if(global.triggeredResetBox == nil) then
    global.triggeredResetBox = false
end

function trigger(objectToChange, printDebug)
    if(global.triggeredResetBox == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        local x = -101.5
        local y = 22.5
        local z =8.5
        level:forceMove(x, y, z, objectToChange)
       
        if(printDebug) then
            print("reset box")
        end
    end
end
