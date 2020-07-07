#pragma once

class ID3D11
{
public:
    ID3D11();

    virtual D3D_FEATURE_LEVEL GetMaxFeatureLevel();
    virtual bool GetMaxFeatureLevelStr(std::string& out);
private:
    D3D_FEATURE_LEVEL m_ftLevel;
};