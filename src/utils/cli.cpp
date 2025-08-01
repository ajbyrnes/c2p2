
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
        if (arg == "--input-file") {
            if (i + 1 < argc) {
                args.inputFile = argv[++i];
            } else {
                throw std::runtime_error("Error: --input-file requires a value");
            }
        }
        else if (arg == "--tree") {
            if (i + 1 < argc) {
                args.treename = argv[++i];
            } else {
                throw std::runtime_error("Error: --tree requires a value");
            }
        }
        else if (arg == "--branch") {
            if (i + 1 < argc) {
                args.branchname = argv[++i];
            } else {
                throw std::runtime_error("Error: --branch requires a value");
            }
        }
        else if (arg == "--max-bytes") {
            if (i + 1 < argc) {
                args.maxBytes = std::stoul(argv[++i]);
            } else {
                throw std::runtime_error("Error: --max-bytes requires a value");
            }
        }
        else if (arg == "--compressor") {
            if (i + 1 < argc) {
                args.compressor = argv[++i];
            } else {
                throw std::runtime_error("Error: --compressor requires a value");
            }
        }
        else if (arg == "--iterations") {
            if (i + 1 < argc) {
                args.iterations = std::stoi(argv[++i]);
            } else {
                throw std::runtime_error("Error: --iterations requires a value");
            }
        }
        else if (arg == "--mantissa-bits") {
            if (i + 1 < argc) {
                args.mantissaBits = std::stoi(argv[++i]);
            } else {
                throw std::runtime_error("Error: --mantissa-bits requires a value");
            }
        } else if (arg == "--compression-level") {
            if (i + 1 < argc) {
                args.compressionLevel = std::stoi(argv[++i]);
            } else {
                throw std::runtime_error("Error: --compression-level requires a value");
            }
        } else {
            throw std::runtime_error("Unknown argument: " + arg);
        }
    }

    return args;
}