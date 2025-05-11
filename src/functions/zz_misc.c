#include <rpr.h>

#include <stdlib.h>
#include <lauxlib.h>
#include <lualib.h>

#include "utils/zz_lua_utils.h"

rt_n32 RT_CDECL zz_file_path_get_type(lua_State *lua_state)
{
	rt_char path[RT_FILE_PATH_SIZE];
	rt_un path_size;
	enum rt_file_path_type type;
	rt_n32 ret = 2;

	if (RT_UNLIKELY(!zz_lua_utils_get_char(lua_state, 1, path, RT_FILE_PATH_SIZE, &path_size)))
		goto end;

	if (RT_UNLIKELY(!rt_file_path_get_type(path, &type)))
		goto end;

	lua_pushinteger(lua_state, type);

	ret = 1;
end:
	if (RT_UNLIKELY(ret == 2)) {
		lua_pushinteger(lua_state, RT_FILE_PATH_TYPE_NONE);
		zz_lua_utils_push_last_error_message(lua_state);
	}

	return ret;
}

rt_n32 RT_CDECL zz_file_system_delete_dir_recursively(lua_State *lua_state)
{
	rt_char path[RT_FILE_PATH_SIZE];
	rt_un path_size;
	rt_n32 ret = 1;

	if (RT_UNLIKELY(!zz_lua_utils_get_char(lua_state, 1, path, RT_FILE_PATH_SIZE, &path_size)))
		goto end;

	if (RT_UNLIKELY(!rt_file_system_delete_dir_recursively(path)))
		goto end;

	ret = 0;
end:
	if (RT_UNLIKELY(ret == 1)) {
		zz_lua_utils_push_last_error_message(lua_state);
	}

	return ret;
}

rt_n32 RT_CDECL zz_is_windows(lua_State *lua_state)
{
#ifdef RT_DEFINE_WINDOWS
	lua_pushboolean(lua_state, RT_TRUE);
#else
	lua_pushboolean(lua_state, RT_FALSE);
#endif
	return 1;
}

rt_n32 RT_CDECL zz_execute(lua_State *lua_state)
{
	rt_char8 *command;
	rt_n32 ret;

	command = (rt_char8*)luaL_checkstring(lua_state, 1);
	if (RT_UNLIKELY(!command)) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		zz_lua_utils_push_last_error_message(lua_state);
		ret = 1;
		goto end;
	}

	if (system(command)) {
		lua_pushstring(lua_state, "Command failed.");
		ret = 1;
	} else {
		ret = 0;
	}
end:
	return ret;
}

rt_n32 RT_CDECL zz_file_path_set_current_dir(lua_State *lua_state)
{
	rt_char path[RT_FILE_PATH_SIZE];
	rt_un path_size;
	rt_n32 ret = 1;

	if (RT_UNLIKELY(!zz_lua_utils_get_char(lua_state, 1, path, RT_FILE_PATH_SIZE, &path_size)))
		goto end;

	if (RT_UNLIKELY(!rt_file_path_set_current_dir(path)))
		goto end;

	ret = 0;
end:
	if (RT_UNLIKELY(ret == 1)) {
		zz_lua_utils_push_last_error_message(lua_state);
	}

	return ret;
}
