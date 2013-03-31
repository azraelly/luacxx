#include "LuaEnvironment.hpp"
#include "LuaStack.hpp"
#include "LuaException.hpp"
#include "LuaValue.hpp"
#include "LuaReferenceAccessible.hpp"
#include "LuaAccessible.hpp"
#include "LuaUserdata.hpp"

LuaStack::LuaStack(Lua& lua) :
    _lua(lua),
    _offset(lua_gettop(luaState()))
{
}

lua_State* LuaStack::luaState() const
{
    return _lua.state;
}

LuaStack& LuaStack::offset(int offset)
{
    if(offset < 0)
        throw LuaException(&lua(), "Offset must be non-negative");
    if(offset > lua_gettop(luaState()))
        throw LuaException(&lua(), "Offset must be less than the top of lua's stack");
    _offset = offset;
    return (*this);
}

int LuaStack::size() const
{
    return lua_gettop(luaState()) - offset();
}

LuaIndex LuaStack::begin()
{
    return LuaIndex(*this, 1);
}

LuaIndex LuaStack::end()
{
    return LuaIndex(*this, size() + 1);
}

LuaIndex LuaStack::rbegin()
{
    return LuaIndex(*this, size(), -1);
}

LuaIndex LuaStack::rend()
{
    return LuaIndex(*this, 0, -1);
}

LuaIndex LuaStack::at(const int pos, const int direction)
{
    return LuaIndex(*this, pos, direction);
}

LuaStack& LuaStack::pop(int count)
{
    if(count > size())
        throw LuaException(&lua(), "Refusing to pop elements not managed by this stack");
    lua_pop(luaState(), count);
    return (*this);
}

LuaStack& LuaStack::shift(int count)
{
    while(count-- > 0) {
        // TODO We need to check if the offset is being moved here.
        lua_remove(luaState(), 1);
    }
    return (*this);
}

void LuaStack::checkPos(int pos) const
{
    if (isMagicalPos(pos))
        return;
    const int top=lua_gettop(luaState());
    if (pos == 0)
        throw LuaException(&lua(), "Stack position must not be zero");
    // Convert relative positions to absolute ones.
    if (pos < 0)
        pos += top;
    if (pos < offset())
        throw LuaException(&lua(), "Stack position is not managed by this stack");
    if (pos > top)
        throw LuaException(&lua(), "Stack position is beyond the top of the lua stack");
}

LuaStack& LuaStack::replace(int pos)
{
    checkPos(pos);
    if (empty())
        throw LuaException(&lua(), "Stack must not be empty when replacing elements");
    lua_replace(luaState(), pos);
    return (*this);
}

LuaStack& LuaStack::swap(int a, int b)
{
    checkPos(a);
    checkPos(b);

    lua_pushvalue(luaState(), b);
    lua_pushvalue(luaState(), a - 1);
    // Stack is now [..., b, a]

    // Replace b by popping the copy of a
    lua_replace(luaState(), b - 2);

    // Replace a by popping the copy of b
    lua_replace(luaState(), a - 1);

    return *this;
}

bool LuaStack::isMagicalPos(const int& pos) const
{
    return false;
}

lua::Type LuaStack::type(int pos) const
{
    checkPos(pos);
    return lua::convert_lua_type(lua_type(luaState(), pos));
}

std::string LuaStack::typestring(int pos) const
{
    checkPos(pos);
    return std::string(lua_typename(luaState(), lua_type(luaState(), pos)));
}

LuaStack& LuaStack::to(const char*& sink, int pos)
{
    checkPos(pos);
    sink = lua_tostring(luaState(), pos);
    return (*this);
}

LuaStack& LuaStack::to(std::string& sink, int pos)
{
    checkPos(pos);
    sink = lua_tostring(luaState(), pos);
    return (*this);
}

LuaStack& LuaStack::to(QString& sink, int pos)
{
    checkPos(pos);
    sink = lua_tostring(luaState(), pos);
    return (*this);
}

void* LuaStack::pointer(int pos)
{
    checkPos(pos);
    if (lua_islightuserdata(luaState(), pos) == 1) {
        return lua_touserdata(luaState(), pos);
    }
    return nullptr;
}

int LuaStack::length(int pos)
{
    checkPos(pos);
    int length;
    #if LUA_VERSION_NUM >= 502
        length = lua_rawlen(luaState(), pos);
    #else
        length = lua_objlen(luaState(), pos);
    #endif
    return length;
}

LuaValue LuaStack::save()
{
    checkPos(-1);
    return LuaValue(
        std::shared_ptr<LuaAccessible>(
            new LuaReferenceAccessible(lua())
        )
    );
}

LuaStack& LuaStack::to(bool& sink, int pos)
{
    checkPos(pos);
    sink = lua_toboolean(luaState(), pos);
    return (*this);
}

LuaStack& LuaStack::to(lua_Number& sink, int pos)
{
    checkPos(pos);
    sink = lua_tonumber(luaState(), pos);
    return (*this);
}

LuaStack& LuaStack::to(char& sink, int pos)
{
    checkPos(pos);
    size_t len = 1;
    sink = *lua_tolstring(luaState(), pos, &len);
    return (*this);
}

LuaStack& LuaStack::to(short& sink, int pos)
{
    checkPos(pos);
    sink = lua_tointeger(luaState(), pos);
    return (*this);
}

LuaStack& LuaStack::to(int& sink, int pos)
{
    checkPos(pos);
    sink = lua_tointeger(luaState(), pos);
    return (*this);
}

LuaStack& LuaStack::to(long& sink, int pos)
{
    checkPos(pos);
    sink = lua_tonumber(luaState(), pos);
    return (*this);
}

LuaStack& LuaStack::to(float& sink, int pos)
{
    checkPos(pos);
    sink = lua_tonumber(luaState(), pos);
    return (*this);
}

LuaStack& LuaStack::to(LuaUserdata*& sink, int pos)
{
    checkPos(pos);

    if (lua_isuserdata(luaState(), pos) == 1 && lua_islightuserdata(luaState(), pos) == 0) {
        sink = static_cast<LuaUserdata*>(lua_touserdata(luaState(), pos));
    } else {
        sink = 0;
    }
    return *this;
}

LuaStack& LuaStack::global(const char* name)
{
    lua_getglobal(luaState(), name);
    return (*this);
}

LuaStack& LuaStack::global(const std::string& name)
{
    global(name.c_str());
    return (*this);
}

LuaStack& LuaStack::global(const QString& name)
{
    global(name.toAscii().data());
    return (*this);
}

LuaStack& LuaStack::get(int tablePos)
{
    checkPos(tablePos);
    lua_gettable(luaState(), tablePos);
    return (*this);
}

LuaStack& LuaStack::pushedSet(int tablePos)
{
    checkPos(tablePos);
    lua_settable(luaState(), tablePos);
    return (*this);
}

LuaStack& LuaStack::operator<<(const char& value)
{
    return push(&value, 1);
}

LuaStack& LuaStack::operator<<(const char* value)
{
    lua_pushstring(luaState(), value);
    return (*this);
}

LuaStack& LuaStack::push(const char* value, int len)
{
    lua_pushlstring(luaState(), value, len);
    return (*this);
}

LuaStack& LuaStack::operator<<(const std::string& value)
{
    return *this << value.c_str();
}

LuaStack& LuaStack::operator<<(const lua_Number& value)
{
    lua_pushnumber(luaState(), value);
    return (*this);
}

LuaStack& LuaStack::operator<<(const short& value)
{
    lua_pushinteger(luaState(), value);
    return (*this);
}

LuaStack& LuaStack::operator<<(const int& value)
{
    lua_pushinteger(luaState(), value);
    return (*this);
}

LuaStack& LuaStack::operator<<(const long& value)
{
    lua_pushnumber(luaState(), value);
    return (*this);
}

LuaStack& LuaStack::operator<<(const float& value)
{
    lua_pushnumber(luaState(), value);
    return (*this);
}

LuaStack& LuaStack::operator<<(const bool& b)
{
    lua_pushboolean(luaState(), b);
    return (*this);
}

void collectUserdata(LuaStack& stack)
{
    LuaUserdata* userdata = stack.as<LuaUserdata*>(1);
    userdata->~LuaUserdata();
}

LuaStack& LuaStack::push(const std::shared_ptr<void>& obj, QString type)
{
    return *this << LuaUserdata(obj, type);
}

LuaStack& LuaStack::push(void* const p, QString type)
{
    return *this << LuaUserdata(p, type);
}

LuaStack& LuaStack::operator<<(const LuaUserdata& userdata)
{
    void* luaUserdata = lua_newuserdata(luaState(), sizeof(LuaUserdata));
    new (luaUserdata) LuaUserdata(userdata);

    if (userdata.isRaw()) {
        _rawUserdata.push_back(static_cast<LuaUserdata*>(luaUserdata));
    }

    *this << lua::value::table;
    set("__gc", collectUserdata);
    setMetatable();

    return (*this);
}


LuaStack& LuaStack::pushPointer(void* const p)
{
    lua_pushlightuserdata(luaState(), p);
    return (*this);
}

LuaStack& LuaStack::operator<<(lua_CFunction func)
{
    return push(func, 0);
}

LuaStack& LuaStack::push(lua_CFunction func, const int closed)
{
    if (closed > 0) {
        checkPos(-closed);
    }

    lua_pushcclosure(luaState(), func, closed);

    return *this;
}

int collectRawCallable(lua_State* state)
{
    void* userdata = lua_touserdata(state, -1);
    lua::LuaCallable* callable = static_cast<lua::LuaCallable*>(userdata);
    using std::function;
    callable->~function();
    return 0;
}

LuaStack& LuaStack::operator<<(void(*func)(LuaStack& stack))
{
    return push(func, 0);
}

LuaStack& LuaStack::push(void(*func)(LuaStack& stack), const int closed)
{
    if (closed > 0) {
        checkPos(-closed);
    }

    void* ptr = lua_newuserdata(luaState(), sizeof(lua::LuaCallable));
    new (ptr) lua::LuaCallable(func);

    // Ensure the LuaCallable gets destructed when necessary.
    //
    *this << lua::value::table;
    set("__gc", collectRawCallable);
    setMetatable();

    pushPointer(&lua());

    // Invoke this twice to move both the Lua environment and the callable pointer to the top of the stack.
    lua_insert(luaState(), -2-closed);
    lua_insert(luaState(), -2-closed);

    push(invokeRawCallable, 2 + closed);
    return (*this);
}

LuaStack& LuaStack::operator<<(const lua::LuaCallable& f)
{
    return push(f, 0);
}

LuaStack& LuaStack::push(const lua::LuaCallable& f, const int closed)
{
    if (closed > 0) {
        checkPos(-closed);
    }

    *this << std::make_shared<lua::LuaCallable>(f);
    pushPointer(&lua());

    // Invoke this twice to move both the Lua environment and the callable pointer to the top of the stack.
    lua_insert(luaState(), -2-closed);
    lua_insert(luaState(), -2-closed);

    push(invokeLuaCallable, 2 + closed);
    return (*this);
}

LuaStack& LuaStack::operator<<(const LuaValue& value)
{
    value.push(*this);
    return *this;
}

LuaStack& LuaStack::operator<<(const LuaAccessible& value)
{
    value.push(*this);
    return *this;
}

LuaStack& LuaStack::operator<<(const lua::value& value)
{
    switch (value) {
        case lua::value::table:
            lua_newtable(luaState());
            break;
        case lua::value::nil:
            lua_pushnil(luaState());
            break;
    }
    return *this;
}

bool LuaStack::hasMetatable(const int pos)
{
    checkPos(pos);
    bool hasMeta = lua_getmetatable(luaState(), pos) != 0;
    if (hasMeta) {
        pop();
    }
    return hasMeta;
}

LuaStack& LuaStack::pushMetatable(const int pos)
{
    checkPos(pos);
    bool hasMeta = lua_getmetatable(luaState(), pos) != 0;
    if (!hasMeta) {
        *this << lua::value::table;
        // Offset to ensure the position is set correctly
        setMetatable(pos > 0 ? pos : pos - 1);
    }

    return *this;
}

LuaStack& LuaStack::setMetatable(const int pos)
{
    checkPos(pos);
    lua_setmetatable(luaState(), pos);
    return (*this);
}


bool LuaStack::isNil(const int pos)
{
    checkPos(pos);
    return lua_isnil(luaState(), pos) == 1;
}

LuaStack::~LuaStack()
{
    for (auto userdata : _rawUserdata) {
        userdata->reset();
    }
    if (size() > 0)
        lua_pop(luaState(), size());
}

int LuaStack::invokeCallable(lua_State* state, const lua::LuaCallable* const func)
{
    void* p = lua_touserdata(state, lua_upvalueindex(2));
    Lua* lua = static_cast<Lua*>(p);

    // Push all upvalues unto the stack.
    int upvalueIndex = 1;
    int i = 3;
    while (!lua_isnone(state, lua_upvalueindex(i))) {
        lua_pushvalue(state, lua_upvalueindex(i));
        lua_insert(state, upvalueIndex++);
        i++;
    }
    LuaStack stack(*lua);
    stack.grab();
    (*func)(stack);
    stack.disown();
    return lua_gettop(state);
}

int LuaStack::invokeRawCallable(lua_State* state)
{
    void* funcPtr = lua_touserdata(state, lua_upvalueindex(1));
    return invokeCallable(state, static_cast<lua::LuaCallable*>(funcPtr));
}

int LuaStack::invokeLuaCallable(lua_State* state)
{
    void* userdata = lua_touserdata(state, lua_upvalueindex(1));
    LuaUserdata* funcPtr = static_cast<LuaUserdata*>(userdata);
    return invokeCallable(state, static_cast<lua::LuaCallable*>(funcPtr->rawData()));
}

LuaIndex begin(LuaStack& stack)
{
    return stack.begin();
}

LuaIndex end(LuaStack& stack)
{
    return stack.end();
}

LuaIndex& operator>>(LuaIndex& index, LuaUserdata*& sink)
{
    index.stack().to(sink, index.pos());
    return ++index;
}

LuaIndex& operator>>(LuaIndex& index, const char*& sink)
{
    index.stack().to(sink, index.pos());
    return ++index;
}

LuaIndex& operator>>(LuaIndex& index, std::string& sink)
{
    sink = index.stack().as<const char*>(index.pos());
    return ++index;
}

LuaIndex& operator>>(LuaIndex& index, QString& sink)
{
    sink = index.stack().as<const char*>(index.pos());
    return ++index;
}

LuaIndex& operator>>(LuaIndex& index, QVariant& sink)
{
    LuaStack& stack = index.stack();
    const int pos(index.pos());
    switch(stack.type(pos)) {
    case lua::NIL:
        sink.clear();
        break;
    case lua::BOOLEAN:
        sink.setValue(stack.as<bool>(pos));
        break;
    case lua::NUMBER:
        sink.setValue(stack.as<double>(pos));
        break;
    case lua::STRING:
        sink.setValue(stack.as<QString>(pos));
        break;
    case lua::TABLE:
    case lua::FUNCTION:
    case lua::THREAD:
    default:
        throw LuaException(&stack.lua(), std::string("Type not supported: ") + stack.typestring(pos));
    }
    return ++index;
}

LuaStack& operator<<(LuaStack& stack, const QChar& value)
{
    return stack << value.toAscii();
}

LuaStack& operator<<(LuaStack& stack, const QString& value)
{
    return stack << value.toStdString();
}

LuaStack& operator<<(LuaStack& stack, const QVariant& variant)
{
    switch (variant.type()) {
    case QVariant::Invalid:
        return stack << lua::value::nil;
    case QVariant::Bool:
        return stack << variant.toBool();
    case QVariant::Char:
        return stack << variant.toChar();
    case QVariant::Int:
        return stack << variant.toInt();
    case QVariant::Double:
    case QVariant::UInt:
        return stack << variant.toDouble();
    default:
        break;
    }
    throw LuaException(&stack.lua(), std::string("Type not supported: ") + variant.typeName());
}
