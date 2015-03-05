local global = require( "global" )

function trigger(objectToChange)
    if(global.triggeredSunStart) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        local maxTimeDiff = 5
        local timeDiff = os.clock()- global.sunStartTime
        if(timeDiff > maxTimeDiff)then
            timeDiff = maxTimeDiff
        end
        timeDiff = timeDiff/maxTimeDiff
        local x = 1 - 0.3 * timeDiff
        local y = 0.7 * timeDiff
        local z = timeDiff^0.5 * 0.6 -0.1
        level:setSunAngle(x,y,z)
    end
end
