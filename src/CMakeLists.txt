set(SRC_LIST cmm.cpp CMMLexer.cpp CMMParser.cpp CMMInterpreter.cpp
	             SourceMgr.cpp AST.cpp NativeFunctions.cpp)

add_executable(cmm ${SRC_LIST})

if (UNIX)
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIR})
    target_link_libraries(cmm ${CURSES_LIBRARIES})
endif (UNIX)

if (MSVC)
endif (MSVC)


set_property(TARGET cmm PROPERTY CXX_STANDARD 11)

set(CXX_STANDARD_REQUIRED on)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
