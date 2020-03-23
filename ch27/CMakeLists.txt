add_executable(simple_lua simple_lua.c)
target_link_libraries(simple_lua lua53 dl m)

add_executable(example_stack example.c util.c)
target_link_libraries(example_stack lua53 dl m)