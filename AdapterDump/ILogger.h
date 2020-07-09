#pragma once

#include <fstream>

class ILogger
{
public:

    ILogger(bool print_stdout);
    
    bool Open(const char* path);
    void Close();
    void Message(const char* fmt, ...);

private:
    std::ofstream ofs;
    char m_buf[0x2000];
    bool m_isOpen;
    bool m_printStdout;
};