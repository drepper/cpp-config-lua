#include <algorithm>
#include <cstring>
#include <iostream>
#include <unordered_set>
#include <pwd.h>
#include <unistd.h>
extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}


namespace config {

  class lua {
    lua_State* state;
    std::unordered_set<std::string> known;
  public:
    template<typename... Args>
    lua(const char* fn, Args&&... args);
    ~lua() { lua_close(state); }

    void rewind(unsigned long long ts);

    const char* get_string(const char* str) { get_value(str, lua_isstring); auto retval = tostring(); pop(1); return retval; }
    double get_double(const char* str) { get_value(str, lua_isnumber); auto retval = tonumber(); pop(1); return retval; }
    long long get_integer(const char* str) { get_value(str, lua_isinteger); auto retval = tointeger(); pop(1); return retval; }

    template<typename R, typename... Args>
    auto call_with_args(const char* fctname, Args&&... args) { getglobal(fctname); push_args(std::forward<Args>(args)...); call(sizeof...(Args), 1); return pop_result<R>(); }

  private:
    template<typename T>
    struct propagate {
      typedef T type;
    };

    void initialize() {}
    template<typename T, typename... Args>
    void initialize(const char* name, T value, Args&&... args);

    template<typename T>
    void initialize1(const char* name, const T& value) { initialize1(name, typename propagate<T>::type(value)); }

    template<typename T, typename... Args>
    void push_args(const T& arg, Args&&... args) { push_arg(arg); push_args(std::forward<Args>(args)...); }
    void push_args() {}

    template<typename T>
    void push_arg(T);

    template<typename R>
    auto pop_result();

    void get_value(const char*, int (*)(lua_State*, int));

    void pushinteger(lua_Integer n) { lua_pushinteger(state, n); }
    void pushstring(const char* s) { lua_pushstring(state, s); }
    void pushstring(const std::string& s) { lua_pushstring(state, s.c_str()); }
    void pushboolean(bool b) { lua_pushboolean(state, b); }
    lua_Integer tointeger(int idx = -1) { return lua_tointeger(state, idx); }
    lua_Number tonumber(int idx = -1) { return lua_tonumber(state, idx); }
    const char* tostring(int idx = -1) { return lua_tostring(state, idx); }
    bool toboolean(int idx = -1) { return lua_toboolean(state, idx); }
    void pop(int idx = 1) { lua_pop(state, idx); }
    void getglobal(const char* k) { lua_getglobal(state, k); }
    void setglobal(const char* k) { lua_setglobal(state, k); }
    void settable(int idx) { lua_settable(state, idx); }
    void newtable() { lua_newtable(state); }
    void register_function(const char* n, lua_CFunction f) { lua_register(state, n, f); }
    void call(int nargs, int nresults) { lua_call(state, nargs, nresults); }
    int type(int idx = -1) { return lua_type(state, idx); }
    bool isnil(int idx = -1) { return lua_isnil(state, idx); }
    bool isfunction(int idx = -1) { return lua_isfunction(state, idx); }
  };

  template<>
  inline void lua::push_arg<const char*>(const char* s)
  {
    pushstring(s);
  }

  template<>
  inline auto lua::pop_result<bool>()
  {
    auto res = toboolean();
    pop();
    return res;
  }


  template<>
  struct lua::propagate<int> {
    typedef lua_Integer type;
  };


  template<typename... Args>
  inline lua::lua(const char* fn, Args&&... args)
    : state(luaL_newstate())
  {
    if (state == nullptr)
      throw std::runtime_error("cannot get config");
    // Allow script to use base library.
    luaL_openlibs(state);

    // If we cannot read the config file this is no failure, we simply start with an empty state.
    (void) luaL_dofile(state, fn);

    initialize(std::forward<Args>(args)...);
  }


  template<typename T, typename... Args>
  inline void lua::initialize(const char* name, T value, Args&&... args)
  {
    known.emplace(name);

    initialize1(name, value);

    initialize(std::forward<Args>(args)...);
  }

  template<>
  inline void lua::initialize1<lua_Integer>(const char* name, const lua_Integer& value)
  {
    getglobal(name);
    if (isnil()) {
      pushinteger(value);
      setglobal(name);
    }
    pop();
  }

  template<>
  inline void lua::initialize1<const char*>(const char* name, const char* const& value)
  {
    getglobal(name);
    if (isnil()) {
      pushstring(value);
      setglobal(name);
    }
    pop();
  }

  template<>
  inline void lua::initialize1<lua_CFunction>(const char* name, const lua_CFunction& value)
  {
    getglobal(name);
    if (isnil())
      register_function(name, value);
    pop();
  }

} // namespace config
