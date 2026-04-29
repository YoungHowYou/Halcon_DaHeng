#include "Halcon.h"
#if defined(_WIN32) || defined(_WIN64)
#define EXPORTS_API __declspec(dllexport)
#else
#define EXPORTS_API __attribute__((visibility("default")))
#endif
#ifdef __cplusplus
extern "C" {
#endif

#pragma region DaHeng
EXPORTS_API Herror HGXInitLib(Hproc_handle proc_handle);
EXPORTS_API Herror HGXCloseLib(Hproc_handle proc_handle);
EXPORTS_API Herror HGXSetLogType(Hproc_handle proc_handle);
EXPORTS_API Herror HGXGetLogType(Hproc_handle proc_handle);
EXPORTS_API Herror HGXGetLastError(Hproc_handle proc_handle);

EXPORTS_API Herror HGXOPenCameraByID(Hproc_handle proc_handle);
EXPORTS_API Herror HGXSetCommandValue(Hproc_handle proc_handle);
EXPORTS_API Herror HGXSetFloatValue(Hproc_handle proc_handle);
EXPORTS_API Herror HGXSetEnumValue(Hproc_handle proc_handle);
EXPORTS_API Herror HGXSetBoolValue(Hproc_handle proc_handle);
EXPORTS_API Herror HGXSetIntValue(Hproc_handle proc_handle);

EXPORTS_API Herror HGXGetFloatValue(Hproc_handle proc_handle);
EXPORTS_API Herror HGXGetEnumValue(Hproc_handle proc_handle);
EXPORTS_API Herror HGXGetBoolValue(Hproc_handle proc_handle);
EXPORTS_API Herror HGXGetIntValue(Hproc_handle proc_handle);

#pragma endregion



#ifdef __cplusplus
}
#endif 