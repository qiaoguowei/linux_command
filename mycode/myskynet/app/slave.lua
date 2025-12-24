local skynet = require "skynet"

local CMD = {}

function CMD.ping()
    skynet.retpack("pong")
end

skynet.start(function()
    skynet.dispatch("lua", function(session, source, cmd, ...)end)
end)