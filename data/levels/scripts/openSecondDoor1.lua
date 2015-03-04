local global = require( "global" )
if(global.triggeredOpenSecondDoor1 == nil) then
    global.triggeredOpenSecondDoor1 = false
end
if(global.triggeredOpenSecondDoor1Undo == nil) then
    global.triggeredOpenSecondDoor1Undo = true
end

function trigger(objectToChange)
    if(global.triggeredOpenSecondDoor1 == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        if(global.openSecondDoorCounter == nil) then
            global.openSecondDoorCounter = 0
        end
        if(global.openSecondDoorCounter == 0) then
            global.openSecondDoorCounter = global.openSecondDoorCounter + 1
        end
        global.triggeredOpenSecondDoor1 = true
        
        if(global.triggeredOpenSecondDoor1 == true and global.triggeredOpenSecondDoor2 == true and global.triggeredOpenSecondDoor3 == true and global.triggeredOpenSecondDoor4 == true) then
            global.triggeredOpenSecondDoor1Undo = false
        end
        
        print("openSecondDoor1")
    end
end

function triggerUndo(objectToChange)
    if(global.triggeredOpenSecondDoor1Undo == false) then
        if(not level) then
            print("No level found in Lua!")
            return
        end
        
        global.openSecondDoorCounter = 0
        global.triggeredOpenSecondDoor1 = false
        global.triggeredOpenSecondDoor2 = false
        global.triggeredOpenSecondDoor3 = false
        global.triggeredOpenSecondDoor4 = false
        
        global.triggeredOpenSecondDoor1Undo = true
        print("openSecondDoor1Undo")
    end
end
