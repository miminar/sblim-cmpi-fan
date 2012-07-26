
cmake_minimum_required (VERSION 2.6)

set(CMAKE_C_FLAGS "-std=c99 -Wall -pedantic -g")

add_subdirectory(src)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(LIB_SUFFIX "")
else(CMAKE_SIZEOF_VOID_P EQUAL 4)
    SET(LIB_SUFFIX 64)
endif(CMAKE_SIZEOF_VOID_P EQUAL 4)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/src/Linux_Fan.registration
    DESTINATION share/sblim-cmpi-fan/)
install(FILES Linux_Fan.mof        DESTINATION share/sblim-cmpi-fan/)
install(FILES provider-register.sh DESTINATION share/sblim-cmpi-fan/)

add_custom_target(register
    ${CMAKE_CURRENT_SOURCE_DIR}/provider-register.sh
    -m ${CMAKE_CURRENT_SOURCE_DIR}/Linux_Fan.mof
    -r ${CMAKE_CURRENT_BINARY_DIR}/src/Linux_Fan.registration)
