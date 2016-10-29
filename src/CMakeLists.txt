set(SRC_LIST cmm.cpp CMMLexer.cpp CMMParser.cpp SourceMgr.cpp)
add_executable(cmm ${SRC_LIST})

set_property(TARGET cmm PROPERTY CXX_STANDARD 11)

set(CXX_STANDARD_REQUIRED on)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
