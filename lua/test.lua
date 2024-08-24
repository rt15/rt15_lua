require("rt15_lua");

local error_message = execute("echo \"Hello, world!\"");
if error_message ~= nil then
  error(error_message);
end
