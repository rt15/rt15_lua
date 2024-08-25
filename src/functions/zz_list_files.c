#include <rpr.h>

#include <lauxlib.h>
#include <lualib.h>

#include "utils/zz_lua_utils.h"

struct zz_list_files_context {
	lua_State *lua_state;
	rt_char *extension;
	rt_un index;
};

static rt_s zz_list_files_callback(const rt_char *path, enum rt_file_path_type type, void *context)
{
	struct zz_list_files_context *list_files_context = (struct zz_list_files_context*)context;
	lua_State *lua_state = list_files_context->lua_state;
	rt_char *extension = list_files_context->extension;
	rt_un path_size = rt_char_get_size(path);
	rt_un extension_size = rt_char_get_size(extension);
	rt_char8 file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_char8 *output;
	rt_b push_path;
	rt_s ret;

	if (type == RT_FILE_PATH_TYPE_FILE) {

		if (RT_UNLIKELY(!rt_encoding_encode(path, path_size, RT_ENCODING_SYSTEM_DEFAULT, file_path, RT_FILE_PATH_SIZE, RT_NULL, RT_NULL, &output, &file_path_size, RT_NULL)))
			goto error;

		if (extension_size) {
			push_path = rt_char_ends_with(path, path_size, extension, extension_size);
		} else {
			push_path = RT_TRUE;
		}

		if (push_path) {
			lua_pushstring(lua_state, file_path);
			lua_rawseti(lua_state, -2, list_files_context->index);
			list_files_context->index++;
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_list_files_do(lua_State *lua_state, rt_char *dir_path, rt_char *extension, rt_b recursively)
{
	struct zz_list_files_context list_files_context;
	rt_s ret;

	list_files_context.lua_state = lua_state;
	list_files_context.extension = extension;
	list_files_context.index = 1;

	if (RT_UNLIKELY(!rt_file_path_browse(dir_path, &zz_list_files_callback, recursively, RT_TRUE, &list_files_context)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_n32 RT_CDECL zz_list_files(lua_State *lua_state)
{
	rt_char dir_path[RT_FILE_PATH_SIZE];
	rt_un dir_path_size;
	rt_char extension[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un extension_size;
	rt_b recursively; 
	rt_s ret;

	lua_newtable(lua_state);

	if (RT_UNLIKELY(!zz_lua_utils_get_char(lua_state, 1, dir_path, RT_FILE_PATH_SIZE, &dir_path_size)))
		goto error;

	if (RT_UNLIKELY(!zz_lua_utils_get_char(lua_state, 2, extension, RT_FILE_PATH_SIZE, &extension_size)))
		goto error;

	recursively = lua_toboolean(lua_state, 3);

	if (RT_UNLIKELY(!zz_list_files_do(lua_state, dir_path, extension, recursively)))
		goto error;

	ret = 1;
free:
	return ret;

error:
	zz_lua_utils_push_last_error_message(lua_state);
	ret = 2;
	goto free;
}
