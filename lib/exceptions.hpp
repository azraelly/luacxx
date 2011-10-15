#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

using namespace std;

class LuaException : public std::runtime_error
{
private:
	const Lua& lua;
public:
	LuaException(const Lua& lua, const char* what) : std::runtime_error(what), lua(lua) {}
};

#endif // EXCEPTIONS_H