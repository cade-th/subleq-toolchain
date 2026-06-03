local json = {}

-- Minimal JSON parser by rxi (simplified)
local function kind_of(obj)
  if type(obj) ~= 'table' then return type(obj) end
  local i = 1
  for _ in pairs(obj) do
    if obj[i] == nil then return 'table' end
    i = i + 1
  end
  if i == 1 then return 'table' else return 'array' end
end

local function escape_str(s)
  local escape_char_map = {
    [ "\\" ] = "\\\\", [ "\"" ] = "\\\"", [ "\b" ] = "\\b",
    [ "\f" ] = "\\f", [ "\n" ] = "\\n", [ "\r" ] = "\\r",
    [ "\t" ] = "\\t",
  }
  return s:gsub("[\"\\\b\f\n\r\t]", escape_char_map)
end

function json.encode(obj)
  local t = type(obj)
  if t == 'number' or t == 'boolean' then return tostring(obj) end
  if t == 'string' then return '"' .. escape_str(obj) .. '"' end
  if t == 'table' then
    local k = kind_of(obj)
    local res = {}
    if k == 'array' then
      for i, v in ipairs(obj) do table.insert(res, json.encode(v)) end
      return "[" .. table.concat(res, ",") .. "]"
    else
      for key, v in pairs(obj) do table.insert(res, json.encode(tostring(key)) .. ":" .. json.encode(v)) end
      return "{" .. table.concat(res, ",") .. "}"
    end
  end
  return "null"
end

local function parse_array(str, pos)
  local res = {}
  pos = pos + 1
  while pos <= #str do
    pos = str:find("%S", pos)
    if not pos then error("Unexpected end of JSON array") end
    if str:sub(pos, pos) == "]" then return res, pos + 1 end
    local val, next_pos = json.decode_val(str, pos)
    table.insert(res, val)
    pos = str:find("%S", next_pos)
    if not pos then error("Unexpected end of JSON array") end
    local char = str:sub(pos, pos)
    if char == "]" then return res, pos + 1 end
    if char ~= "," then error("Expected ',' or ']' at pos " .. pos) end
    pos = pos + 1
  end
end

local function parse_object(str, pos)
  local res = {}
  pos = pos + 1
  while pos <= #str do
    pos = str:find("%S", pos)
    if not pos then error("Unexpected end of JSON object") end
    if str:sub(pos, pos) == "}" then return res, pos + 1 end
    local key, next_pos = json.decode_val(str, pos)
    if type(key) ~= "string" then error("Object key must be string at pos " .. pos) end
    pos = str:find("%S", next_pos)
    if not pos or str:sub(pos, pos) ~= ":" then error("Expected ':' at pos " .. pos) end
    local val, val_pos = json.decode_val(str, pos + 1)
    res[key] = val
    pos = str:find("%S", val_pos)
    if not pos then error("Unexpected end of JSON object") end
    local char = str:sub(pos, pos)
    if char == "}" then return res, pos + 1 end
    if char ~= "," then error("Expected ',' or '}' at pos " .. pos) end
    pos = pos + 1
  end
end

local function parse_string(str, pos)
  local res = ""
  pos = pos + 1
  while pos <= #str do
    local next_quote = str:find("\"", pos)
    if not next_quote then error("Unterminated string") end
    local s = str:sub(pos, next_quote - 1)
    if s:sub(-1) == "\\" then
      res = res .. s:sub(1, -2) .. "\""
      pos = next_quote + 1
    else
      res = res .. s
      return res, next_quote + 1
    end
  end
end

function json.decode_val(str, pos)
  pos = str:find("%S", pos)
  if not pos then return nil, nil end
  local char = str:sub(pos, pos)
  if char == "{" then return parse_object(str, pos) end
  if char == "[" then return parse_array(str, pos) end
  if char == "\"" then return parse_string(str, pos) end
  if char == "t" and str:sub(pos, pos + 3) == "true" then return true, pos + 4 end
  if char == "f" and str:sub(pos, pos + 4) == "false" then return false, pos + 5 end
  if char == "n" and str:sub(pos, pos + 3) == "null" then return nil, pos + 4 end
  local num_str = str:match("^-?%d+%.?%d*[eE]?[-+]?%d*", pos)
  if num_str then return tonumber(num_str), pos + #num_str end
  error("Invalid JSON value at pos " .. pos)
end

function json.decode(str)
  local val, next_pos = json.decode_val(str, 1)
  return val
end

return json
