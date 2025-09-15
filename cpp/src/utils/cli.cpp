
/**
 * @file cli.cpp
 * @brief Command-line argument parsing implementation for the utils module.
 *
 * Provides a function to parse command-line arguments into an Args struct.
 */
#include <stdexcept>

#include "cli.hpp"

/**
 * @brief Parse command-line arguments into an Args struct.
 *
 * Supported arguments:
 *   --input-file <file>
 *   --tree <name>
 *   --branch <name>
 *   --max-bytes <number>
 *   --compressor <name>
 *   --iterations <number>
 *   --mantissa-bits <number>
 *   --compression-level <number>
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of argument strings.
 * @return Args struct populated with parsed values.
 * @throws std::runtime_error if required values are missing or unknown arguments are provided.
 */
const Args parseArgs(int argc, char* argv[]) {
    Args args;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--data") {
            if (i + 1 < argc) {
                args.data = argv[++i];
            } else {
                throw std::runtime_error("Error: --data requires a value");
            }
        }
        else if (arg == "--config") {
            if (i + 1 < argc) {
                args.config = argv[++i];
            } else {
                throw std::runtime_error("Error: --config requires a value");
            }
        } else {
            throw std::runtime_error("Unknown argument: " + arg);
        }
    }

    return args;
}