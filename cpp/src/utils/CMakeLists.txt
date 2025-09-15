# utils/CMakeLists.txt


add_library(utils STATIC
    utils.hpp utils.cpp
    cli.hpp cli.cpp
    root.hpp root.cpp
)

target_link_libraries(
    utils INTERFACE
    ROOT::Core ROOT::RIO ROOT::Tree
    nlohmann_json::nlohmann_json
)
