local global = require( "global" )
if(global.triggeredResetPlayer == nil) then
    global.triggeredResetPlayer = false
end

function trigger(objectToChange, printDebug)
    if(global.triggeredResetPlayer == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
       
        if(global.triggeredFristCheckpoint == true) then
            if(global.triggeredOpenFirstDoor == true) then
                if(global.openedSecondDoor == true) then
                    if(global.triggeredOpenThirdDoor == true) then
                        level:movePlayer(-169.5,21.5,58.5)
                    else
                        level:movePlayer(-78.5,21.75,4.5)
                    end
                else
                    level:movePlayer(17.5,21.0,87.0)
                end
            else
                level:movePlayer(18.0, 23.0, 145.0)
            end
        else
            level:resetPlayer()
        end
       
        if(printDebug) then
            print("reset player")
        end
    end
end
