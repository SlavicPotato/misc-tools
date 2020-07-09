#pragma once

class IDisplayConfig
{
public:
    IDisplayConfig(HMONITOR hMonitor);

    virtual void GetTargetInfo(DISPLAYCONFIG_PATH_TARGET_INFO& out);
    virtual bool GetMonitorName(std::wstring &out);

    static const char* GetOutputTechName(DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY vo);
    static const char* GetScalingName(DISPLAYCONFIG_SCALING s);
    static const char* GetScanlineOrderingName(DISPLAYCONFIG_SCANLINE_ORDERING so);
    static const char* GetRotationName(DISPLAYCONFIG_ROTATION so);
private:
    HMONITOR m_hMonitor;

    DISPLAYCONFIG_PATH_TARGET_INFO m_targetInfo;
}; 