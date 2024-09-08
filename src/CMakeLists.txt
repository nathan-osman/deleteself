set(SRC
    deleteself.h
    deleteself.cpp
)

add_library(deleteself STATIC ${SRC})

set_target_properties(deleteself PROPERTIES
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED TRUE
)

target_include_directories(deleteself PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(deleteself stubcpp)
