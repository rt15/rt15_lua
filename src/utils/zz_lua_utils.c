#include "utils/zz_lua_utils.h"

void zz_lua_utils_push_last_error_message(lua_State *lpLuaState)
{
	rt_char last_error_message[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un last_error_message_size = 0;
	rt_char8 last_error_message8[RT_CHAR8_QUARTER_BIG_STRING_SIZE];
	rt_un last_error_message8_size;
	rt_char8 *output;


	if (RT_UNLIKELY(!rt_error_message_append_last(last_error_message, RT_CHAR_QUARTER_BIG_STRING_SIZE, &last_error_message_size)))
		goto error;

	if (RT_UNLIKELY(!rt_encoding_encode(last_error_message, last_error_message_size, RT_ENCODING_SYSTEM_DEFAULT, last_error_message8, RT_CHAR8_QUARTER_BIG_STRING_SIZE, RT_NULL, RT_NULL, &output, &last_error_message8_size, RT_NULL)))
		goto error;

	lua_pushstring(lpLuaState, last_error_message8);

free:
	return;

error:
	lua_pushstring(lpLuaState, "Unexpected error.");
	goto free;
}

rt_s zz_lua_utils_get_char(lua_State *lua_state, rt_un arg_index, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_char8 *str8;
	rt_char *output;
	rt_s ret;

	str8 = (rt_char8*)luaL_checkstring(lua_state, arg_index);
	if (RT_UNLIKELY(!str8)) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	if (RT_UNLIKELY(!rt_encoding_decode(str8, rt_char8_get_size(str8), RT_ENCODING_SYSTEM_DEFAULT, buffer, buffer_capacity, RT_NULL, RT_NULL, &output, buffer_size, RT_NULL)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
