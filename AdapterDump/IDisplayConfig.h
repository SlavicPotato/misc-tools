#pragma once

class IDisplayConfig
{
public:
    IDisplayConfig(HMONITOR hMonitor);

    virtual bool GetMonitorName(std::wstring &out);
    virtual bool GetOutputTech(DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY &out);

    static const char* GetOutputTypeName(DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY vo);
private:
    HMONITOR m_hMonitor;

    LUID m_adapterID;
    UINT32 m_targetID;
}; 