# author Rudy Castan
# date 2025 Fall
# CS200 Computer Graphics I
# copyright DigiPen Institute of Technology

add_library(the_openal INTERFACE)

if(WIN32)
# download binaries from OpenAL Soft for windows x64
    FetchContent_Declare(
        openalsoft
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL "https://github.com/kcat/openal-soft/releases/download/1.24.3/openal-soft-1.24.3-bin.zip"
        URL_HASH MD5=2e035d4dac29da6d9481fe68e3af881a
    )
    FetchContent_MakeAvailable(openalsoft)

    target_include_directories(the_openal SYSTEM INTERFACE ${openalsoft_SOURCE_DIR}/include/AL)
    target_link_directories(the_openal INTERFACE ${openalsoft_SOURCE_DIR}/libs/Win64)
    target_link_libraries(the_openal INTERFACE OpenAL32)

    # Define a custom target to copy OpenAL DLL files to the build directory
    set(TEMP_EXE_FOLDER $<IF:$<BOOL:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}>,${CMAKE_RUNTIME_OUTPUT_DIRECTORY},${CMAKE_BINARY_DIR}>)

    add_custom_target(copy_openal_dll
        COMMAND ${CMAKE_COMMAND} -E copy
                ${openalsoft_SOURCE_DIR}/bin/Win64/soft_oal.dll
                ${TEMP_EXE_FOLDER}/soft_oal.dll
        COMMAND ${CMAKE_COMMAND} -E copy
                ${openalsoft_SOURCE_DIR}/router/Win64/OpenAL32.dll
                ${TEMP_EXE_FOLDER}/OpenAL32.dll
        DEPENDS ${openalsoft_SOURCE_DIR}/bin/Win64/soft_oal.dll ${openalsoft_SOURCE_DIR}/router/Win64/OpenAL32.dll
        COMMENT "Copying OpenAL Soft DLL files to executable directory"
    )
    add_dependencies(the_openal copy_openal_dll)

else()
    # Debian Linux : sudo apt install libopenal-dev
    # MacOS : brew install openal-soft
    find_package(OpenAL REQUIRED)
    target_link_libraries(the_openal INTERFACE ${OPENAL_LIBRARY})
    target_include_directories(the_openal SYSTEM INTERFACE ${OPENAL_INCLUDE_DIR})

    if(EMSCRIPTEN)
        # Emscripten has their own implementation of OpenAL, we just need to add the linker flag and update the include path
        # https://emscripten.org/docs/porting/Audio.html
        target_include_directories(the_openal SYSTEM INTERFACE ${OPENAL_INCLUDE_DIR}/AL)
        target_link_options(the_openal INTERFACE -lopenal)
    endif()

endif()
