#ifndef LUACXX_QOBSERVABLE_INCLUDED
#define LUACXX_QOBSERVABLE_INCLUDED

#include "../stack.hpp"
#include "../reference.hpp"
#include "../algorithm.hpp"
#include "../convert/string.hpp"
#include "QEvent.hpp"
#include <iostream>

namespace lua {

template <class Object>
class QObservable : public Object
{

lua::reference _target;

public:
    template <class... Args>
    QObservable(lua_State* const state, Args... args) :
        Object(args...),
        _target(state)
    {
    }

    bool event(QEvent* event) override
    {
        if (_target && lua::call<bool>(_target, event)) {
            return true;
        } else {
            return Object::event(event);
        }
    }

    void setDelegate(const lua::index& delegate)
    {
        _target = delegate;
    }
};

template <class T>
int QObservable_event(lua_State* const state)
{
    lua::get<QObservable<T>*>(state, 1)->setDelegate(lua::index(state, 2));
    return 0;
}

template <class T>
struct Metatable<QObservable<T>>
{
    static constexpr const char* name = "QObservable";

    static bool metatable(const lua::index& mt, QObservable<T>* const value)
    {
        mt["event"] = QObservable_event<T>;
        return lua::Metatable<T>::metatable(mt, value);
    }
};

}; // namespace lua

extern "C" int luaopen_Qt5Core_QObservable(lua_State* const);

#endif // LUACXX_QOBSERVABLE_INCLUDED
