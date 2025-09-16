# utils/CMakeLists.txt


add_library(utils STATIC
    utils.hpp utils.cpp
    cli.hpp cli.cpp
    root.hpp root.cpp
)

target_link_libraries(
    utils PUBLIC
    ROOT::Core ROOT::RIO ROOT::Tree ROOT::TreePlayer
    nlohmann_json::nlohmann_json
)
