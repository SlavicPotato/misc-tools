#pragma once

class IOutput
{
public:
    IOutput(Microsoft::WRL::ComPtr<IDXGIOutput>& pOutput);

    virtual bool QueryHardwareCompositionSupport(UINT& pFlags) const noexcept;
    virtual bool GetDesc(DXGI_OUTPUT_DESC& out) const noexcept;
private:
    Microsoft::WRL::ComPtr<IDXGIOutput> m_pOutput;
};

class IAdapter
{
public:
    typedef std::vector<IOutput> OutputList;

    IAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter>& pAdapter, UINT index);

    virtual void GetOutputs(OutputList& out) const;
    virtual bool GetDesc(DXGI_ADAPTER_DESC& out) const noexcept;
    virtual UINT GetIndex() const noexcept;
private:
    Microsoft::WRL::ComPtr<IDXGIAdapter> m_pAdapter;
    UINT m_index;
};
