# Read CIM classes out of MOF file
execute_process(COMMAND sed -e "/class/ !D" -e "s/class \\(.*\\):.*/\\1/g"
                INPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../mof/Linux_Fan.mof
                OUTPUT_VARIABLE CIM_CLASSES
)

# And fill list with them
string(REGEX MATCHALL "[a-zA-Z_-]+" CIM_CLASSES ${CIM_CLASSES})

# Get headers and sources names from the list of CIM classes
set(CIM_HEADERS "")
set(CIM_PROVIDERS "")
foreach(CLASS ${CIM_CLASSES})
    if (NOT ${CLASS} STREQUAL Linux_FanAssociatedSensor)
        set(CIM_HEADERS ${CIM_HEADERS} cmpi${CLASS}.h)
    endif(NOT ${CLASS} STREQUAL Linux_FanAssociatedSensor)
    set(PROVIDER cmpi${CLASS}Provider.c)
    set(CIM_PROVIDERS ${CIM_PROVIDERS} ${PROVIDER})
endforeach(CLASS ${CIM_CLASSES})

add_library(cmpiLinux_Fan SHARED
    Linux_Fan.h
    Linux_Fan.c
    cmpiLinux_Fan.h
    cmpiLinux_Fan.c
    cmpiLinux_FanCommon.h
    cmpiLinux_FanCommon.c
    cmpiLinux_FanSensor.h
    cmpiLinux_FanSensor.c
    ${CIM_PROVIDERS}
    ${CIM_HEADERS}
    )

# link cmpiOsBase_Common library
target_link_libraries(cmpiLinux_Fan "-lcmpiOSBase_Common")
# link sensors library
target_link_libraries(cmpiLinux_Fan "-lsensors")
include_directories(/usr/include/cmpi ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS cmpiLinux_Fan DESTINATION lib${LIB_SUFFIX}/cmpi)
