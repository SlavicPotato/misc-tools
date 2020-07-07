#pragma once

enum class DXGIVersion : uint32_t
{
    V_0,
    V_1,
    V_2,
    V_3,
    V_4,
    V_5,
    V_6,
    V_7
};

enum class DXGICaps : uint32_t
{
    NONE = 0,
    FLIP_SEQUENTIAL = 1 << 0,
    FLIP_DISCARD = 1 << 1,
    FLIP_BOTH = 3
};

class IDXGI
{
public:
    typedef std::vector<IAdapter> AdapterList;

    IDXGI();

    virtual DXGIVersion GetVersion() const;
    virtual void GetAdapters(AdapterList& out) const;
    virtual bool CheckFeatureSupport(DXGI_FEATURE Feature, void* pFeatureSupportData, UINT FeatureSupportDataSize) const noexcept;
    virtual bool HasCapability(DXGICaps cap) const noexcept;
    virtual DXGICaps GetCapabilities() const noexcept;
private:
    template <typename T>
    bool CheckFactory() const noexcept {
        Microsoft::WRL::ComPtr<T> tmp;
        return SUCCEEDED(m_pFactory.As(&tmp));
    }

    Microsoft::WRL::ComPtr<IDXGIFactory> m_pFactory;
    DXGIVersion m_version;
    DXGICaps m_caps;
};
