set(SRC
    main.cpp
)

add_executable(test WIN32 ${SRC})

target_link_libraries(test deleteself)
