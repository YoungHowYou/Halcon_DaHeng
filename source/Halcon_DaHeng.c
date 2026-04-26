#include <stdio.h>
#include "Halcon_DaHeng.h"
Herror CGXInitLib(Hproc_handle proc_handle)
{
    return HGXInitLib(proc_handle);
}
Herror CGXCloseLib(Hproc_handle proc_handle)
{
    return HGXCloseLib(proc_handle);
}
Herror CGXSetLogType(Hproc_handle proc_handle)
{
    return HGXSetLogType(proc_handle);
}
Herror CGXGetLogType(Hproc_handle proc_handle)
{
    return HGXGetLogType(proc_handle);
}
Herror CGXGetLastError(Hproc_handle proc_handle)
{
    return HGXGetLastError(proc_handle);
}
Herror CGXOPenCameraByID(Hproc_handle proc_handle)
{
    return HGXOPenCameraByID(proc_handle);  
}
Herror CGXSetCommandValue(Hproc_handle proc_handle)
{
    return HGXSetCommandValue(proc_handle); 
}
Herror CGXSetFloatValue(Hproc_handle proc_handle)
{
    return HGXSetFloatValue(proc_handle); 
}
Herror CGXSetEnumValue(Hproc_handle proc_handle)
{
    return HGXSetEnumValue(proc_handle); 
}
Herror CGXSetBoolValue(Hproc_handle proc_handle)
{
    return HGXSetBoolValue(proc_handle); 
}
Herror CGXSetIntValue(Hproc_handle proc_handle)
{
    return HGXSetIntValue(proc_handle); 
}
Herror CGXGetFloatValue(Hproc_handle proc_handle)
{
    return HGXGetFloatValue(proc_handle); 
}
Herror CGXGetEnumValue(Hproc_handle proc_handle)
{
    return HGXGetEnumValue(proc_handle); 
}
Herror CGXGetBoolValue(Hproc_handle proc_handle)
{
    return HGXGetBoolValue(proc_handle); 
}
Herror CGXGetIntValue(Hproc_handle proc_handle)
{
    return HGXGetIntValue(proc_handle); 
}