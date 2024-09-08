# The stub is loaded into memory as a new process and waits for the calling
# process to terminate, at which point it will delete the image on disk

# Because deleteself is distributed as a static library, resource files
# cannot be used and the assembled executable must be declared as a static
# array of bytes

# CMake can directly invoke an assembler BUT there is no easy way to force it
# to explicitly compile and link a 32-bit application; hence we do the
# assembly process ourselves

add_custom_command(
    OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/stub.exe
    COMMAND ${NASM} -f bin -o ${CMAKE_CURRENT_BINARY_DIR}/stub.exe ${CMAKE_CURRENT_SOURCE_DIR}/main.asm
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/main.asm
    COMMENT "Assembling stub"
)

add_custom_target(stubexe
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.asm
)

# Now the assembled EXE must be converted to an array of bytes using a utility
# program written for this purpose

set(SRC
    main.cpp
)

add_executable(stubconv ${SRC})

add_custom_command(
    OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/stub.cpp
    COMMAND stubconv ${CMAKE_CURRENT_BINARY_DIR}/stub.exe ${CMAKE_CURRENT_BINARY_DIR}/stub.cpp
    DEPENDS stubconv ${CMAKE_CURRENT_BINARY_DIR}/stub.exe
    COMMENT "Converting stub"
)

add_custom_target(stubcpp
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/stub.cpp
)
