cmake_minimum_required(VERSION 3.21)
project(exlaunch_proj CXX C ASM)

if (NOT SWITCH)
    message(FATAL_ERROR "Not targeting switch, make sure to specify -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain.cmake")
endif ()

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

if (LOGGER_IP)
    add_compile_definitions(LOGGER_IP="${LOGGER_IP}")
endif ()

## Title Id
set(CONFIG_TITLE_ID 0x0100000000010000 CACHE INTERNAL "")
add_compile_definitions(EXL_PROGRAM_ID=${CONFIG_TITLE_ID} NNSDK=1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.json ${CMAKE_CURRENT_BINARY_DIR}/subsdk9.json)

## Project sources
include_directories(${PROJECT_SOURCE_DIR}/source/lib)
include_directories(${PROJECT_SOURCE_DIR}/include/nn)
include_directories(${PROJECT_SOURCE_DIR}/source/program)
include_directories(${PROJECT_SOURCE_DIR}/include/sead)
include_directories(${PROJECT_SOURCE_DIR}/source/)
include_directories(${PROJECT_SOURCE_DIR}/include/)
file(GLOB_RECURSE SOURCES_ASM ${PROJECT_SOURCE_DIR}/source/*.s)
file(GLOB_RECURSE SOURCES_H ${PROJECT_SOURCE_DIR}/source/*.h ${PROJECT_SOURCE_DIR}/source/*.hpp)
file(GLOB_RECURSE SOURCES_CXX ${PROJECT_SOURCE_DIR}/source/*.cpp ${PROJECT_SOURCE_DIR}/source/*.c)
file(GLOB_RECURSE LOCAL_LIBRARIES ${PROJECT_SOURCE_DIR}/libs/*.a)

## Include nx tools
include(${CMAKE_SOURCE_DIR}/cmake/SwitchTools.cmake)

## subsdk9
set(CMAKE_EXECUTABLE_SUFFIX ".elf")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
add_compile_options(-fPIC)
#set_source_files_properties(${SOURCES_CXX} PROPERTIES LANGUAGE CXX)
add_executable(subsdk9 ${SOURCES_ASM} ${SOURCES_C} ${SOURCES_H} ${SOURCES_CXX})
add_compile_definitions(EXL_LOAD_KIND=Module EXL_LOAD_KIND_ENUM=EXL_LOAD_KIND_MODULE)
target_link_libraries(subsdk9 ${LOCAL_LIBRARIES})

## Prepare subsdk ld
set(SUBSDK_LD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/misc/link.ld)
execute_process(COMMAND uname OUTPUT_VARIABLE uname)
if (uname MATCHES "^MINGW")
    string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" SUBSDK_LD_PATH "${SUBSDK_LD_PATH}")
endif ()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/misc/specs/module.specs ${CMAKE_CURRENT_BINARY_DIR}/subsdk9.specs)

## Build nso & generate npdm
add_nso_target_subsdk(subsdk9)
set_target_properties(subsdk9 PROPERTIES CONFIG_JSON "${CMAKE_CURRENT_BINARY_DIR}/subsdk9.json")
__generate_npdm(subsdk9)
set_target_properties(subsdk9 PROPERTIES LINK_FLAGS "-specs ${CMAKE_BINARY_DIR}/subsdk9.specs -g3 ${ARCH} -nostartfiles")

file(GLOB SUBSDKs "${CMAKE_CURRENT_BINARY_DIR}/subsdk[0-9]")
string(REGEX REPLACE "^0x" "" TARGET_TITLE_ID ${CONFIG_TITLE_ID})
if (FTP_IP)
    install(
            CODE
            "execute_process(COMMAND \"ncftpput -u ${FTP_USER} -p ${FTP_PASS} -P 5000 ${FTP_IP} /atmosphere/contents/${TARGET_TITLE_ID}/exefs ${CMAKE_BINARY_DIR}/main.npdm ${SUBSDKS}\")"
            COMPONENT FTPUpload
    )
    ## Upload to target switch
    #    add_custom_command(TARGET deploy_ftp POST_BUILD COMMAND ncftpput -u ${FTP_USER} -p ${FTP_PASS} -P 5000 ${FTP_IP} /atmosphere/contents/${TITLE_ID}/exefs ${CMAKE_BINARY_DIR}/main.npdm ${SUBSDKS})
endif ()

if (RYU_PATH)
    install(DIRECTORY DESTINATION ${RYU_PATH}/mods/contents/${TARGET_TITLE_ID}/mods COMPONENT RYUJINX)
    install(
            CODE
            "file(GLOB SUBSDKS ${CMAKE_CURRENT_BINARY_DIR}/subsdk[0-9])
            message(\${SUBSDKS})
            foreach(SUBSDK \${SUBSDKS})
                file(COPY \${SUBSDK} DESTINATION ${RYU_PATH}/mods/contents/${TARGET_TITLE_ID}/mods/)
            endforeach()
            file(COPY ${CMAKE_CURRENT_BINARY_DIR}/main.npdm DESTINATION ${RYU_PATH}/mods/contents/${TARGET_TITLE_ID}/mods/)"
            COMPONENT RYUJINX
    )
endif ()