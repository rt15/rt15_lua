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
	rt_char8 *end_of_line;
	rt_s ret;

	if (RT_UNLIKELY(!rt_char8_replace(line, line_size, searched, searched_size, replacement, replacement_size, buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!output_stream->write(output_stream, buffer, buffer_size)))
		goto error;

	switch (eol) {
	case RT_EOL_NONE:
		end_of_line = RT_NULL;
		break;
	case RT_EOL_LF:
		end_of_line = "\n";
		break;
	case RT_EOL_CRLF:
		end_of_line = "\r\n";
		break;
	case RT_EOL_CR:
		end_of_line = "\r";
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	if (end_of_line) {
		if (RT_UNLIKELY(!output_stream->write(output_stream, end_of_line, rt_char8_get_size(end_of_line))))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_replace_in_file_with_files(rt_char8 *searched, rt_char8 *replacement, struct rt_file *file, struct rt_file *tmp_file)
{
	struct rt_input_stream *input_stream;
	rt_char8 buffer[RT_CHAR8_HALF_BIG_STRING_SIZE];
	struct zz_replace_in_file_context context;
	rt_s ret;

	input_stream = &file->io_device.input_stream;

	context.output_stream = &tmp_file->io_device.output_stream;
	context.searched = searched;
	context.searched_size = rt_char8_get_size(searched);
	context.replacement = replacement;
	context.replacement_size = rt_char8_get_size(replacement);

	if (RT_UNLIKELY(!rt_read_lines(input_stream, buffer, RT_CHAR8_HALF_BIG_STRING_SIZE, &zz_replace_in_file_callback, &context)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_replace_in_file_with_tmp_file_path(rt_char8 *searched, rt_char8 *replacement, rt_char *file_path, rt_char *tmp_file_path)
{
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	struct rt_file tmp_file;
	rt_b tmp_file_created = RT_FALSE;
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_create(&file, file_path, RT_FILE_MODE_READ)))
		goto error;
	file_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_file_create(&tmp_file, tmp_file_path, RT_FILE_MODE_TRUNCATE)))
		goto error;
	tmp_file_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_replace_in_file_with_files(searched, replacement, &file, &tmp_file)))
		goto error;

	ret = RT_OK;
free:
	if (tmp_file_created) {
		tmp_file_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_io_device_free(&tmp_file.io_device) && ret))
			goto error;
	}
	if (file_created) {
		file_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_io_device_free(&file.io_device) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_replace_in_file_do(rt_char8 *searched, rt_char8 *replacement, rt_char *file_path)
{
	rt_char tmp_file_path[RT_FILE_PATH_SIZE];
	rt_un tmp_file_path_size;
	rt_s ret;

	tmp_file_path_size = rt_char_get_size(file_path);
	if (RT_UNLIKELY(!rt_char_copy(file_path, tmp_file_path_size, tmp_file_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R(".tmp"), 4, tmp_file_path, RT_FILE_PATH_SIZE, &tmp_file_path_size))) goto error;
	if (RT_UNLIKELY(!zz_replace_in_file_with_tmp_file_path(searched, replacement, file_path, tmp_file_path))) goto error;
	if (RT_UNLIKELY(!rt_file_system_delete_file(file_path))) goto error;
	if (RT_UNLIKELY(!rt_file_system_move_file(tmp_file_path, file_path))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

RT_EXPORT rt_n32 RT_CDECL zz_replace_in_file(lua_State *lua_state)
{
	rt_char8 *searched;
	rt_char8 *replacement;
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un size;
	rt_n32 ret;

	searched = (rt_char8*)luaL_checkstring(lua_state, 1);
	if (RT_UNLIKELY(!searched)) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	replacement = (rt_char8*)luaL_checkstring(lua_state, 2);
	if (RT_UNLIKELY(!replacement)) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	if (RT_UNLIKELY(!zz_lua_utils_get_char(lua_state, 3, file_path, RT_FILE_PATH_SIZE, &size)))
		goto error;

	if (RT_UNLIKELY(!zz_replace_in_file_do(searched, replacement, file_path)))
		goto error;

	ret = 0;
free:
	return ret;

error:
	zz_lua_utils_push_last_error_message(lua_state);
	ret = 1;
	goto free;
}
