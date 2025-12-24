local skynet = require "skynet"
local socket = require "skynet.socket"

skynet.start(function ()
    -- local listenfd = socket.listen("0.0.0.0", 8888)
    -- socket.start(listenfd, function(clientfd, addr)
    --     print("receive a client:", clientfd, addr)
    -- end)
    -- skynet.timeout(100, function()
    --     print("after 1s, do here")
    -- end)
    -- print("hello skynet")

    local slave = skynet.newservice("slave")
    local response = skynet.call(slave, "lua", "ping")
    print("main", response)
end)