#ifndef MACROS_H
#define MACROS_H

#define LUA_USERDATA_SELF "__NV_SELF"

#define DISABLE_FN(TABLE, FN)   \
	lua_getglobal(L, TABLE);    \
	lua_pushstring(L, FN);      \
	lua_pushnil(L);             \
	lua_settable(L, -3);        \
	lua_pop(L, 1)

#define SET_THIS \
	lua_pushlightuserdata(L, this); \
	lua_setglobal(L, LUA_USERDATA_SELF)

#define GET_THIS(TYPE, NAME) \
	lua_getglobal(L, LUA_USERDATA_SELF); \
	luaL_checktype(L, -1, LUA_TLIGHTUSERDATA); \
	TYPE *NAME = static_cast<TYPE *>(lua_touserdata(L, -1))

#define SET_ENV \
	lua_pushlightuserdata(L, &env); \
	lua_setglobal(L, "__ENV")

#define GET_ENV(NAME) \
	lua_getglobal(L, "__ENV"); \
	luaL_checktype(L, -1, LUA_TLIGHTUSERDATA); \
	jmp_buf *NAME = static_cast<jmp_buf *>(lua_touserdata(L, -1))

#endif // MACROS_H
