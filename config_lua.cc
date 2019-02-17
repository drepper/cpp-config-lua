#include "config_lua.hh"


void config::lua::rewind(unsigned long long ts)
{
  getglobal("load_history");
  if (isfunction()){
    // There is a load_history function.
    pushinteger(ts);

    newtable();
    unsigned cnt = 0;
    std::for_each(known.begin(), known.end(),
                  [&cnt, this] (const auto& name) {
                    this->pushinteger(++cnt);
                    this->pushstring(name);
                    this->settable(-3);
                  });

    call(2, 1);
    pop();
  }
}


void config::lua::get_value(const char* str, int (*typechk)(lua_State*, int))
{
  getglobal(str);
  auto t = type();
  while (isfunction()) {
    call(0, 1);
    t = type();
  }

  if (typechk(state, -1))
    return;

  // Error handling.
  pop();
  if (t == LUA_TNIL)
    throw std::out_of_range("undefined value");
  else
    throw std::invalid_argument("unexpected value");
}
