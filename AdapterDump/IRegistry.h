#pragma once

class IRegistry
{
public:
    IRegistry();
    IRegistry(HKEY hKey, const char* subkey, REGSAM access);
    virtual ~IRegistry() noexcept;

    IRegistry& operator=(const IRegistry& rhs) {
        _copy(rhs);
        return *this;
    }

    IRegistry(const IRegistry& rhs)
    {
        _copy(rhs);
    }

    virtual bool GetDWORD(const char* name, DWORD& out, DWORD def) const noexcept;
private:
    void _copy(const IRegistry& rhs)
    {
        m_hKey = rhs.m_hKey;
        const_cast<IRegistry&>(rhs).m_hKey = NULL;
    }

    HKEY m_hKey;
};