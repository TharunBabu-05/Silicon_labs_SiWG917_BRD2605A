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

# The installed AIML generator currently stops before contributing Wi-Fi to the
# generated CMake graph. Keep the SiWG917 Wi-Fi stack here until that generator
# defect is fixed, so this project remains buildable and reproducible.
set(TD_WISECONNECT_SDK "C:/Users/tharu/.silabs/slt/installs/conan/p/wiseca1ca655dfba28/p")
set(TD_WIFI_SOURCES
    "${SDK_PATH}/platform_core/platform/service/mem_pool/src/sl_mem_pool.c"
    "${TD_WISECONNECT_SDK}/components/common/src/malloc_thread_safety.c"
    "${TD_WISECONNECT_SDK}/components/common/src/sl_utility.c"
    "${TD_WISECONNECT_SDK}/components/device/stm32/silabs_utility/common/src/sl_string.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/ahb_interface/src/rsi_hal_mcu_m4_ram.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/ahb_interface/src/rsi_hal_mcu_m4_rom.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/ahb_interface/src/sl_platform.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/ahb_interface/src/sl_platform_wireless.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/ahb_interface/src/sl_si91x_bus.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/ahb_interface/src/sl_si91x_timer.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/ahb_interface/src/sli_siwx917_soc.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/errno/src/sl_si91x_errno.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/firmware_upgrade/firmware_upgradation.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/host_mcu/si91x/siwx917_soc_ncp_host.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/sl_net/src/sl_net_rsi_utility.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/sl_net/src/sl_net_si91x.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/sl_net/src/sl_net_si91x_callback_framework.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/sl_net/src/sl_net_si91x_integration_handler.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/sl_net/src/sl_si91x_net_internal_stack.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/sl_net/src/sli_net_si91x_utility.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/sli_hal_si91x/src/sli_hal_si91x.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/sli_si91x_wifi_command_engine/src/sli_si91x_wifi_command_engine.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/sli_si91x_wifi_command_engine/src/sli_si91x_wifi_command_engine_config.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/sli_si91x_wifi_event_handler/src/sli_si91x_wifi_event_handler.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/socket/src/sl_si91x_socket_utility.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/src/sl_rsi_utility.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/src/sl_si91x_driver.c"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/src/sli_si91x_driver.c"
    "${TD_WISECONNECT_SDK}/components/protocol/wifi/src/si91x/sl_wifi.c"
    "${TD_WISECONNECT_SDK}/components/protocol/wifi/src/sl_wifi_basic_credentials.c"
    "${TD_WISECONNECT_SDK}/components/protocol/wifi/src/sl_wifi_callback_framework.c"
    "${TD_WISECONNECT_SDK}/components/protocol/wifi/src/sli_wifi_callback_framework.c"
    "${TD_WISECONNECT_SDK}/components/service/bsd_socket/si91x_socket/sl_si91x_bsd_socket.c"
    "${TD_WISECONNECT_SDK}/components/service/network_manager/src/si91x/sl_net_credentials.c"
    "${TD_WISECONNECT_SDK}/components/service/network_manager/src/sl_net.c"
    "${TD_WISECONNECT_SDK}/components/service/network_manager/src/sl_net_basic_certificate_store.c"
    "${TD_WISECONNECT_SDK}/components/service/network_manager/src/sl_net_basic_profiles.c"
    "${TD_WISECONNECT_SDK}/components/service/network_manager/src/sli_net_common_utility.c"
    "${TD_WISECONNECT_SDK}/components/service/network_manager/src/sli_net_credentials.c"
    "${TD_WISECONNECT_SDK}/components/sli_buffer_manager/src/sli_buffer_manager.c"
    "${TD_WISECONNECT_SDK}/components/sli_command_engine/src/sli_command_engine.c"
    "${TD_WISECONNECT_SDK}/components/sli_event_engine/src/sli_event_engine.c"
    "${TD_WISECONNECT_SDK}/components/sli_queue_manager/src/sli_queue_manager.c"
    "${TD_WISECONNECT_SDK}/components/sli_routing_utility/src/sli_routing_utility.c"
    "${TD_WISECONNECT_SDK}/components/sli_wifi/src/sli_wifi.c"
    "${TD_WISECONNECT_SDK}/components/sli_wifi/src/sli_wifi_power_profile.c"
    "${TD_WISECONNECT_SDK}/components/sli_wifi/src/sli_wifi_utility.c"
)

target_sources(slc PRIVATE ${TD_WIFI_SOURCES})
target_include_directories(slc PUBLIC
    "${SDK_PATH}/platform_core/platform/common/inc"
    "${SDK_PATH}/platform_core/platform/service/mem_pool/inc"
    "${TD_WISECONNECT_SDK}/resources/defaults"
    "${TD_WISECONNECT_SDK}/components/common/inc"
    "${TD_WISECONNECT_SDK}/components/device/stm32/silabs_utility/common/inc"
    "${TD_WISECONNECT_SDK}/components/protocol/wifi/inc"
    "${TD_WISECONNECT_SDK}/components/service/bsd_socket/inc"
    "${TD_WISECONNECT_SDK}/components/service/bsd_socket/si91x_socket"
    "${TD_WISECONNECT_SDK}/components/service/network_manager/inc"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/ahb_interface/inc"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/errno/inc"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/firmware_upgrade"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/sl_net/inc"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/socket/inc"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/inc"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/sli_hal_si91x/inc"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/sli_si91x_wifi_command_engine/inc"
    "${TD_WISECONNECT_SDK}/components/device/silabs/si91x/wireless/sli_si91x_wifi_event_handler/inc"
    "${TD_WISECONNECT_SDK}/components/sli_buffer_manager/inc"
    "${TD_WISECONNECT_SDK}/components/sli_command_engine/inc"
    "${TD_WISECONNECT_SDK}/components/sli_event_engine/inc"
    "${TD_WISECONNECT_SDK}/components/sli_queue_manager/inc"
    "${TD_WISECONNECT_SDK}/components/sli_routing_utility/inc"
    "${TD_WISECONNECT_SDK}/components/sli_wifi/inc"
)
target_compile_definitions(slc PUBLIC
    SL_NET_COMPONENT_INCLUDED=1
    SL_WIFI_COMPONENT_INCLUDED=1
    SLI_SI91X_OFFLOAD_NETWORK_STACK=1
    SLI_SI91X_SOCKETS=1
    SL_CODE_COMPONENT_SI91X_WIRELESS=si91x_wireless
    SL_CODE_COMPONENT_SLI_HAL_SI91X=sli_hal_si91x
    SL_CODE_COMPONENT_SLI_ROUTING_UTILITY=sli_routing_utility
    SL_CODE_COMPONENT_SLI_SI91X_WIFI_CMD_ENGINE=sli_si91x_wifi_cmd_engine
)

add_executable(aiml_soc_voice_control_light_siwg917
    # TinyDesk platform adapter and core engine.
    ../tinydesk_siwg917.c
    ../tinydesk_siwg917_network.c
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
    ../tinydesk/apps/network.c
    ../tinydesk/apps/sysmon.c
)

target_include_directories(aiml_soc_voice_control_light_siwg917 PRIVATE
    ..
    ../tinydesk/include
    ../tinydesk/apps
    ../config
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
