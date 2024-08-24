#ifndef ZZ_LUA_UTILS_H
#define ZZ_LUA_UTILS_H

#include <rpr.h>

#include <lauxlib.h>
#include <lualib.h>

void zz_lua_utils_push_last_error_message(lua_State *lpLuaState);

rt_s zz_lua_utils_get_char(lua_State *lua_state, rt_un arg_index, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size);

#endif /* ZZ_LUA_UTILS_H */
