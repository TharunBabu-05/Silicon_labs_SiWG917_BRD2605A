# Define minimal required version of CMake.
cmake_minimum_required(VERSION "3.25")

# Project definition
project(
	aiml_soc_voice_control_light_siwg917
	VERSION 1.0
	LANGUAGES C CXX ASM
)

# Include the definition of the slc_aiml_soc_voice_control_light_siwg917 target,
# which contains the content of the SLC project
include(aiml_soc_voice_control_light_siwg917.cmake)

add_executable(aiml_soc_voice_control_light_siwg917
    # TinyDesk platform adapter and core engine.
    ../tinydesk_siwg917.c
    ../tinydesk/src/draw.c
    ../tinydesk/src/event.c
    ../tinydesk/src/input.c
    ../tinydesk/src/render.c
    ../tinydesk/src/screen.c
    ../tinydesk/src/td.c
    ../tinydesk/src/theme.c
    ../tinydesk/src/timer.c
    ../tinydesk/src/utf8.c
    ../tinydesk/src/widgets.c
    ../tinydesk/src/wm.c
    ../tinydesk/apps/about.c
    ../tinydesk/apps/counter.c
    ../tinydesk/apps/sysmon.c
)

target_include_directories(aiml_soc_voice_control_light_siwg917 PRIVATE
    ..
    ../tinydesk/include
    ../tinydesk/apps
)

target_compile_definitions(aiml_soc_voice_control_light_siwg917 PRIVATE
    TD_SIWG917_PSRAM=1
    TD_MAX_COLS=100
    TD_MAX_ROWS=30
    TD_DEFAULT_COLS=100
    TD_DEFAULT_ROWS=30
    TD_OUT_BUF_SIZE=1024
    TD_PASTE_MAX=512
    TD_OSC52_MAX=0
    TD_VT_SCROLLBACK=0
    TD_MAX_WINDOWS=10
    TD_MAX_WIDGETS=80
    TD_MAX_TIMERS=16
    TD_MAX_APPS=8
    TD_SIWG917_TRACE=1
)

target_compile_options(aiml_soc_voice_control_light_siwg917 PUBLIC
    # Set additional compiler flags here
)

target_link_options(aiml_soc_voice_control_light_siwg917 PUBLIC
    # Set additional linker flags here
)

# Link with the content defined in the SLC project
target_link_libraries(aiml_soc_voice_control_light_siwg917 PRIVATE
    slc
    # This script comes after the generated SiWG917 memory-map script, so
    # its .tinydesk_psram section can use the declared PSRAM region.
    "-T${CMAKE_CURRENT_LIST_DIR}/../tinydesk_siwg917.ld"
)

# Include managed project content if available
include(aiml_soc_voice_control_light_siwg917_project.cmake OPTIONAL RESULT_VARIABLE managed_project)
if(managed_project)
    message(STATUS "Using managed project content from ${managed_project}")
endif()

# Force the gcc linker command
set_target_properties(aiml_soc_voice_control_light_siwg917 PROPERTIES LINKER_LANGUAGE C)

# Create .bin, .hex and .s37 artifacts after building the project
add_custom_command(TARGET aiml_soc_voice_control_light_siwg917
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_SREC_CMD} "$<TARGET_FILE:aiml_soc_voice_control_light_siwg917>" "$<TARGET_FILE_DIR:aiml_soc_voice_control_light_siwg917>/$<TARGET_FILE_BASE_NAME:aiml_soc_voice_control_light_siwg917>.s37"
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_IHEX_CMD} "$<TARGET_FILE:aiml_soc_voice_control_light_siwg917>" "$<TARGET_FILE_DIR:aiml_soc_voice_control_light_siwg917>/$<TARGET_FILE_BASE_NAME:aiml_soc_voice_control_light_siwg917>.hex"
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_BIN_CMD}  "$<TARGET_FILE:aiml_soc_voice_control_light_siwg917>" "$<TARGET_FILE_DIR:aiml_soc_voice_control_light_siwg917>/$<TARGET_FILE_BASE_NAME:aiml_soc_voice_control_light_siwg917>.bin" 
)

# Run post-build pipeline to perform additional post-processing
if(post_build_command)
add_custom_command(TARGET aiml_soc_voice_control_light_siwg917
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
    COMMAND ${post_build_command}
)
endif()
