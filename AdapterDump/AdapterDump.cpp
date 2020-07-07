#include "pch.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define MessageDbg(fmt, ...) printf(__FILE__ ":" STR(__LINE__) " <" __FUNCTION__ "> " fmt "\n", __VA_ARGS__)
#define Message(fmt, ...) printf(fmt "\n", __VA_ARGS__)

#define BoolToYN(x) (x ? "yes" : "no")

static int run()
{
    using namespace StrHelpers;

    Message("** Video adapter dump\n");

    IAdapters ia;

    IAdapters::AdapterList adList;
    ia.Get(adList);

    if (!adList.size()) {
        Message("No adapters detected");
        return 0;
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

            bool qr = output.QueryHardwareCompositionSupport(&flags);
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
                Message("\t\t HW Composition: Fullscreen=%s  Windowed=%s  CursorStretched=%s",
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

    return 0;
}

static int pauseexit(int code = 0)
{
    getchar();
    return code;
}

int main()
{
    int r;
    try {
        r = run();
    }
    catch (Exceptions::hexception& e) {
        Message("Exception occured in %s, line %d (%lX): %s", e.file(), e.line(), e.hresult(), e.what());
        r = 1;
    }
    catch (std::exception& e) {
        Message("Exception occured: %s", e.what());
        r = 1;
    }

    return pauseexit(r);
}
