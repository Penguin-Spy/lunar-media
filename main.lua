--local lfs = require "lfs"
local dump = require "dump"

---@type function[]
local handlers = {}
---@param event string
---@param handler function
local function on(event, handler)
  handlers[event] = handler
end

on("media_play_pause", function()
  local is_playing = vlc.is_playing()
  print("media_play_pause: " .. tostring(is_playing))
  if (is_playing) then
    vlc.pause()
  else
    vlc.resume()
  end
end)

on("media_next_track", function()
  print("media_next_track")
  vlc.resume()
end)

on("media_previous_track", function()
  print("media_previous_track")
  vlc.pause()
end)

on("playback_complete", function()
  print("playback complete")
end)

on("key", function(key)
  dump("got key:", key) --, string.char(code))
  if key == "Q" then
    vlc.stop()
    os.exit(true, true)
  end
end)

on("term_resize", function()
  print("resize")
end)


vlc.play("Chapter Complete - Ascending B-Sides.mp3")

vlc.set_volume(70) -- (0 = mute, 100 = 0dB)

--print("lunar-media registry contents:")
--dump(debug.getregistry()["lunar-media"])


while true do
  local data = table.pack(coroutine.yield())
  --dump("resumed with ", data)
  local handler = handlers[data[1]]
  if handler then
    handler(table.unpack(data, 2))
  else
    print("warning: unhandled event", dump(data))
  end
end
