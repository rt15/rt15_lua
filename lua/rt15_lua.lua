RT_FILE_PATH_TYPE_NONE = 0;
RT_FILE_PATH_TYPE_DIR = 1;
RT_FILE_PATH_TYPE_FILE = 2;

local open_lib, error_message = package.loadlib("rt15_luad.dll", "zz_open_lib");
if open_lib == nil then
  error(error_message);
end
open_lib();

function file_path_get_type(path)
  return rt15_lua.file_path_get_type(path);
end

function file_system_delete_dir_recursively(dir_path)
  return rt15_lua.file_system_delete_dir_recursively(dir_path);
end

function is_windows()
  return rt15_lua.is_windows();
end

function execute(command)
  print(command);
  return rt15_lua.execute(command);
end

function file_path_set_current_dir(path)
  return rt15_lua.file_path_set_current_dir(path);
end

function replace_in_file(searched, replacement, file_path)
  return rt15_lua.replace_in_file(searched, replacement, file_path);
end

function list_files(dir_path, extension, recursive)
  return rt15_lua.list_files(dir_path, extension, recursive);
end
