#pragma once

class IOutput
{
public:
    IOutput(Microsoft::WRL::ComPtr<IDXGIOutput>& pOutput);

    virtual bool QueryHardwareCompositionSupport(UINT &pFlags);
    virtual bool GetDesc(DXGI_OUTPUT_DESC& out);
private:
    Microsoft::WRL::ComPtr<IDXGIOutput> m_pOutput;
};

class IAdapter
{
public:
    typedef std::vector<IOutput> OutputList;

    IAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter>& pAdapter, UINT index);

    virtual void GetOutputs(OutputList& out);
    virtual bool GetDesc(DXGI_ADAPTER_DESC& out);

    virtual UINT GetIndex() const;
private:
    Microsoft::WRL::ComPtr<IDXGIAdapter> m_pAdapter;
    UINT m_index;
};
