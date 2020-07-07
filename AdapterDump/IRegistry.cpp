#include "pch.h"

IRegistry::IRegistry()
    : m_hKey(NULL)
{
}

IRegistry::IRegistry(HKEY hKey, const char* subkey, REGSAM access)
{
    ThrowIfFailed2(RegOpenKeyExA(hKey, subkey, 0, access, &m_hKey));
}

IRegistry::~IRegistry() noexcept
{
    if (m_hKey != NULL) {
        RegCloseKey(m_hKey);
    }
}

bool IRegistry::GetDWORD(const char* name, DWORD& out, DWORD def) const noexcept
{

    DWORD result;
    DWORD size = sizeof(result);
    DWORD type;

    LSTATUS r = RegQueryValueExA(
        m_hKey,
        name,
        0,
        &type,
        reinterpret_cast<BYTE*>(&result),
        &size);

    if (r != ERROR_SUCCESS || type != REG_DWORD) {
        out = def;
        return false;
    }

    out = result;
    return true;
}