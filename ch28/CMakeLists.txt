add_executable(example_window window.c ${PROJECT_SOURCE_DIR}/ch27/util.c)
target_link_libraries(example_window lua53 dl m)