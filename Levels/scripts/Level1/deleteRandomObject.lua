function trigger()
    print("Hello from Lua!")
    if(not Level) then
        print("No Level found!")
        return
    end
    --a = table.getn(Level.getObjects())
    --rand = math.random(0, table.getn(Level->getObjects()) - 1)
    --Level->getObjects()->erase(level->getObjects()->begin() + rand)
end
