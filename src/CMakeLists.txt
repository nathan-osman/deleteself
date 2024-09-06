set(SRC
    deleteself.h
    deleteself.cpp
)

add_library(deleteself SHARED ${SRC})

set_target_properties(deleteself PROPERTIES
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED TRUE
    DEFINE_SYMBOL         DELETESELF_EXPORTS
)

target_include_directories(deleteself PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
