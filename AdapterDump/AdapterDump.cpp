#include "pch.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define MessageDbg(fmt, ...) printf(__FILE__ ":" STR(__LINE__) " <" __FUNCTION__ "> " fmt "\n", __VA_ARGS__)
#define Message(fmt, ...) printf(fmt "\n", __VA_ARGS__)

#define BoolToYN(x) ((x) ? "yes" : "no")

#define NVFTS_SUBKEY	"SYSTEM\\CurrentControlSet\\Services\\nvlddmkm\\FTS"
#define NVFTS_VAL		"EnableRID70579"

typedef void(*execfunc_t)(void);

static void dxgiDump()
{
    using namespace StrHelpers;

    IDXGI dxgi;

    Message("** DXGI info\n");

    Message("\t Version: %u", Misc::Underlying(dxgi.GetVersion()));

    BOOL allowTearing;
    bool r = dxgi.CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(BOOL));

    Message("\t Tearing support: %s", BoolToYN(r && allowTearing));
    Message("\t Flip: sequential=%s discard=%s",
        BoolToYN(dxgi.HasCapability(DXGICaps::DXGI_CAP_FLIP_SEQUENTIAL)),
        BoolToYN(dxgi.HasCapability(DXGICaps::DXGI_CAP_FLIP_DISCARD)));

    Message("\n");
    Message("** Video adapters\n");

    IDXGI::AdapterList adList;
    dxgi.GetAdapters(adList);

    if (!adList.size()) {
        Message("No adapters detected");
        return;
    }

    for (auto& adapter : adList) {
        DXGI_ADAPTER_DESC ad;
        if (!adapter.GetDesc(ad)) {
            Message("[%u] UNKNOWN ADAPTER", adapter.GetIndex());
        }
        else {
            Message("[%u] %s | MEM: %zu MB | SHARED: %zu MB",
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

        for (auto& output : oList) {

            UINT flags;

            bool hwcomp_w = false;
            bool hwcomp_f = false;
            bool hwcomp_cs = false;

            bool qr = output.QueryHardwareCompositionSupport(flags);
            if (qr) {
                hwcomp_w = (flags & DXGI_HARDWARE_COMPOSITION_SUPPORT_FLAG_WINDOWED) != 0;
                hwcomp_f = (flags & DXGI_HARDWARE_COMPOSITION_SUPPORT_FLAG_FULLSCREEN) != 0;
                hwcomp_cs = (flags & DXGI_HARDWARE_COMPOSITION_SUPPORT_FLAG_CURSOR_STRETCHED) != 0;
            }

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

                Message("\t %s | %s\n", dname.c_str(), oname.c_str());
            }
            else {
                Message("\t UNKNOWN OUTPUT\n");
            }

            if (qr) {
                Message("\t\t HW Composition: fullscreen=%s  windowed=%s  cursor stretched=%s",
                    BoolToYN(hwcomp_f), BoolToYN(hwcomp_w), BoolToYN(hwcomp_cs));
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

    return;
}

static void regDump()
{
    Message("** Registry\n");

    IRegistry reg(HKEY_LOCAL_MACHINE, NVFTS_SUBKEY, KEY_READ);

    DWORD v;
    reg.GetDWORD(NVFTS_VAL, v, 0);
    Message("\t nvlddmkm: EnableRID70579=%d", v);
}

static void wrap_exec(execfunc_t f)
{
    try {
        f();
    }
    catch (Exceptions::hexception& e) {
        Message("Exception occured in %s, line %d (0x%lX): %s", e.file(), e.line(), e.hresult(), e.what());
    }
}

static void run()
{
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

    return pauseexit(0);
}
