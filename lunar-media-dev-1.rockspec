---@diagnostic disable: lowercase-global
package = "lunar-media"
version = "dev-1"
source = {
   url = "git+https://github.com/Penguin-Spy/lunar-media.git"
}
description = {
   summary = "simple media player using lua and libvlc. interactive CLI only, no GUI.",
   homepage = "https://github.com/Penguin-Spy/lunar-media",
   license = "MPL-2.0"
}
dependencies = {
   "lua ~> 5.4",
   "luafilesystem",
   "lunajson"
}
build = { type = "none" }
