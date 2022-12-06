// Run CMake with -DCMAKE_BUILD_TYPE=Debug to turn on profiling

#ifndef NDEBUG
#define PROFILE_START std::chrono::system_clock::time_point profstart = std::chrono::system_clock::now();
#define PROFILE_END(label) \
    std::chrono::system_clock::time_point profend = std::chrono::system_clock::now(); \
    std::chrono::duration<double> elapsedSeconds = (profend - profstart); \
    std::cout << "Profiler " << label << ": " << elapsedSeconds.count() << "s\n";
#define PROFILE_INTERVAL(label) \
    static std::chrono::system_clock::time_point profintv = std::chrono::system_clock::now(); \
    std::chrono::system_clock::time_point profintvnow = std::chrono::system_clock::now(); \
    std::chrono::duration<double> elapsedSecondsIntv = (profintvnow - profintv); \
    profintv = profintvnow; \
    std::cout << "Profiler " << label << ": " << elapsedSecondsIntv.count() << "s (" << 1.0 / elapsedSecondsIntv.count() << " Hz)\n";
#else
#define PROFILE_START
#define PROFILE_END(label)
#define PROFILE_INTERVAL(label)
#endif