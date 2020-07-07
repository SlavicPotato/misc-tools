#include "pch.h"

static std::unordered_map<D3D_FEATURE_LEVEL, std::string> 
ftLevelToString = {
    {D3D_FEATURE_LEVEL_11_1, "11.1"},
    {D3D_FEATURE_LEVEL_11_0, "11.0"},
    {D3D_FEATURE_LEVEL_10_1, "10.1"},
    {D3D_FEATURE_LEVEL_10_0, "10.0"},
    {D3D_FEATURE_LEVEL_9_3, "9.3"},
    {D3D_FEATURE_LEVEL_9_2, "9.2"},
    {D3D_FEATURE_LEVEL_9_1, "9.1"},
    {D3D_FEATURE_LEVEL_1_0_CORE, "1.0"},
};

ID3D11::ID3D11()
    : m_ftLevel(D3D_FEATURE_LEVEL_1_0_CORE)
{
    static const D3D_FEATURE_LEVEL FeatureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    ThrowIfFailed(D3D11CreateDevice(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        0,
        FeatureLevels,
        ARRAYSIZE(FeatureLevels),
        D3D11_SDK_VERSION,
        NULL,
        &m_ftLevel,
        NULL
    ));
}

D3D_FEATURE_LEVEL ID3D11::GetMaxFeatureLevel()
{
    return m_ftLevel;
}

bool ID3D11::GetMaxFeatureLevelStr(std::string &out)
{
    auto it = ftLevelToString.find(m_ftLevel);
    if (it != ftLevelToString.end()) {
        out = it->second;
        return true;
    }

    return false;
}