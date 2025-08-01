
/**
 * @file utils.cpp
 * @brief Utility functions for formatting, host info, and timestamps.
 */
#include <chrono>
#include <format>
#include <string>
#include <stdexcept>
#include <vector>

#include <unistd.h>

#include "utils.hpp"

/**
 * @brief Returns a human-readable string for a byte size (GB, MB, KB, bytes).
 * @param numBytes Number of bytes.
 * @return Formatted size string.
 */
std::string getSizeString(size_t numBytes) {
    if (numBytes >= 1024 * 1024 * 1024) {
        return std::format("{:.2f} GB", numBytes / (1024.0 * 1024.0 * 1024.0));
    } else if (numBytes >= 1024 * 1024) {
        return std::format("{:.2f} MB", numBytes / (1024.0 * 1024.0));
    } else if (numBytes >= 1024) {
        return std::format("{:.2f} KB", numBytes / 1024.0);
    } else {
        return std::to_string(numBytes) + " bytes";
    }
}

/**
 * @brief Gets the hostname of the current machine.
 * @return Hostname string, or "unknown_host" if unavailable.
 */
std::string getHost() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return std::string(hostname);
    } else {
        return "unknown_host";
    }
}

/**
 * @brief Gets the current timestamp as a string.
 * @param filenameSafe If true, returns a filename-safe timestamp.
 * @return Timestamp string.
 */
std::string getTimestamp(bool filenameSafe) {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::tm tm = *std::localtime(&in_time_t);
    char buffer[100];

    if (filenameSafe) {
        // Replace spaces with underscores for filename safety
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &tm);
    } else {
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    }

    return std::string(buffer);
}

std::string timeMessage(const std::string& message) {
    return std::format("[{}] {}", getTimestamp(), message);
}