#include <rpr.h>

#include <lauxlib.h>
#include <lualib.h>

#include "utils/zz_lua_utils.h"

struct zz_convert_eol_context {
	enum rt_eol eol;
};

static rt_s zz_convert_eol_callback(const rt_char8 *line, rt_un line_size, RT_UNUSED enum rt_eol eol, struct rt_process_file_context *process_file_context)
{
	struct rt_output_stream *output_stream = process_file_context->output_stream;
	enum rt_eol target_eol = ((struct zz_convert_eol_context*)process_file_context->context)->eol;
	rt_s ret;

	if (RT_UNLIKELY(!output_stream->write(output_stream, line, line_size)))
		goto error;

	if (RT_UNLIKELY(!rt_process_file_write_eol(target_eol, output_stream)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_convert_eol_do(rt_char *file_path, enum rt_eol eol)
{
	struct zz_convert_eol_context convert_eol_context;
	rt_s ret;

	convert_eol_context.eol = eol;

	if (RT_UNLIKELY(!rt_process_file(file_path, &zz_convert_eol_callback, &convert_eol_context)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_n32 RT_CDECL zz_convert_eol(lua_State *lua_state)
{
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_char eol_str[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un eol_str_size;
	enum rt_eol eol;
	rt_n32 ret;

	if (RT_UNLIKELY(!zz_lua_utils_get_char(lua_state, 1, file_path, RT_FILE_PATH_SIZE, &file_path_size)))
		goto error;

	if (RT_UNLIKELY(!zz_lua_utils_get_char(lua_state, 2, eol_str, RT_CHAR_QUARTER_BIG_STRING_SIZE, &eol_str_size)))
		goto error;

	rt_char_fast_upper(eol_str);
	if (rt_char_equals(eol_str, eol_str_size, _R("CRLF"), 4)) {
		eol = RT_EOL_CRLF;
	} else if (rt_char_equals(eol_str, eol_str_size, _R("LF"), 2)) {
		eol = RT_EOL_LF;
	} else {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);	
		goto error;
	}

	if (RT_UNLIKELY(!zz_convert_eol_do(file_path, eol)))
		goto error;

	ret = 0;
free:
	return ret;

error:
	zz_lua_utils_push_last_error_message(lua_state);
	ret = 1;
	goto free;
}
