#include "log.hpp"

class StdioLog : public Log {
   public:
    void log(const LogLevel level, const char* msg,
             const char* const pos) override {
        printf("[%-5s %s] %s\n", this->level(level), pos, msg);
    }
    StdioLog() {}
};

Log& Log::logger = *(new Log());

void Log::log(const LogLevel level, const char* msg, const char* const pos) {
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

extern "C" void logstr(unsigned char level, const char* buffer,
                       const char* pos) {
    Log::instance().log(static_cast<LogLevel>(level), buffer, pos);
}