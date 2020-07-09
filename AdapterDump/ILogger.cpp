#include "pch.h"

ILogger::ILogger(bool print_stdout) :
    m_printStdout(print_stdout),
    m_isOpen(false)
{
}

bool ILogger::Open(const char* path)
{
    if (ofs.is_open()) {
        return false;
    }

    ofs.open(path, std::ios_base::trunc);
    return (m_isOpen = ofs.is_open());
}

void ILogger::Close()
{
    if (ofs.is_open()) {
        ofs.close();
    }
}

void ILogger::Message(const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    if (ofs.is_open()) {
        _vsnprintf_s(m_buf, _TRUNCATE, fmt, args);
        ofs << m_buf << std::endl;
    }

    if (m_printStdout) {
        vprintf(fmt, args);
        putc('\n', stdout);
    }

    va_end(args);
}