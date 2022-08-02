#include "log.hpp"

class StdioLog : public Log {
   public:
    void log(const LogLevel level, const char* msg) override {
        printf("[%-5s] %s\n", this->level(level), msg);
    }
    StdioLog() {}
};

Log& Log::logger = *(new Log());

void Log::log(const LogLevel level, const char* msg) {
    // do nothing
}

constexpr const char* const Log::level(const LogLevel level) {
    switch (level) {
        case LogLevel::ERROR:
            return "Error";
        case LogLevel::WARN:
            return "Warn";
        case LogLevel::INFO:
            return "Info";
        case LogLevel::DEBUG:
            return "Debug";
        default:
            return "Other";
    }
}

void Log::set_stdio() {
    StdioLog* l = new StdioLog();
    l->set_global();
}

extern "C" void logstr(unsigned char level, const char* buffer) {
    Log::instance().log(static_cast<LogLevel>(level), buffer);
}