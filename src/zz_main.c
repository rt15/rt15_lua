#include <rpr.h>

#include <stdlib.h>
#include <lauxlib.h>
#include <lualib.h>

/* Misc functions. */
RT_EXPORT rt_n32 RT_CDECL zz_file_path_get_type(lua_State *lua_state);
RT_EXPORT rt_n32 RT_CDECL zz_file_system_delete_dir_recursively(lua_State *lua_state);
RT_EXPORT rt_n32 RT_CDECL zz_is_windows(lua_State *lua_state);
RT_EXPORT rt_n32 RT_CDECL zz_execute(lua_State *lua_state);
RT_EXPORT rt_n32 RT_CDECL zz_file_path_set_current_dir(lua_State *lua_state);

/* Big functions. */
RT_EXPORT rt_n32 RT_CDECL zz_replace_in_file(lua_State *lua_state);
RT_EXPORT rt_n32 RT_CDECL zz_list_files(lua_State *lua_state);
RT_EXPORT rt_n32 RT_CDECL zz_convert_eol(lua_State *lua_state);

static const luaL_Reg zz_lib_functions[] = {
	{ "file_path_get_type", zz_file_path_get_type },
	{ "file_system_delete_dir_recursively", zz_file_system_delete_dir_recursively },
	{ "is_windows", zz_is_windows },
	{ "execute", zz_execute },
	{ "file_path_set_current_dir", zz_file_path_set_current_dir },
	{ "replace_in_file", zz_replace_in_file },
	{ "list_files", zz_list_files },
	{ "convert_eol", zz_convert_eol },
	{ RT_NULL, RT_NULL }
};

RT_EXPORT rt_n32 RT_CDECL zz_open_lib(lua_State *lua_state)
{
	luaL_newlib(lua_state, zz_lib_functions);
	lua_setglobal(lua_state, "rt15_lua");
	return 1;
}
