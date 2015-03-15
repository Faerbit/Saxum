local global = require( "global" )
if(global.triggeredSunStart == nil) then
    global.triggeredSunStart = false
end

function trigger(objectToChange, printDebug)
    if(global.triggeredSunStart == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        local time = os.clock()
        global.sunStartTime = time
        global.triggeredSunStart = true
        level:activateEndgame()
        if(printDebug) then
            print("sunStart")
        end
    end
end
