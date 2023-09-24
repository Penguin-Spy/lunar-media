local colors = {
  ["nil"] = "\27[90m",
  ["number"] = "\27[92m",
  ["string"] = "\27[31m",
  ["boolean"] = "\27[34m",
  ["function"] = "\27[95m",
  ["thread"] = "\27[96m",
  ["userdata"] = "\27[33m",
  RESET = "\27[m"
}
local function colorize(v)
  if type(v) == "string" then
    return colors.string .. "\"" .. v .. "\"" .. colors.RESET
  else
    return colors[type(v)] .. tostring(v) .. colors.RESET
  end
end

local function indent(layer, value)
  return string.rep("  ", layer) .. value
end

local function raw(variable, layer, inline)
  local ret = ""
  layer = layer or 0
  if type(variable) == "table" then
    if (layer < 10) then
      ret = ret .. "{\n"
      for key, value in pairs(variable) do
        local rep
        if value == package then -- avoid printing some large, recursive tables as an element of a table
          rep = colors["nil"] .. "[ package ]" .. colors.RESET
        elseif value == _G then
          rep = colors["nil"] .. "[ _G ]" .. colors.RESET
        elseif value == variable then
          rep = colors["nil"] .. "[ self reference ]" .. colors.RESET
        else
          rep = raw(value, layer + 1, true)
        end
        ret = ret .. indent(layer + 1, "[" .. colorize(key) .. "] = " .. rep) .. ",\n"
      end
      ret = ret .. indent(layer, "}")
    else
      ret = ret .. colors["nil"] .. "{ more layers }" .. colors.RESET
    end
  else
    local str = colorize(variable)
    ret = ret .. (inline and str or indent(layer, str))
  end

  return ret
end

local function dump(...)
  local ret = ""
  for _, value in ipairs(table.pack(...)) do
    ret = ret .. raw(value) .. "\t"
  end
  return ret
end

--- Return value is callable and will print directly to stdio. \
--- Dumps the provided values, recusring through tables.
return setmetatable({
  raw = raw,
  dump = dump,
  colorize = colorize
}, {
  __call = function(_, ...)
    print(dump(...))
  end
})
