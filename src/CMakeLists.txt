
set(HEADERS ${PROJECT_BINARY_DIR}/config.h
            header.h)

set(SOURCES header.h
            main.cpp
            NetworkSubsystem.cpp
            NetworkSubsystem.h
            TransportSocket.cpp
            TransportSocket.h
            Tuple.cpp
            Tuple.h)

set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/contrib
                 ${PROJECT_SOURCE_DIR}/src
                 ${PROJECT_BINARY_DIR})

if(USE_OPENSSL)
  add_definitions(-DUSE_SSL)
endif()

IF(WIN32)
  include_directories(${INCLUDE_DIRS} ${LIBOPENSSL_INCLUDE_DIR})
  link_directories(${LIBRARY_OUTPUT_PATH} ${LIBOPENSSL_LIB_DIR})
  add_definitions(-D__STDC_CONSTANT_MACROS -D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE)

  add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
  add_dependencies(${PROJECT_NAME} rutil)
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/MP") #Yes #Don¡¯t set means No
  target_link_libraries(${PROJECT_NAME} rutil.lib ares.lib ${LIBOPENSSL_LIBS} ws2_32.lib Winmm.lib)
ELSE()
  include_directories(${INCLUDE_DIRS} ${LIBOPENSSL_INCLUDE_DIR})
  link_directories(${LIBRARY_OUTPUT_PATH} ${LIBOPENSSL_LIB_DIR})
  set(CMAKE_CXX_FLAGS "-g -O0 -std=c++11 -Wall -Wno-write-strings -fpermissive")

  add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
  add_dependencies(${PROJECT_NAME} rutil)
  target_link_libraries(${PROJECT_NAME} rutil.a ares.a ${LIBOPENSSL_LIBS} pthread resolv)
ENDIF()
