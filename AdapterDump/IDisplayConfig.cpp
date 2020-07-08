#include "pch.h"

IDisplayConfig::IDisplayConfig(HMONITOR hMonitor)
    :m_hMonitor(hMonitor)
{
    MONITORINFOEXW info;
    info.cbSize = sizeof(info);
    ThrowIfFailed3(GetMonitorInfoW(hMonitor, &info));

    UINT32 requiredPaths, requiredModes;
    ThrowIfFailed2(GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &requiredPaths, &requiredModes));
    std::vector<DISPLAYCONFIG_PATH_INFO> paths(requiredPaths);
    std::vector<DISPLAYCONFIG_MODE_INFO> modes(requiredModes);
    ThrowIfFailed2(QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &requiredPaths, paths.data(), &requiredModes, modes.data(), nullptr));

    for (auto& p : paths)
    {
        DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceName;
        sourceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        sourceName.header.size = sizeof(sourceName);
        sourceName.header.adapterId = p.sourceInfo.adapterId;
        sourceName.header.id = p.sourceInfo.id;
        ThrowIfFailed2(DisplayConfigGetDeviceInfo(&sourceName.header));

        if (wcscmp(info.szDevice, sourceName.viewGdiDeviceName) == 0) {
            m_adapterID = p.sourceInfo.adapterId;
            m_targetID = p.targetInfo.id;

            return;
        }
    }

    throw Exceptions::hexception(2, __LINE__, __FILE__);
}

bool IDisplayConfig::GetMonitorName(std::wstring& out)
{
    DISPLAYCONFIG_TARGET_DEVICE_NAME name;
    name.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
    name.header.size = sizeof(name);
    name.header.adapterId = m_adapterID;
    name.header.id = m_targetID;

    if (DisplayConfigGetDeviceInfo(&name.header) != ERROR_SUCCESS) {
        return false;
    }

    out = name.monitorFriendlyDeviceName;
    return true;

}

bool IDisplayConfig::GetOutputTech(DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY& out)
{
    DISPLAYCONFIG_TARGET_BASE_TYPE bt;
    bt.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_BASE_TYPE;
    bt.header.size = sizeof(bt);
    bt.header.adapterId = m_adapterID;
    bt.header.id = m_targetID;

    if (DisplayConfigGetDeviceInfo(&bt.header) != ERROR_SUCCESS) {
        return false;
    }

    out = bt.baseOutputTechnology;
    return true;
}

const char* IDisplayConfig::GetOutputTypeName(DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY vo)
{
    switch (vo)
    {
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_OTHER:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_OTHER);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HD15:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HD15);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SVIDEO:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SVIDEO);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPOSITE_VIDEO:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPOSITE_VIDEO);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPONENT_VIDEO:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPONENT_VIDEO);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DVI:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DVI);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HDMI:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HDMI);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_D_JPN:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_D_JPN);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDI:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDI);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EXTERNAL:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EXTERNAL);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EXTERNAL:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EXTERNAL);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDTVDONGLE:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDTVDONGLE);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_MIRACAST:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_MIRACAST);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INDIRECT_WIRED:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INDIRECT_WIRED);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INDIRECT_VIRTUAL:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INDIRECT_VIRTUAL);
    case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL:
        return STR(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL);
    }

    return "Unknown";
}
