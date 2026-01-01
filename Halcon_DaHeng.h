#include "Halcon.h"
#define EXPORTS_API __declspec(dllexport)
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





#pragma endregion



#ifdef __cplusplus
}
#endif 


