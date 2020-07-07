#pragma once

enum class DXGIVersion : uint32_t
{
    DXGI_VERSION_0 = 0,
    DXGI_VERSION_1,
    DXGI_VERSION_2,
    DXGI_VERSION_3,
    DXGI_VERSION_4,
    DXGI_VERSION_5,
    DXGI_VERSION_6,
    DXGI_VERSION_7
};

enum class DXGICaps : uint32_t
{
    DXGI_CAP_NONE = 0x00000000,
    DXGI_CAP_FLIP_SEQUENTIAL = 0x00000001,
    DXGI_CAP_FLIP_DISCARD = 0x00000002,
    DXGI_CAP_FLIP_BOTH = 0x00000003
};

class IDXGI
{
public:
    typedef std::vector<IAdapter> AdapterList;

    IDXGI();

    virtual DXGIVersion GetVersion() const;
    virtual void GetAdapters(AdapterList& out);
    virtual bool CheckFeatureSupport(DXGI_FEATURE Feature, void* pFeatureSupportData, UINT FeatureSupportDataSize);
    virtual bool HasCapability(DXGICaps cap) const;
    virtual DXGICaps GetCapabilities() const;
private:
    template <typename T>
    bool CheckFactory() {
        Microsoft::WRL::ComPtr<T> tmp;
        return SUCCEEDED(m_pFactory.As(&tmp));
    }

    Microsoft::WRL::ComPtr<IUnknown> m_pFactory;
    DXGIVersion m_version;
    DXGICaps m_caps;
};
