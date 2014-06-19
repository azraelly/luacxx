#include "QAbstractNativeEventFilter.hpp"
#include "../type/function.hpp"
#include "../thread.hpp"

#include <QAbstractNativeEventFilter>

void lua::QAbstractNativeEventFilter_metatable(const lua::index& mt)
{
    // TODO Set up metatable methods for this class
}

int QAbstractNativeEventFilter_new(lua_State* const state)
{
    lua::make<QAbstractNativeEventFilter>(state);
    // TODO Set up object-specific methods

    return 1;
}

int luaopen_luacxx_QAbstractNativeEventFilter(lua_State* const state)
{
    lua::thread env(state);

    env["QAbstractNativeEventFilter"] = lua::value::table;
    env["QAbstractNativeEventFilter"]["new"] = QAbstractNativeEventFilter_new;

    return 0;
}