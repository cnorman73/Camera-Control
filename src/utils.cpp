#include "utils.h"
#include <chrono> // Include chrono library for time functions
#include <sstream> // Include stringstream
#include <iomanip> // Include to use put_time for formatting the date/time

// Note that this is called from a thread so it needs to be (is) thread safe
// This uses all thread safe functions including (POSIX-only) localtime_r
std::string Utils::getCurrentTimestamp() {
    // Get the current time
    auto now = std::chrono::system_clock::now();

    // Convert to time_t, potentially losing subsecond precision
    auto now_c = std::chrono::system_clock::to_time_t(now);

    // Convert to tm struct for formatting, using thread-safe function
    std::tm now_tm{};
    localtime_r(&now_c, &now_tm); // For POSIX
    // localtime_s(&now_tm, &now_c); // For Windows, swap the arguments

    // Get the current time again to calculate subsecond part (hundredths of a second)
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto duration = now_ms.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000; // milliseconds
    auto fraction = millis / 10; // Convert milliseconds to hundredths of a second, though requirement seems to suggest keeping it as millis for precision

    // Format the timestamp with subsecond precision in the format YYYY-MM-DDTHH:MM:SS.SS
    std::stringstream ss;
    ss << std::put_time(&now_tm, "%Y-%m-%dT%H:%M:%S.") << std::setw(2) << std::setfill('0') << fraction;

    return ss.str();
}
