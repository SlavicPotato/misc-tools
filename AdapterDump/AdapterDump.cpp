#include "pch.h"

#define Message(fmt, ...) printf(fmt "\n", __VA_ARGS__)

#define BoolToYN(x) ((x) ? "yes" : "no")

#define NVFTS_SUBKEY	"SYSTEM\\CurrentControlSet\\Services\\nvlddmkm\\FTS"
#define NVFTS_VAL		"EnableRID70579"

typedef void(*execfunc_t)(void);

static void dxgiDump()
{
    using namespace StrHelpers;

    Message("** DXGI info\n");

    IDXGI dxgi;

    Message("\t Version: %u", Misc::Underlying(dxgi.GetVersion()));

    BOOL allowTearing;
    bool r = dxgi.CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(BOOL));

    Message("\t Tearing support: %s", BoolToYN(r && allowTearing));
    Message("\t Flip: sequential=%s discard=%s",
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
            Message("   [%u] UNKNOWN ADAPTER", adapter.GetIndex());
        }
        else {
            Message("   [%u] %s | MEM: %zu MB | SHARED: %zu MB",
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

            DXGI_OUTPUT_DESC od;
            bool gd = output.GetDesc(od);

            if (gd) {
                dname = ToNative(od.DeviceName);

                DISPLAY_DEVICEA dm;
                dm.cb = sizeof(dm);

                if (EnumDisplayDevicesA(dname.c_str(), 0, &dm, 0) == TRUE) {
                    oname = dm.DeviceID;
                }
                else {
                    oname = "UNKNOWN ID";
                }

                Message("\t %s [%s]\n", dname.c_str(), oname.c_str());
            }
            else {
                Message("\t UNKNOWN OUTPUT\n");
            }

            UINT flags;
            if (output.QueryHardwareCompositionSupport(flags)) {
                Message("\t\t HW Composition: fullscreen=%s  windowed=%s  cursor stretched=%s",
                    BoolToYN(flags & DXGI_HARDWARE_COMPOSITION_SUPPORT_FLAG_FULLSCREEN),
                    BoolToYN(flags & DXGI_HARDWARE_COMPOSITION_SUPPORT_FLAG_WINDOWED),
                    BoolToYN(flags & DXGI_HARDWARE_COMPOSITION_SUPPORT_FLAG_CURSOR_STRETCHED));
            }
            else {
                Message("\t\t QueryHardwareCompositionSupport FAILED");
            }

            if (gd) {
                Message("\t\t Attached: %s | Rotation: %d", BoolToYN(od.AttachedToDesktop), od.Rotation);
            }

            Message("");
        }
    }
}

static void d3d11Dump()
{
    Message("** D3D info\n");

    ID3D11 d3d11;

    std::string ftLevel;
    if (d3d11.GetMaxFeatureLevelStr(ftLevel)) {
        Message("\t Feature level: %s", ftLevel.c_str());
    }
    else {
        Message("\t Feature level: %X", d3d11.GetMaxFeatureLevel());
    }
}

static void regDump()
{
    Message("** Registry\n");

    IRegistry reg(HKEY_LOCAL_MACHINE, NVFTS_SUBKEY, KEY_READ);

    DWORD v;
    reg.GetDWORD(NVFTS_VAL, v, 0);
    Message("\t nvlddmkm: EnableRID70579=%ld", v);
}

static void wrap_exec(execfunc_t f)
{
    try {
        f();
    }
    catch (Exceptions::hexception& e) {
        Message("Exception occured at %s:%d (0x%lX): %s", e.file(), e.line(), e.hresult(), e.what());
    }
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
    try {
        run();
    }
    catch (std::exception& e) {
        Message("Exception occured: %s", e.what());
    }

    return pauseexit();
}
