cmake_minimum_required (VERSION 2.8.8)
project (HTTPServer)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

include_directories(.)

find_package(Threads REQUIRED)

SET (BOOST_ROOT "~/boost_1_63_0")

set(BOOST_COMPONENTS system thread filesystem date_time)
# Late 2017 TODO: remove the following checks and always use std::regex
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
        set(BOOST_COMPONENTS ${BOOST_COMPONENTS} regex)
        message("legacy GCC detected: boost regex")
        add_definitions(-DUSE_BOOST_REGEX)
    endif()
endif()
find_package(Boost 1.63.0 COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
if(Boost_MINOR_VERSION LESS 58)
    message("legacy boost detected: using TLS 1.2 workaround")
    add_definitions(-DBOOST_TLS12_FALLBACK)
 endif()
include_directories(SYSTEM ${Boost_INCLUDE_DIR})

set(OPENSSL_ROOT_DIR "/usr/include/openssl")
if(APPLE)
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
endif()

set(libmongocxx_DIR "~/mongo-cxx-driver-r3.0.3/lib/cmake/libmongocxx-3.0.3/")
set(libbsoncxx_DIR "~/mongo-cxx-driver-r3.0.3/lib/cmake/libbsoncxx-3.0.3/")

find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)
include_directories(${LIBMONGOCXX_INCLUDE_DIR})
include_directories(${LIBBSONCXX_INCLUDE_DIR})
include_directories("~/mongo-cxx-driver-r3.0.3/include/mongocxx/v_noabi")
include_directories("~/mongo-cxx-driver-r3.0.3/include/bsoncxx/v_noabi")
include_directories("~/mongo-c-driver/include/libmongoc-1.0")
include_directories("~/mongo-c-driver/include/libbson-1.0")

link_directories(${LIBMONGOCXX_LIBRARY_DIRS} ${LIBBSONCXX_LIBRARY_DIRS})

#TODO: add requirement for version 1.0.1g (can it be done in one line?)
find_package(OpenSSL)

if(OPENSSL_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_OPENSSL")
    include_directories(SYSTEM ${OPENSSL_INCLUDE_DIR})

    #manually add the sources
    set(SOURCES ./HTTPServer.cpp
		./structures/Comment.cpp
		./structures/Community.cpp
		./structures/Post.cpp
		./structures/User.cpp
		./structures/Utlityfunctions.cpp
	)

    add_executable(HTTPServer ${SOURCES})
    target_link_libraries(HTTPServer ${Boost_LIBRARIES})
    target_link_libraries(HTTPServer ${OPENSSL_LIBRARIES})
    target_link_libraries(HTTPServer ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(HTTPServer ${LIBMONGOCXX_LIBRARIES})
    target_link_libraries(HTTPServer ${LIBBSONCXX_LIBRARIES})


endif()

if(MSYS) #TODO: Is MSYS true when MSVC is true?
    if(OPENSSL_FOUND)
        target_link_libraries(HTTPServer ws2_32 wsock32)
    endif()
endif()

enable_testing()

install(FILES server_http.hpp client_http.hpp server_https.hpp client_https.hpp database.h auth.h DESTINATION include/CBB)

