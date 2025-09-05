
/**
 * @file utils.hpp
 * @brief Utility function declarations for host info, formatting, and timestamps.
 */
#pragma once

#include <string>

/**
 * @brief Gets the hostname of the current machine.
 * @return Hostname string, or "unknown_host" if unavailable.
 */
std::string getHost();

/**
 * @brief Returns a human-readable string for a byte size (GB, MB, KB, bytes).
 * @param numBytes Number of bytes.
 * @return Formatted size string.
 */
std::string getSizeString(size_t numBytes);

/**
 * @brief Gets the current timestamp as a string.
 * @param filenameSafe If true, returns a filename-safe timestamp.
 * @return Timestamp string.
 */
std::string getTimestamp(bool filenameSafe = false);

/**
 * @brief Prepends a timestamp to a message string.
 * @param message Message to prepend.
 * @return Timestamped message string.
 */
std::string timeMessage(const std::string& message);