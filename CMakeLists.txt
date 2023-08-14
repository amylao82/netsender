
include_directories(${CMAKE_SOURCE_DIR}/include)
SET(LIBNETSENDER_SRC src/ip_utils.cpp  src/netsender.cpp  src/netsender_baseimpl.cpp  src/netsender_tcp_client.cpp  src/netsender_tcp_server.cpp  src/netsender_udp.cpp  src/protocol_general.cpp  src/protocol_interface.cpp  src/socketopt.cpp)
if(WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
endif()

ADD_LIBRARY(netsender STATIC ${LIBNETSENDER_SRC})

