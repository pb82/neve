#include "sandbox.hpp"

Sandbox::Sandbox(Action *action) : action(action) {
    L = luaL_newstate();
    loadLibraries();
}

Sandbox::~Sandbox() {
    lua_close(L);
}

void Sandbox::loadLibraries() {
    // Load safe libraries
    luaL_requiref(L, LUA_GLOBAL,        luaopen_base,       true);
    luaL_requiref(L, LUA_STRLIBNAME,    luaopen_string,     true);
    luaL_requiref(L, LUA_MATHLIBNAME,   luaopen_math,       true);
    luaL_requiref(L, LUA_UTF8LIBNAME,   luaopen_utf8,       true);
    luaL_requiref(L, LUA_COLIBNAME,     luaopen_coroutine,  true);
    luaL_requiref(L, LUA_TABLIBNAME,    luaopen_table,      true);

    // The OS lib has some useful functions for data and time we
    // want to retain these and only disable the dangerous ones
    luaL_requiref(L, LUA_OSLIBNAME,     luaopen_os,         true);

    // Disable unsafe functions
    DISABLE_FN(LUA_GLOBAL,              "dofile");
    DISABLE_FN(LUA_GLOBAL,              "collectgarbage");
    DISABLE_FN(LUA_GLOBAL,              "load");
    DISABLE_FN(LUA_GLOBAL,              "loadfile");
    DISABLE_FN(LUA_GLOBAL,              "require");
    DISABLE_FN(LUA_GLOBAL,              "module");
    DISABLE_FN(LUA_OSLIBNAME,           "execute");
    DISABLE_FN(LUA_OSLIBNAME,           "exit");
    DISABLE_FN(LUA_OSLIBNAME,           "getenv");
    DISABLE_FN(LUA_OSLIBNAME,           "remove");
    DISABLE_FN(LUA_OSLIBNAME,           "setlocale");
    DISABLE_FN(LUA_OSLIBNAME,           "tmpname");
}

RunCode Sandbox::run(JSON::Value &args, JSON::Value &result, std::string *msg) {
    int status, type;
    const char *err = nullptr;

    if (action->timeout > 0 || action->memory) {
        UPDATE_MS(this, started);
        lua_sethook(L, hook, LUA_MASKCOUNT, GRANULARITY);
    }

    if ((status = luaL_loadbuffer(L,
            action->bytecode.c_str(),
            action->bytecode.size(),
            action->name.c_str())) != LUA_OK) {
        err = lua_tostring(L, -1);
        if (err) { *msg = err; } else { *msg = "Failed to load bytecode"; }
        return ErrBytecode;
    }

    // Priming: run the script to set up all global vars
    if ((status = lua_pcall(L, 0, 0, 0)) != LUA_OK) {
        err = lua_tostring(L, -1);
        if (err) { *msg = err; } else { *msg = "Failed priming"; }
        return ErrPriming;
    }

    // Push the main function on the stack
    if((type = lua_getglobal(L, "main")) != LUA_TFUNCTION) {
        err = lua_tostring(L, -1);
        if (err) { *msg = err; } else { *msg = "Failed to push main function"; }
        return ErrNoMain;
    }

    // Set up the rescue point to jump back to when a script
    // has to be aborted
    int error = setjmp(env);
    switch (error) {
    case 0:
        break;
    case ErrTimeout:
        *msg = "Action `";
        msg->append(action->name);
        msg->append("' aborted after timeout");
        return ErrTimeout;
    case ErrMemory:
        *msg = "Action `";
        msg->append(action->name);
        msg->append("' aborted after violating memory constraints");
        return ErrMemory;
    default:
        *msg = "Unknown error";
        return ErrUnknown;
    }

    // Inject the arguments
    args.toLua(L);

    // Second call to actually run the main function
    if ((status = lua_pcall(L, 1, 1, 0)) != LUA_OK) {
        err = lua_tostring(L, -1);
        if (err) { *msg = err; } else { *msg = "Error in main"; }
        return ErrMain;
    }

    result.fromLua(L);
    return Success;
}

void Sandbox::hook(lua_State *L, lua_Debug *) {
    GET_ENV(env);
    GET_THIS(Sandbox, This);
    UPDATE_MS(This, running);

    if (This->action->timeout > 0 && This->running >= This->action->timeout) {
        std::longjmp(*env, ErrTimeout);
    }

    if (This->action->memory > 0 && lua_gc(L, LUA_GCCOUNT, 0) >= This->action->memory) {
        std::longjmp(*env, ErrMemory);
    }
}
