cmake_minimum_required(VERSION 3.16)
project(SteamAnalyzer VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    http.cpp
    steam_api.cpp
    report.cpp
    server.cpp
    api_handler.cpp
    providers/provider_registry.cpp
    providers/provider_report.cpp
    providers/steam_provider.cpp
)

add_executable(game_analyzer ${SOURCES})

target_include_directories(game_analyzer PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/providers
)

find_package(CURL REQUIRED)
target_link_libraries(game_analyzer PRIVATE CURL::libcurl)

if(WIN32)
    target_link_libraries(game_analyzer PRIVATE ws2_32)
endif()

find_package(nlohmann_json QUIET)
if(nlohmann_json_FOUND)
    target_link_libraries(game_analyzer PRIVATE nlohmann_json::nlohmann_json)
else()
    target_include_directories(game_analyzer PRIVATE ${CMAKE_SOURCE_DIR})
endif()

add_custom_command(TARGET game_analyzer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/web
        $<TARGET_FILE_DIR:game_analyzer>/web
    COMMENT "Copying web/ to output..."
)

target_compile_options(game_analyzer PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /wd4996>
)
