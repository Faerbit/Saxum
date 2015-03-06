local global = require( "global" )

function trigger(objectToChange)
    if(global.triggeredSunStart) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        local maxTimeDiff = 20
        local timeDiff = os.clock()- global.sunStartTime
        if(timeDiff > maxTimeDiff)then
            timeDiff = maxTimeDiff
        end
        timeDiff = timeDiff/maxTimeDiff
        local x = 1 - 0.3 * timeDiff
        local y = math.sqrt(timeDiff) * 1.0 - 0.1
        local z = 0.7 * timeDiff
        level:setSunDirection(x,y,z)
    end
end
