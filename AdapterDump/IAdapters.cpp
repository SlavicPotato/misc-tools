#include "pch.h"

using namespace Microsoft::WRL;

IOutput::IOutput(ComPtr<IDXGIOutput>& pOutput)
    : m_pOutput(pOutput)
{
}

bool IOutput::QueryHardwareCompositionSupport(UINT& pFlags) const noexcept
{
    UINT flags;
    ComPtr<IDXGIOutput6> output6;

    if (FAILED(m_pOutput.As(&output6))) {
        return false;
    }

    if (FAILED(output6->CheckHardwareCompositionSupport(&flags))) {
        return false;
    }

    pFlags = flags;

    return true;
}

bool IOutput::GetDesc(DXGI_OUTPUT_DESC& out) const noexcept
{
    return SUCCEEDED(m_pOutput->GetDesc(&out));
}

IAdapter::IAdapter(ComPtr<IDXGIAdapter>& pAdapter, UINT index)
    : m_pAdapter(pAdapter), m_index(index)
{
}

void IAdapter::GetOutputs(OutputList& out) const
{
    for (UINT i = 0;; ++i)
    {
        ComPtr<IDXGIOutput> output;
        HRESULT res = m_pAdapter->EnumOutputs(i, &output);

        if (res == DXGI_ERROR_NOT_FOUND) {
            break;
        }

        ThrowIfFailed(res);

        out.push_back(IOutput(output));
    }
}

bool IAdapter::GetDesc(DXGI_ADAPTER_DESC& out) const noexcept
{
    return SUCCEEDED(m_pAdapter->GetDesc(&out));
}

UINT IAdapter::GetIndex() const noexcept
{
    return m_index;
}
