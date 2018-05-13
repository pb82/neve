#ifndef MACROS_H
#define MACROS_H

#define LUA_USERDATA_SELF "__NV_SELF"

#define SET_THIS \
	lua_pushlightuserdata(L, this); \
	lua_setglobal(L, LUA_USERDATA_SELF)

#define GET_THIS(TYPE, NAME) \
	lua_getglobal(L, LUA_USERDATA_SELF); \
	luaL_checktype(L, -1, LUA_TLIGHTUSERDATA); \
	TYPE *NAME = static_cast<TYPE *>(lua_touserdata(L, -1))

#endif // MACROS_H
