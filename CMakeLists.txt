cmake_minimum_required(VERSION 3.12)
project(ubank CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Custom settings
set(USERVER_FORCE_DOWNLOAD_PACKAGES ON CACHE BOOL "" FORCE)
Set(USERVER_FEATURE_PATCH_LIBPQ OFF)
# Enable userver libraries that are needed in this project
set(USERVER_FEATURE_POSTGRESQL ON CACHE BOOL "" FORCE)

# Compatibility mode: some systems don't support these features
set(USERVER_FEATURE_CRYPTOPP_BLAKE2 OFF CACHE BOOL "" FORCE)
set(USERVER_FEATURE_GRPC_CHANNELZ OFF CACHE BOOL "" FORCE)
set(USERVER_FEATURE_REDIS_HI_MALLOC ON CACHE BOOL "" FORCE)


# Adding userver dependency
add_subdirectory(third_party/userver)   
userver_setup_environment()

add_library(${PROJECT_NAME}_client_objs OBJECT
    src/client_info.hpp
    src/client_info.cpp
    src/client.hpp
    src/client.cpp
)
target_link_libraries(${PROJECT_NAME}_client_objs PUBLIC userver-postgresql)

add_library(${PROJECT_NAME}_deposit_objs OBJECT
    src/deposit.hpp
    src/deposit.cpp
    src/deposit_info.hpp
    src/deposit_info.cpp
)
target_link_libraries(${PROJECT_NAME}_deposit_objs PUBLIC userver-postgresql)

# Common sources
#add_library(${PROJECT_NAME}_objs OBJECT
#    src/hello.hpp
#    src/hello.cpp
#)
#target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver-postgresql)


# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
#target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_client_objs)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_deposit_objs)

# Unit Tests
add_executable(${PROJECT_NAME}_unittest
    src/hello_test.cpp
)
target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_client_objs userver-utest)
add_google_tests(${PROJECT_NAME}_unittest)


# Benchmarks
add_executable(${PROJECT_NAME}_benchmark
    src/hello_benchmark.cpp
)
target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE ${PROJECT_NAME}_client_objs userver-ubench)
add_google_benchmark_tests(${PROJECT_NAME}_benchmark)


# Functional Tests
include(UserverTestsuite)

userver_testsuite_add_simple()


# Install
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
  message(STATUS "Set install prefix: $ENV{PREFIX}")
  file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
  set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
endif()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
