project(Server_Communications)

file(GLOB SOURCES "*.cpp" "*.h")

add_library(Server_Communications STATIC ${SOURCES})

set(Boost_USE_STATIC_LIBS ON)

#set(BOOST_ROOT "C:/Program Files/boost/boost_1_81_0")

find_package(Boost REQUIRED COMPONENTS system)

target_link_libraries(Server_Communications PRIVATE Interfaces Message_System Boost::system)

target_include_directories(Server_Communications PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../Interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/../Message_System
)


