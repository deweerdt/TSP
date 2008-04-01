SET(CMAKE_SYSTEM_NAME Rtems)

# specify the cross compiler
SET(CMAKE_C_COMPILER   /opt/rtems-4.7/bin/i386-rtems4.7-gcc)
SET(CMAKE_CXX_COMPILER /opt/rtems-4.7/bin/i386-rtems4.7-g++)
SET(CMAKE_AR /opt/rtems-4.7/bin/i386-rtems4.7-ar)

# where is the target environment
SET(CMAKE_FIND_ROOT_PATH  /opt/rtems-4.7)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(BUILD_SHARED_LIBS OFF)

INCLUDE_DIRECTORIES(/opt/rtems-4.7/i386-rtems4.7/pc386/lib/include)
