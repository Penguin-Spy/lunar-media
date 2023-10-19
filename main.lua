local lfs = require "lfs"
local dump = require "dump"

script.on("media_play_pause", function()
  local is_playing = vlc.is_playing()
  print("media_play_pause: " .. tostring(is_playing))
  if (is_playing) then
    vlc.pause()
  else
    vlc.resume()
  end
end)

script.on("media_next_track", function()
  print("media_next_track")
  vlc.resume()
end)

script.on("media_previous_track", function()
  print("media_previous_track")
  vlc.pause()
end)

script.on("playback_complete", function()
  print("playback complete")
end)

--script.exit()


--vlc.play("Reach for the Summit.mp3")
vlc.play("Chapter Complete - Ascending B-Sides.mp3")

vlc.set_volume(70) -- (0 = mute, 100 = 0dB)
--vlc.stop()

print("lunar-media registry contents:")
dump(debug.getregistry()["lunar-media"])


local a, b = io.read()
print("io.read: " .. tostring(a) .. " " .. tostring(b))
