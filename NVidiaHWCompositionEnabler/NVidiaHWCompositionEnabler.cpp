#define WIN32_LEAN_AND_MEAN  

#include <windows.h>
#include <setupapi.h>
#include <stdio.h>
#include <cguid.h>


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define Message(fmt, ...) printf(__FILE__ ":" STR(__LINE__) " <" __FUNCTION__ "> " fmt, __VA_ARGS__)

#define NVFTS_SUBKEY	"SYSTEM\\CurrentControlSet\\Services\\nvlddmkm\\FTS"
#define NVFTS_VAL		"EnableRID70579"

static bool OpenRegistryKey(HKEY hKey, const char* subkey, REGSAM access, HKEY& out)
{
    HKEY result;
    LSTATUS r = RegOpenKeyExA(hKey, subkey, 0, access, &result);
    if (r != ERROR_SUCCESS) {
        Message("FAILED: %ld\n", r);
        return false;
    }

    out = result;

    return true;
}

static bool GetRegistryValDWORD(HKEY hKey, const char* name, DWORD& out, DWORD def)
{
    DWORD result;
    DWORD size = sizeof(result);

    LSTATUS r = RegQueryValueExA(
        hKey,
        name,
        0,
        NULL,
        reinterpret_cast<BYTE*>(&result),
        &size);

    if (r != ERROR_SUCCESS) {
        out = def;
        return false;
    }

    out = result;
    return true;
}

static bool SetRegistryValDWORD(HKEY hKey, const char* name, DWORD val)
{
    LSTATUS r = RegSetValueExA(
        hKey,
        name,
        0,
        REG_DWORD,
        reinterpret_cast<BYTE*>(&val),
        sizeof(val));

    if (r != ERROR_SUCCESS) {
        Message("FAILED: %ld", r);
        return false;
    }

    return true;
}

static void test()
{

    HDEVINFO hdi = SetupDiCreateDeviceInfoList(NULL, NULL);
   
    SP_DEVINFO_DATA o;
    o.cbSize = sizeof(SP_DEVINFO_DATA);

    BOOL r = SetupDiCreateDeviceInfoA(hdi, "PCI\\VEN_10DE&DEV_1F08&SUBSYS_868C1043&REV_A1\\4&2D78AB8F", &GUID_NULL, NULL, NULL, 0, &o);
    printf("%p %d\n", hdi, r);

    if (r == FALSE)
        return;


    r = SetupDiRestartDevices(hdi, &o);

    printf("%d\n", r);
}

static int errexit()
{
    getchar();
    return 1;
}

int main()
{
    /*test();
    getchar();
    return 0;*/

    printf("** NVIDIA windowed hardware composition support toggler\n\n");

    HKEY hKey;

    bool r = OpenRegistryKey(HKEY_LOCAL_MACHINE, NVFTS_SUBKEY, KEY_READ | KEY_WRITE, hKey);
    if (!r) {
        return errexit();
    }

    DWORD vNVFTS;
    GetRegistryValDWORD(hKey, NVFTS_VAL, vNVFTS, 0);

    printf("Windowed hardware composition is currently %s\n", vNVFTS ? "ENABLED" : "DISABLED");

    bool act = false;

    while (true) {
        printf("Do you wish to %s? (Y/N) ", vNVFTS ? "disable" : "enable");

        int c = getchar();

        for (int t = c; t != EOF && t != '\n'; t = getchar()) {}

        if (c == 'y' || c == 'Y') {
            act = true;
            break;
        }
        else if (c == 'n' || c == 'N') {
            break;
        }
        else {
            printf("Type in YES or NO.\n");
        }
    }

    if (act) {
        if (SetRegistryValDWORD(hKey, NVFTS_VAL, !vNVFTS)) {
            printf("Succeeded, restart your computer for the changes to take effect.\n");
        }
        getchar();
    }
    else {
        printf("Nothing was changed.\n");
    }

    RegCloseKey(hKey);

    return 0;
}
