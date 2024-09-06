set(SRC
    main.cpp
)

add_executable(stub WIN32 ${SRC})

set_target_properties(stub PROPERTIES
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED TRUE
)
