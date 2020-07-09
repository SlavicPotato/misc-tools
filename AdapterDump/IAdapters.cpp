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

bool IOutput::GetDesc1(DXGI_OUTPUT_DESC1& out) const noexcept
{
    ComPtr<IDXGIOutput6> output6;
    if (FAILED(m_pOutput.As(&output6))) {
        return false;
    }

    return SUCCEEDED(output6->GetDesc1(&out));
}

const char* IOutput::GetColorSpaceName(DXGI_COLOR_SPACE_TYPE colorspace) noexcept
{
    switch (colorspace) {
    case DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709:
        return STR(DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709);
    case DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709:
        return STR(DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709);
    case DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P709:
        return STR(DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P709);
    case DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P2020:
        return STR(DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P2020);
    case DXGI_COLOR_SPACE_RESERVED:
        return STR(DXGI_COLOR_SPACE_RESERVED);
    case DXGI_COLOR_SPACE_YCBCR_FULL_G22_NONE_P709_X601:
        return STR(DXGI_COLOR_SPACE_YCBCR_FULL_G22_NONE_P709_X601);
    case DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P601:
        return STR(DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P601);
    case DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P601:
        return STR(DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P601);
    case DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P709:
        return STR(DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P709);
    case DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P709:
        return STR(DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P709);
    case DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P2020:
        return STR(DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P2020);
    case DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P2020:
        return STR(DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P2020);
    case DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020:
        return STR(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
    case DXGI_COLOR_SPACE_YCBCR_STUDIO_G2084_LEFT_P2020:
        return STR(DXGI_COLOR_SPACE_YCBCR_STUDIO_G2084_LEFT_P2020);
    case DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020:
        return STR(DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020);
    case DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_TOPLEFT_P2020:
        return STR(DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_TOPLEFT_P2020);
    case DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P2020:
        return STR(DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P2020);
    case DXGI_COLOR_SPACE_YCBCR_STUDIO_GHLG_TOPLEFT_P2020:
        return STR(DXGI_COLOR_SPACE_YCBCR_STUDIO_GHLG_TOPLEFT_P2020);
    case DXGI_COLOR_SPACE_YCBCR_FULL_GHLG_TOPLEFT_P2020:
        return STR(DXGI_COLOR_SPACE_YCBCR_FULL_GHLG_TOPLEFT_P2020);
    case DXGI_COLOR_SPACE_RGB_STUDIO_G24_NONE_P709:
        return STR(DXGI_COLOR_SPACE_RGB_STUDIO_G24_NONE_P709);
    case DXGI_COLOR_SPACE_RGB_STUDIO_G24_NONE_P2020:
        return STR(DXGI_COLOR_SPACE_RGB_STUDIO_G24_NONE_P2020);
    case DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_LEFT_P709:
        return STR(DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_LEFT_P709);
    case DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_LEFT_P2020:
        return STR(DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_LEFT_P2020);
    case DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_TOPLEFT_P2020:
        return STR(DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_TOPLEFT_P2020);
    case DXGI_COLOR_SPACE_CUSTOM:
        return STR(DXGI_COLOR_SPACE_CUSTOM);
    }

    return "UNKNOWN";
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
