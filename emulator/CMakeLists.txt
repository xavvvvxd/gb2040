cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

option(PICO_BUILD "Build for RP2040/RP2350" OFF)
set(TEST_ROM "" CACHE FILEPATH "Path to a test ROM to embed in the Pico build")

file(GLOB_RECURSE CORE_SOURCES "src/core/*.cpp")

set(SHARED_SOURCES
    src/main.cpp
    ${CORE_SOURCES}
)

if (PICO_BUILD)
    if(WIN32)
        set(USERHOME $ENV{USERPROFILE})
    else()
        set(USERHOME $ENV{HOME})
    endif()
    set(sdkVersion 2.2.0)
    set(toolchainVersion 14_2_Rel1)
    set(picotoolVersion 2.2.0-a4)

    # Pull in Raspberry Pi Pico SDK (must be before project)
    include(pico_sdk_import.cmake)
endif()

project(gb2040 C CXX ASM)

if(PICO_BUILD)
    # ========= pico target =========

    pico_sdk_init()

    if(TEST_ROM)
        set(EMBEDDED_ROM_HEADER "${CMAKE_CURRENT_BINARY_DIR}/embedded_rom.h")
        find_package(Python3 REQUIRED COMPONENTS Interpreter)
        add_custom_command(
            OUTPUT ${EMBEDDED_ROM_HEADER}
            COMMAND ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/scripts/embed_rom.py
                ${TEST_ROM}
                ${EMBEDDED_ROM_HEADER}
            DEPENDS ${TEST_ROM}
            COMMENT "Embedding test ROM: ${TEST_ROM}"
        )
        add_custom_target(embedded_rom DEPENDS ${EMBEDDED_ROM_HEADER})
        message(STATUS "Test ROM will be embedded: ${TEST_ROM}")
    endif()

    add_executable(gb2040)

    pico_enable_stdio_usb(gb2040 1)

    # pico_set_binary_type(gb2040 no_flash)

    file(GLOB_RECURSE PIO_SOURCES "src/*.pio")
    pico_generate_pio_header(gb2040 ${PIO_SOURCES})

    file(GLOB_RECURSE PLATFORM_SOURCES "src/platform/pico/*.cpp")

    # setting global LTO results in a clash with the pico sdk for some reason
    set_source_files_properties(
        ${SHARED_SOURCES}
        ${PLATFORM_SOURCES}
        PROPERTIES
            COMPILE_OPTIONS "-flto"
    )

    # add rp2040 executable
    target_sources(gb2040 PRIVATE
        ${SHARED_SOURCES}
        ${PLATFORM_SOURCES}
    )

    if(TEST_ROM)
        add_dependencies(gb2040 embedded_rom)
        target_include_directories(gb2040 PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
        target_compile_definitions(gb2040 PRIVATE HAS_EMBEDDED_ROM=1)
    endif()

    # set up include dirs for pico target
    target_include_directories(gb2040
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    # pull in pico deps
    target_link_libraries(gb2040 pico_stdlib hardware_pio hardware_dma hardware_spi)

    target_compile_options(gb2040 PRIVATE
        -O3
        -ffast-math
        -funroll-loops

        $<$<COMPILE_LANGUAGE:CXX>:
            -fno-exceptions
            -fno-rtti
        >
    )

    target_link_options(gb2040 PRIVATE -flto)

    pico_add_extra_outputs(gb2040)
else()
    # ========= desktop target =========

    # add desktop executable
    add_executable(gb2040
        ${SHARED_SOURCES}
        src/platform/desktop/platform.cpp
    )

    # do the same for desktop, include SDL
    target_include_directories(gb2040
        PUBLIC 
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3/include
    )

    # pull in desktop deps
    target_link_libraries(gb2040
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3/lib/x64/SDL3.lib
    )

    target_link_options(gb2040 PRIVATE
        -static
    )

    # copy SDL3.dll next to desktop executable
    add_custom_command(TARGET gb2040 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3/lib/x64/SDL3.dll
        $<TARGET_FILE_DIR:gb2040>
    )

endif()
# add url via pico_set_program_url
