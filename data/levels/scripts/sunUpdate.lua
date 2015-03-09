local global = require( "global" )

function trigger(objectToChange)
    if(global.triggeredSunStart) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        local maxTimeDiff = 30
        local timeDiff = os.clock()- global.sunStartTime
        if(timeDiff > maxTimeDiff)then
            timeDiff = maxTimeDiff
        end
        timeDiff = timeDiff/maxTimeDiff
        local x = 10
        local y = -18 + math.sqrt(timeDiff) * 29
        local z = -8 + timeDiff * 15
        level:setSunDirection(x,y,z)
    end
end


        --local x = 0.7 + 0.3 * timeDiff
        --local y = math.sqrt(timeDiff) * 2.2 - 1.5
        --local z = -0.7 - 0.7 * timeDiff
        --start 1,-6,-2
        --ziel 10,10,10
