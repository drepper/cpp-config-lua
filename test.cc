#include "config_lua.hh"


int test_fct(lua_State* state)
{
  auto s = lua_tostring(state, -1);
  auto p = getpwuid(getuid());
  lua_pushboolean(state, p != nullptr && strcmp(p->pw_name, s) == 0);
  return 1;
}


int
main(int argc, char* argv[])
{
  config::lua cf(argc == 1 ? DATADIR "/config" : argv[1], "foo", 1234, "bar", "some string", "test_fct", test_fct);

  cf.rewind(1);

  const char* names[] { "foo", "bar" };
  for (auto n : names)
    try {
      auto i = cf.get_integer(n);
      std::cout << n << ": integer " << i << std:: endl;
    } catch (...) {
      try {
        auto d = cf.get_double(n);
        std::cout << n << ": double " << d << std:: endl;
      } catch (...) {
        try {
          auto s = cf.get_string(n);
          std::cout << n << ": string " << s << std:: endl;
        } catch (const std::exception& e) {
          std::cout << e.what() << std::endl;
        }
      }
    }

  std::cout << "test_fct = " << cf.call_with_args<bool>("test_fct", "drepper") << std::endl;
}
