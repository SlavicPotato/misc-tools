#include "pch.h"

#define BoolToYN(x) ((x) ? "yes" : "no")

#define NVFTS_SUBKEY	"SYSTEM\\CurrentControlSet\\Services\\nvlddmkm\\FTS"
#define NVFTS_VAL		"EnableRID70579"

typedef void(*execfunc_t)(void*);

ILogger gLog(true);

#define Message(fmt, ...) gLog.Message(fmt, __VA_ARGS__)

using namespace StrHelpers;

static void wrap_exec(execfunc_t f, void* a = nullptr)
{
    try {
        f(a);
    }
    catch (Exceptions::hexception& e) {
        Message("Exception occured at %s:%d (0x%lX): %s | expr: %s",
            e.file(), e.line(), e.hresult(), e.what(), e.expression());
    }
}

inline static UINT getrr(DISPLAYCONFIG_RATIONAL &r)
{
    if (!r.Denominator) {
        return 0;
    }
    return r.Numerator / r.Denominator;
}

static void monitorDump(void* a)
{
    auto hMonitor = static_cast<HMONITOR>(a);

    IDisplayConfig dc(hMonitor);

    DISPLAYCONFIG_PATH_TARGET_INFO ti;
    std::wstring name;

    dc.GetTargetInfo(ti);

    Message("\t\tMonitor: %s\n",
        dc.GetMonitorName(name) ? ToNative(name).c_str() : "FAILED");

    Message("\t\t\tRefresh rate: %u/%u (%u Hz)", 
        ti.refreshRate.Numerator, ti.refreshRate.Denominator, getrr(ti.refreshRate));
    Message("\t\t\tScaling: %s", dc.GetScalingName(ti.scaling));
    Message("\t\t\tRotation: %s", dc.GetRotationName(ti.rotation));
    Message("\t\t\tScanline: %s", dc.GetScanlineOrderingName(ti.scanLineOrdering));
    Message("\t\t\tOutputType: %s", dc.GetOutputTechName(ti.outputTechnology));
    Message("\t\t\tStatus flags: 0x%X", ti.statusFlags);
}

static void dxgiDump(void*)
{

    Message("** DXGI info\n");

    IDXGI dxgi;

    Message("\tVersion: %u", Misc::Underlying(dxgi.GetVersion()));

    BOOL allowTearing;
    bool r = dxgi.CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(BOOL));

    Message("\tTearing support: %s", BoolToYN(r && allowTearing));
    Message("\tFlip: sequential=%s discard=%s",
        BoolToYN(dxgi.HasCapability(DXGICaps::FLIP_SEQUENTIAL)),
        BoolToYN(dxgi.HasCapability(DXGICaps::FLIP_DISCARD)));

    Message("\n");
    Message("** Video adapters\n");

    IDXGI::AdapterList adList;
    dxgi.GetAdapters(adList);

    if (!adList.size()) {
        Message("No adapters detected");
        return;
    }

    for (const auto& adapter : adList)
    {
        DXGI_ADAPTER_DESC ad;
        if (!adapter.GetDesc(ad)) {
            Message("  [%u] UNKNOWN ADAPTER", adapter.GetIndex());
        }
        else {
            Message("  [%u] %s | MEM: %zu MB | SHARED: %zu MB",
                adapter.GetIndex(),
                ToNative(ad.Description).c_str(),
                ad.DedicatedVideoMemory / 1024 / 1024,
                ad.SharedSystemMemory / 1024 / 1024);
        }

        IAdapter::OutputList oList;
        adapter.GetOutputs(oList);

        if (oList.size()) {
            Message("");
        }

        for (const auto& output : oList)
        {
            std::string oname;
            std::string dname;

            bool gd1, gd;

            DXGI_OUTPUT_DESC1 od;
            gd1 = gd = output.GetDesc1(od);
            if (!gd) {
                gd = output.GetDesc(reinterpret_cast<DXGI_OUTPUT_DESC&>(od));
            }

            if (gd) {
                dname = ToNative(od.DeviceName);

                DISPLAY_DEVICEA dm;
                dm.cb = sizeof(dm);

                if (EnumDisplayDevicesA(dname.c_str(), 0, &dm, 0) != FALSE) {
                    oname = dm.DeviceID;
                }
                else {
                    oname = "UNKNOWN ID";
                }

                Message("\t%s [%s]\n", dname.c_str(), oname.c_str());
            }
            else {
                Message("\tUNKNOWN OUTPUT\n");
            }

            UINT flags;
            if (output.QueryHardwareCompositionSupport(flags)) {
                Message("\t\tHW Composition: fullscreen=%s  windowed=%s  cursor stretched=%s",
                    BoolToYN(flags & DXGI_HARDWARE_COMPOSITION_SUPPORT_FLAG_FULLSCREEN),
                    BoolToYN(flags & DXGI_HARDWARE_COMPOSITION_SUPPORT_FLAG_WINDOWED),
                    BoolToYN(flags & DXGI_HARDWARE_COMPOSITION_SUPPORT_FLAG_CURSOR_STRETCHED));
            }
            else {
                Message("\t\tQueryHardwareCompositionSupport FAILED");
            }

            if (gd) {
                Message("\t\tAttached: %s | Coords: ((%ld, %ld) (%ld, %ld)) %ldx%ld",
                    BoolToYN(od.AttachedToDesktop), 
                    od.DesktopCoordinates.left, od.DesktopCoordinates.top,
                    od.DesktopCoordinates.right, od.DesktopCoordinates.bottom,
                    od.DesktopCoordinates.right - od.DesktopCoordinates.left,
                    od.DesktopCoordinates.bottom - od.DesktopCoordinates.top);
            }

            if (gd1) {
                Message("\t\tBitsPerColor: %u | ColorSpace: %s",
                    od.BitsPerColor, output.GetColorSpaceName(od.ColorSpace));
            }

            if (gd) {
                wrap_exec(monitorDump, od.Monitor);
            }

            Message("");
        }
    }
}

static void d3d11Dump(void*)
{
    Message("** D3D info\n");

    ID3D11 d3d11;

    std::string ftLevel;
    if (d3d11.GetMaxFeatureLevelStr(ftLevel)) {
        Message("\tFeature level: %s", ftLevel.c_str());
    }
    else {
        Message("\tFeature level: %X", d3d11.GetMaxFeatureLevel());
    }
}

static void regDump(void*)
{
    Message("** Registry\n");

    IRegistry reg(HKEY_LOCAL_MACHINE, NVFTS_SUBKEY, KEY_READ);

    DWORD v;
    reg.GetDWORD(NVFTS_VAL, v, 0);
    Message("\tnvlddmkm: EnableRID70579=%ld", v);
}

static void run()
{
    wrap_exec(d3d11Dump);
    Message("\n");
    wrap_exec(dxgiDump);
    Message("\n");
    wrap_exec(regDump);
}

static int pauseexit(int code = 0)
{
    getchar();
    return code;
}

int main()
{
    TCHAR exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);

    TCHAR exeFolder[MAX_PATH];
    _tcscpy_s(exeFolder, exePath);
    PathRemoveFileSpec(exeFolder);

    _snprintf_s(exePath, _TRUNCATE, "%s\\%s", exeFolder, "AdapterDump.log");

    if (!gLog.Open(exePath)) {
        
        auto errcode = GetLastError();
        Message("Couldn't create log file: (%ld) %s",
            errcode,
            std::system_category().message(errcode).c_str());
    }

    try {
        run();
    }
    catch (std::exception& e) {
        Message("Exception occured: %s", e.what());
    }

    gLog.Close();

    return pauseexit();
}
