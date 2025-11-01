#include <rpr.h>

#include <lauxlib.h>
#include <lualib.h>

#include "utils/zz_lua_utils.h"

struct zz_replace_in_file_context {
	struct rt_output_stream *output_stream;
	rt_char8 *searched;
	rt_un searched_size;
	rt_char8 *replacement;
	rt_un replacement_size;
};

static rt_s zz_replace_in_file_callback(const rt_char8 *line, rt_un line_size, enum rt_eol eol, void *context)
{
	struct zz_replace_in_file_context *replace_in_file_context = (struct zz_replace_in_file_context*)context;
	struct rt_output_stream *output_stream = replace_in_file_context->output_stream;
	rt_char8 *searched = replace_in_file_context->searched;
	rt_un searched_size = replace_in_file_context->searched_size;
	rt_char8 *replacement = replace_in_file_context->replacement;
	rt_un replacement_size = replace_in_file_context->replacement_size;
	rt_char8 buffer[RT_CHAR8_HALF_BIG_STRING_SIZE];
	rt_un buffer_size = 0;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_char8_replace(line, line_size, searched, searched_size, replacement, replacement_size, buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto end;

	if (RT_UNLIKELY(!output_stream->write(output_stream, buffer, buffer_size)))
		goto end;

	if (RT_UNLIKELY(!rt_process_file_write_eol(eol, output_stream)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_replace_in_file_with_files(rt_char8 *searched, rt_char8 *replacement, struct rt_file *file, struct rt_file *tmp_file)
{
	struct rt_input_stream *input_stream;
	rt_char8 buffer[RT_CHAR8_HALF_BIG_STRING_SIZE];
	struct zz_replace_in_file_context context;
	rt_s ret = RT_FAILED;

	input_stream = &file->io_device.input_stream;

	context.output_stream = &tmp_file->io_device.output_stream;
	context.searched = searched;
	context.searched_size = rt_char8_get_size(searched);
	context.replacement = replacement;
	context.replacement_size = rt_char8_get_size(replacement);

	if (RT_UNLIKELY(!rt_read_lines(input_stream, buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, &zz_replace_in_file_callback, &context)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_replace_in_file_with_tmp_file_path(rt_char8 *searched, rt_char8 *replacement, rt_char *file_path, rt_char *tmp_file_path)
{
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	struct rt_file tmp_file;
	rt_b tmp_file_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_file_create(&file, file_path, RT_FILE_MODE_READ)))
		goto end;
	file_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_file_create(&tmp_file, tmp_file_path, RT_FILE_MODE_TRUNCATE)))
		goto end;
	tmp_file_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_replace_in_file_with_files(searched, replacement, &file, &tmp_file)))
		goto end;

	ret = RT_OK;
end:
	if (tmp_file_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&tmp_file.io_device)))
			ret = RT_FAILED;
	}
	if (file_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&file.io_device)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_replace_in_file_do(rt_char8 *searched, rt_char8 *replacement, rt_char *file_path)
{
	rt_char tmp_file_path[RT_FILE_PATH_SIZE];
	rt_un tmp_file_path_size;
	rt_s ret = RT_FAILED;

	tmp_file_path_size = rt_char_get_size(file_path);
	if (RT_UNLIKELY(!rt_char_copy(file_path, tmp_file_path_size, tmp_file_path, RT_FILE_PATH_SIZE))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R(".tmp"), 4, tmp_file_path, RT_FILE_PATH_SIZE, &tmp_file_path_size))) goto end;
	if (RT_UNLIKELY(!zz_replace_in_file_with_tmp_file_path(searched, replacement, file_path, tmp_file_path))) goto end;
	if (RT_UNLIKELY(!rt_file_system_delete_file(file_path))) goto end;
	if (RT_UNLIKELY(!rt_file_system_move_file(tmp_file_path, file_path))) goto end;

	ret = RT_OK;
end:
	return ret;
}

RT_EXPORT rt_n32 RT_CDECL zz_replace_in_file(lua_State *lua_state)
{
	rt_char8 *searched;
	rt_char8 *replacement;
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un size;
	rt_n32 ret = 1;

	searched = (rt_char8*)luaL_checkstring(lua_state, 1);
	if (RT_UNLIKELY(!searched)) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	replacement = (rt_char8*)luaL_checkstring(lua_state, 2);
	if (RT_UNLIKELY(!replacement)) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	if (RT_UNLIKELY(!zz_lua_utils_get_char(lua_state, 3, file_path, RT_FILE_PATH_SIZE, &size)))
		goto end;

	if (RT_UNLIKELY(!zz_replace_in_file_do(searched, replacement, file_path)))
		goto end;

	ret = 0;
end:
	if (RT_UNLIKELY(ret == 1)) {
		zz_lua_utils_push_last_error_message(lua_state);
	}

	return ret;
}
