
/**
 * @file cli.hpp
 * @brief Declarations for command-line argument parsing and argument struct.
 */
#pragma once

#include <string>

/**
 * @struct Args
 * @brief Structure to hold parsed command-line arguments and their default values.
 *
 * Members:
 *   data       Path to .root file containing data to compress.
 *   config     Path to .json file containing benchmark configuration.
 */
struct Args {
    std::string data{};
    std::string config{};
};

/**
 * @brief Parse command-line arguments into an Args struct.
 * @param argc Number of command-line arguments.
 * @param argv Array of argument strings.
 * @return Args struct populated with parsed values.
 */
const Args parseArgs(int argc, char* argv[]);