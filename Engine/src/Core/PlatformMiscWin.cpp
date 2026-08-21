// Copyright mzoesch. All rights reserved.

#if 0

#if JAFG_PLATFORM_WINDOWS

#include "Nodes/InterfaceTypes.h"
#include "Platform/PlatformMisc.h"

#include <cfgmgr32.h> /* MAX_DEVICE_ID_LEN */

#include <SetupApi.h>
/* For GCC/Clang, link with the setup api using '-lsetupapi' at compile time. */
#if JAFG_WITH_MSVC
    #pragma comment( lib, "setupapi.lib" )
#endif /* JAFG_WITH_MSVC */

namespace
{

typedef std::map<std::wstring, std::wstring> DevNameToDevId;          /* DeviceName -> DeviceId */
typedef std::map<std::wstring, std::pair<int, int> > PhyMonitorSizes; /* DeviceId -> (width, height) */

//# Get a handle for the primary physical monitor.
UNUSED NOINLINE HMONITOR GetPrimaryMonitorHandle()
{
    return MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
}

//#
//# Tries to retrieve the monitor sizes using the SetupAPI and the EDID data in the w registry.
//#
//# @return The monitor sizes in millimeter.
//#
NOINLINE PhyMonitorSizes FindMonitorSizesFromEdid()
{
    PhyMonitorSizes ScreenSizes;

    constexpr GUID GuidDevInterfaceMonitor =
    {
        0xe6f07b5f, 0xee97, 0x4a90, { 0xb0, 0x76, 0x33, 0xf5, 0x7b, 0xf4, 0xea, 0xa7, },
    };
    const HDEVINFO HDevInfo = SetupDiGetClassDevs(&GuidDevInterfaceMonitor, nullptr, nullptr, DIGCF_DEVICEINTERFACE);

    // Instead of creating a buffer in each iteration and calling SetupDiGetDeviceInterfaceDetail
    // twice (once to find the required buffer size and once to actually get the data), we create a
    // buffer up front with the maximum size it can have.
    // The 128 contant comes from the fact that the DeviceId parameter in DISPLAY_DEVICE is
    // at most 128 characters, and we will be looking for a match with this id later.
    // Note that the buffer is slightly larger than it has to be (as "sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA)"
    // was used instead of "offsetof(SP_DEVICE_INTERFACE_DETAIL_DATA, DevicePath)").
    wchar_t devPathBuffer[sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W) + (128 * sizeof(wchar_t))];

    DWORD MonitorIndex = 0;
    SP_DEVICE_INTERFACE_DATA DevInfo;
    DevInfo.cbSize = sizeof(DevInfo);
    while (SetupDiEnumDeviceInterfaces(HDevInfo, nullptr, &GuidDevInterfaceMonitor, MonitorIndex, &DevInfo))
    {
        ++MonitorIndex;

        // Retrieve the id of the device interface
        SP_DEVICE_INTERFACE_DETAIL_DATA_W* DevPathData = reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(devPathBuffer);
        DevPathData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        SP_DEVINFO_DATA DevInfoData;
        memset(&DevInfoData, 0, sizeof(DevInfoData));
        DevInfoData.cbSize = sizeof(DevInfoData);
        if (!SetupDiGetDeviceInterfaceDetailW(HDevInfo, &DevInfo, DevPathData, sizeof(devPathBuffer), nullptr, &DevInfoData))
        {
            continue; // Error
        }

        // We now have the device id that we will use to match this device on other places later.
        const std::wstring DeviceId = DevPathData->DevicePath;

        // Find the instance id of the device to look up the EDID in the registry
        wchar_t InstanceId[MAX_DEVICE_ID_LEN];
        if (!SetupDiGetDeviceInstanceIdW(HDevInfo, &DevInfoData, InstanceId, MAX_PATH, nullptr))
        {
            continue; // Error
        }

        // Find the EDID registry key
        HKEY hEDIDRegKey = SetupDiOpenDevRegKey(HDevInfo, &DevInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        if (!hEDIDRegKey || (hEDIDRegKey == INVALID_HANDLE_VALUE))
        {
            continue; // Error
        }

        // Read the EDID data from the registry
        BYTE DataEDID[1024];
        DWORD SizeOfDataEDID = sizeof(DataEDID);
        if (ERROR_SUCCESS == RegQueryValueExW(hEDIDRegKey, L"EDID", nullptr, nullptr, DataEDID, &SizeOfDataEDID))
        {
            // Extract the width and height of the monitor from the EDID
            int WidthMm = ((DataEDID[68] & 0xF0) << 4) + DataEDID[66];
            int HeightMm = ((DataEDID[68] & 0x0F) << 8) + DataEDID[67];
            ScreenSizes[DeviceId] = std::make_pair(WidthMm, HeightMm);
        }

        RegCloseKey(hEDIDRegKey);

        continue;
    }

    return ScreenSizes;
}

/**
 * Creates mapping between device names and ids.
 */
NOINLINE DevNameToDevId GetDeviceNamesToIdMap()
{
    DevNameToDevId NamesToIdMap;

    u32 NrPaths;
    u32 NrModes;
    GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &NrPaths, &NrModes);

    std::vector<DISPLAYCONFIG_PATH_INFO> Paths(NrPaths);
    std::vector<DISPLAYCONFIG_MODE_INFO> Modes(NrModes);
    QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &NrPaths, &Paths[0], &NrModes, &Modes[0], nullptr);

    for (const auto& [SourceInfo, TargetInfo, Flags] : Paths)
    {
        DISPLAYCONFIG_SOURCE_DEVICE_NAME SourceName;
        SourceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        SourceName.header.size = sizeof(SourceName);
        SourceName.header.adapterId = SourceInfo.adapterId;
        SourceName.header.id = SourceInfo.id;
        DisplayConfigGetDeviceInfo(&SourceName.header);

        DISPLAYCONFIG_TARGET_DEVICE_NAME TargetName;
        TargetName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        TargetName.header.size = sizeof(TargetName);
        TargetName.header.adapterId = SourceInfo.adapterId;
        TargetName.header.id = TargetInfo.id;
        DisplayConfigGetDeviceInfo(&TargetName.header);

        NamesToIdMap[SourceName.viewGdiDeviceName] = TargetName.monitorDevicePath;
    }

    return NamesToIdMap;
}

NOINLINE bool CaseInsensitiveComparison(const std::wstring& Str1, const std::wstring& Str2)
{
    if (Str1.length() != Str2.length())
    {
        return false;
    }

    for (unsigned i = 0; i < Str1.length(); ++i)
    {
        if (Str1[i] == Str2[i])
        {
            continue;
        }
        if ((Str1[i] >= L'A') && (Str1[i] <= L'Z') && ((Str1[i] - (L'A' - L'a')) == Str2[i]))
        {
            continue;
        }
        if ((Str2[i] >= L'A') && (Str2[i] <= L'Z') && ((Str2[i] - (L'A' - L'a')) == Str1[i]))
        {
            continue;
        }

        return false;
    }

    return true;
}

//# @author https://github.com/texus
LIntVector2 GetMonitorSizeInMillimeter(const HMONITOR HMonitor)
{
    const PhyMonitorSizes& sizesById = FindMonitorSizesFromEdid();
    const DevNameToDevId& deviceIdsByName = GetDeviceNamesToIdMap();

    MONITORINFOEXW MonInfo;
    MonInfo.cbSize = sizeof(MonInfo);
    if (!GetMonitorInfoW(HMonitor, &MonInfo))
    {
        return {};
    }

    const auto DeviceIdIt = deviceIdsByName.find(MonInfo.szDevice);
    if (DeviceIdIt == deviceIdsByName.end())
    {
        return {};
    }

    const std::wstring& DeviceId = DeviceIdIt->second;

    for (const auto& [Fst, Snd] : sizesById)
    {
        const std::wstring& DevId = Fst;
        const std::pair<int, int>& Size = Snd;
        if (CaseInsensitiveComparison(DeviceId, DevId) == false)
        {
            continue;
        }

        return { Size.first, Size.second };
    }

    return {};
}

BOOL CALLBACK MonitorEnumProc(const HMONITOR HMonitor, HDC HdcMonitor, LPRECT LprcMonitor, LPARAM DwData)
{
    using namespace Jafg;

    MONITORINFO info;
    info.cbSize = sizeof(info);

    if (GetMonitorInfo(HMonitor, &info))
    {
        LPhysicalViewport Viewport;

        Viewport.WidthPx  = Maths::Absolute(info.rcMonitor.left - info.rcMonitor.right);
        Viewport.HeightPx = Maths::Absolute(info.rcMonitor.top  - info.rcMonitor.bottom);

        const LIntVector2 Size = GetMonitorSizeInMillimeter(HMonitor);
        Viewport.WidthMm  = Size.X;
        Viewport.HeightMm = Size.Y;

        Viewport.Name = algo::sprintf("[{}] Generic Pnp Monitor ({}x{}px)",
            GPlatformMisc->PhysicalViewports.size(),
            Viewport.WidthPx, Viewport.HeightPx
            );

        GPlatformMisc->PhysicalViewports.emplace_back(std::move(Viewport));

        return TRUE;
    }

    LOG_ERROR(LogPlatformMisc, "Failed to get physical monitor info. Platform denied access or other unknown error.")

    return FALSE;
}

} /* ~Namespace <Anonymous> */

i32 Jafg::PlatformMisc::GetNumberOfPhysicalViewports()
{
    return ::GetSystemMetrics(SM_CMONITORS);
}

bool Jafg::PlatformMisc::SetPhysicalViewports()
{
    // DISPLAY_DEVICE Dd;
    // Dd.cb = sizeof(Dd);
    //
    // i32 DIdx = 0;
    // while (EnumDisplayDevices(nullptr, DIdx, &Dd, 0x0))
    // {
    //     std::wstring DName = Dd.DeviceName;
    //
    //     i32 MonitorIndex = 0;
    //     while (EnumDisplayDevices(DName.c_str(), MonitorIndex, &Dd, 0x0))
    //     {
    //         std::wcout << Dd.DeviceName << L", " <<Dd.DeviceString << L"\n";
    //         ++MonitorIndex;
    //     }
    //
    //     ++DIdx;
    //     continue;
    // }

    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, 0x0);

    return true;
}

#endif /* JAFG_PLATFORM_WINDOWS */

#endif /* 0 */
