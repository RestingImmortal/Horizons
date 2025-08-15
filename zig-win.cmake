set(CMAKE_C_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/scripts/zig-cc")
set(CMAKE_CXX_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/scripts/zig-cxx")
set(CMAKE_LINKER "${CMAKE_CURRENT_SOURCE_DIR}/scripts/zig-cc")
set(CMAKE_AR "${CMAKE_CURRENT_SOURCE_DIR}/scripts/zig-ar")
set(CMAKE_RANLIB "${CMAKE_CURRENT_SOURCE_DIR}/scripts/zig-ranlib")
set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> ar cr x86_64-windows-gnu <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> ar cr x86_64-windows-gnu <OBJECTS>")
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> cc <FLAGS> <OBJECTS> -o x86_64-windows-gnu <LINK_LIBRARIES>")

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER_TARGET x86_64-windows-gnu)
set(CMAKE_CXX_COMPILER_TARGET x86_64-windows-gnu)
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -fuse-ld=lld -static -target x86_64-windows-gnu")
set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=lld -static -target x86_64-windows-gnu")

set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

