#pragma once

enum class DXGIVersion : uint32_t
{
    VERSION_0,
    VERSION_1,
    VERSION_2,
    VERSION_3,
    VERSION_4,
    VERSION_5,
    VERSION_6,
    VERSION_7
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
