#pragma once

class ID3D11
{
public:
    ID3D11();

    virtual D3D_FEATURE_LEVEL GetMaxFeatureLevel() const noexcept;
    virtual bool GetMaxFeatureLevelStr(std::string& out) const noexcept;
private:
    D3D_FEATURE_LEVEL m_ftLevel;
};