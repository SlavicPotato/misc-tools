#pragma once

class IRegistry
{
public:
    IRegistry(HKEY hKey, const char* subkey, REGSAM access);
    virtual ~IRegistry() noexcept;

    virtual bool GetDWORD(const char* name, DWORD& out, DWORD def);
private:
    HKEY m_hKey;
};