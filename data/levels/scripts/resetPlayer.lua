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
        
        if(global.triggeredOpenFirstDoor == true) then
            if(global.openedSecondDoor == true) then
                if(global.triggeredOpenThirdDoor == true) then
                    level:movePlayer(-169.5,22.5,58.5)
                else
                    level:movePlayer(-78.5,22.5,4.5)
                end
            else
                level:movePlayer(17.5,22.5,87.0)
            end
        else
            level:resetPlayer()
        end
        
        print("reset player")
    end
end
