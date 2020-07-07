#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN  

#include <windows.h>
#include <stdio.h>

#include <vector>
#include <string>

#include <wrl/client.h>

#include <dxgi1_6.h>

#include "helpers.h"

#include "IAdapters.h"
#include "IRegistry.h"
#include "IDXGI.h"

template <class T, class = std::enable_if_t<is_any_of<T, DXGICaps>::value>>
inline T operator|(T lhs, T rhs)
{
    return static_cast<T>(Misc::Underlying(lhs) | Misc::Underlying(rhs));
}

template <class T, class = std::enable_if_t<is_any_of<T, DXGICaps>::value>>
inline T operator&(T lhs, T rhs)
{
    return static_cast<T>(Misc::Underlying(lhs) & Misc::Underlying(rhs));
}

#endif //PCH_H
