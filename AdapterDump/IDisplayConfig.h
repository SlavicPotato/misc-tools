#pragma once

class IDisplayConfig
{
public:
    IDisplayConfig(HMONITOR hMonitor);

    virtual void GetTargetInfo(DISPLAYCONFIG_PATH_TARGET_INFO& out) const noexcept;
    virtual bool GetMonitorName(std::wstring &out) const noexcept;

    static const char* GetOutputTechName(DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY vo) noexcept;
    static const char* GetScalingName(DISPLAYCONFIG_SCALING s) noexcept;
    static const char* GetScanlineOrderingName(DISPLAYCONFIG_SCANLINE_ORDERING so) noexcept;
    static const char* GetRotationName(DISPLAYCONFIG_ROTATION so) noexcept;
private:
    HMONITOR m_hMonitor;
    DISPLAYCONFIG_PATH_TARGET_INFO m_targetInfo;
}; 