local global = require( "global" )
if(global.triggeredResetPlayer == nil) then
    global.triggeredResetPlayer = false
end
function trigger(objectToChange)
    if(global.triggeredResetPlayer == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        level:resetPlayer()
        
        --global.triggeredResetPlayer = true
        print("reset player")
    end
end
