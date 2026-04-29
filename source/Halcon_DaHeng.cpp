#include "Halcon.h"
#include "Halcon_DaHeng.h"
#include "HalconCpp.h"
#include "GxIAPI.h"
#include <cstdint>
#include <cstring>
#include <cerrno>
using namespace HalconCpp;

#if !defined(_WIN32) && !defined(_WIN64)
typedef int64_t __int64;
static inline int strcpy_s(char *dst, size_t dst_size, const char *src)
{
    if (!dst || !src || dst_size == 0) return EINVAL;
    size_t n = strlen(src);
    if (n >= dst_size) { dst[0] = '\0'; return ERANGE; }
    memcpy(dst, src, n + 1);
    return 0;
}
template <size_t N>
static inline int strcpy_s(char (&dst)[N], const char *src) { return strcpy_s(dst, N, src); }
#endif

#define H_GXCamera_TAG 0xC0FFEE80
#define H_GXCamera_SEM_TYPE "GXCamera"
#define Return_Herror                  \
    if (re != GX_STATUS_SUCCESS)       \
    {                                  \
        return 10000 * H__LINE__ + re; \
    }

extern "C"
{
    typedef struct
    {
        GX_DEV_HANDLE 相机句柄;
    } HUserHandleData;

    static Herror HUserHandleDestructor(Hproc_handle ph, HUserHandleData *data)
    {
        int re = GX_STATUS_SUCCESS;
        if (data->相机句柄)
        {

            GXCloseDevice(data->相机句柄);
            data->相机句柄 = NULL;
            int re = GXCloseLib();
        }
        else
        {
            return H_MSG_TRUE;
        }
        return HFree(ph, data);
    }
    // 句柄类型描述符
    const HHandleInfo HandleTypeUser = HANDLE_INFO_INITIALIZER_NOSER(H_GXCamera_TAG, H_GXCamera_SEM_TYPE, HUserHandleDestructor, NULL, NULL);
}

Herror HGXInitLib(Hproc_handle proc_handle)
{
    int re = GXInitLib();
    Return_Herror return H_MSG_TRUE;
}

Herror HGXCloseLib(Hproc_handle proc_handle)
{
    int re = GXCloseLib();
    Return_Herror return H_MSG_TRUE;
}
Herror HGXSetLogType(Hproc_handle proc_handle)
{
    Hcpar ui32LogType;
    HGetSPar(proc_handle, 1, LONG_PAR, &ui32LogType, 1);
    int re = GXSetLogType((uint32_t)ui32LogType.par.l);
    Return_Herror return H_MSG_TRUE;
}
Herror HGXGetLogType(Hproc_handle proc_handle)
{
    uint32_t ui32LogType;
    int re = GXGetLogType(&ui32LogType);
    Return_Herror
        int64_t LogType = (int64_t)ui32LogType;
    HPutElem(proc_handle, 1, &LogType, 1, LONG_PAR);
    return H_MSG_TRUE;
}

Herror HGXGetLastError(Hproc_handle proc_handle)
{

    GX_STATUS emStatus = GX_STATUS_SUCCESS;
    GX_STATUS emErrCode = GX_STATUS_SUCCESS;
    char *pszTemp = NULL;
    size_t nSize = 0;
    // 传入NULL指针，获取实际长度，然后申请buffer再获取描述信息
    GXGetLastError(&emErrCode, NULL, &nSize);
    pszTemp = new char[nSize];
    emStatus = GXGetLastError(&emErrCode, pszTemp, &nSize);
    INT4_8 ErrCode = (INT4_8)emErrCode;
    HPutElem(proc_handle, 1, &pszTemp, 1, STRING_PAR);
    HPutElem(proc_handle, 2, &ErrCode, 1, LONG_PAR);

    delete[] pszTemp;
    pszTemp = NULL;
    return H_MSG_TRUE;
}
struct MyContext
{
    int 分时频闪数;
    char 相机用户名[64];
    HTuple 相机采集队列; // 直接放对象

    MyContext(int flash, const char *name, const HTuple &queue)
        : 分时频闪数(flash), 相机采集队列(queue)
    {
        strcpy_s(相机用户名, name);
    }
};
static void GX_STDC OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM *pFrame)
{
    if (pFrame->status == GX_FRAME_STATUS_SUCCESS)
    {
        // 对图像进行某些操作
        HObject ho_Image;
        HTuple hv_MessageHandle;
        HTuple hv_MessageHandleRemove;
        MyContext *ctx = static_cast<MyContext *>(pFrame->pUserParam);
        if (pFrame->nPixelFormat == GX_PIXEL_FORMAT_MONO8)
        {
            GenImage1(&ho_Image, "byte", (pFrame->nWidth) * (ctx->分时频闪数), (pFrame->nHeight) / (ctx->分时频闪数), (__int64)pFrame->pImgBuf);
        }
        else if (pFrame->nPixelFormat == GX_PIXEL_FORMAT_MONO12)
        {
            GenImage1(&ho_Image, "uint2", (pFrame->nWidth) * (ctx->分时频闪数), (pFrame->nHeight) / (ctx->分时频闪数), (__int64)pFrame->pImgBuf);
        }
        CreateMessage(&hv_MessageHandle);
        SetMessageObj(ho_Image, hv_MessageHandle, "image");
        SetMessageTuple(hv_MessageHandle, "nFrameNum", (__int64)(pFrame->nFrameID));
        // GX_CHUNK_DATA_HANDLE
        SetMessageTuple(hv_MessageHandle, "nDeviceID", ctx->相机用户名);
        try
        {
          EnqueueMessage((ctx->相机采集队列), hv_MessageHandle, HTuple(), HTuple());
        }
         catch (HException &HDevExpDefaultException)
        {
            //DequeueMessage((ctx->相机采集队列), "timeout", "infinite", &hv_MessageHandleRemove);
            //EnqueueMessage((ctx->相机采集队列), hv_MessageHandle, HTuple(), HTuple());
            //ClearMessage(hv_MessageHandleRemove);

        }
        ClearMessage(hv_MessageHandle);
    }
    return;
}
Herror HGXOPenCameraByID(Hproc_handle proc_handle)
{

    Hcpar 相机用户名, 分时频闪数, 设备类型;
    const Hcpar *相机采集队列容器;
    INT4_8 参数个数; // 参数个数
    HAllocStringMem(proc_handle, 64);
    HGetSPar(proc_handle, 1, STRING_PAR, &相机用户名, 1);
    HGetSPar(proc_handle, 2, LONG_PAR, &分时频闪数, 1);
    HGetSPar(proc_handle, 3, LONG_PAR, &设备类型, 1);
    HGetPPar(proc_handle, 4, &相机采集队列容器, &参数个数);
    HTuple 相机采集队列(const_cast<Hcpar *>(相机采集队列容器), 1);

    uint32_t ui32DeviceNum = 0;
    GXUpdateAllDeviceListEx(设备类型.par.l, &ui32DeviceNum, 1000);
    GX_STATUS re = GX_STATUS_SUCCESS;
    GX_OPEN_PARAM stOpenParam;
    stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
    stOpenParam.openMode = GX_OPEN_USERID;
    stOpenParam.pszContent = 相机用户名.par.s;
    GX_DEV_HANDLE hDevice = NULL;
    re = GXOpenDevice(&stOpenParam, &hDevice);
    Return_Herror

        HUserHandleData **handle_data;
    // 分配输出句柄
    HCkP(HAllocOutputHandle(proc_handle, 1, &handle_data, &HandleTypeUser));
    // 分配并初始化用户数据
    HCkP(HAlloc(proc_handle, sizeof(HUserHandleData), (void **)handle_data));
    (*handle_data)->相机句柄 = hDevice;
    MyContext *ctx = new MyContext(分时频闪数.par.l, 相机用户名.par.s, 相机采集队列);
    GXRegisterCaptureCallback(hDevice, ctx, OnFrameCallbackFun);

    Return_Herror return H_MSG_TRUE;
}

Herror HGXSetCommandValue(Hproc_handle proc_handle)
{
    HUserHandleData *handle_data;
    Hcpar 参数名称;
    HAllocStringMem(proc_handle, 64);
    HGetCElemH1(proc_handle, 1, &HandleTypeUser, &handle_data);

    HGetSPar(proc_handle, 2, STRING_PAR, &参数名称, 1);
    GX_STATUS re = GX_STATUS_SUCCESS;
    re = GXSetCommandValue(handle_data->相机句柄, 参数名称.par.s);
    Return_Herror return H_MSG_TRUE;
}

Herror HGXSetFloatValue(Hproc_handle proc_handle)
{
    HUserHandleData *handle_data;
    Hcpar 参数名称, 参数值;
    HAllocStringMem(proc_handle, 64);
    HGetCElemH1(proc_handle, 1, &HandleTypeUser, &handle_data);

    HGetSPar(proc_handle, 2, STRING_PAR, &参数名称, 1);
    HGetSPar(proc_handle, 3, DOUBLE_PAR, &参数值, 1);

    int re = GXSetFloatValue(handle_data->相机句柄, 参数名称.par.s, 参数值.par.d);
    Return_Herror return H_MSG_TRUE;
}
Herror HGXSetEnumValue(Hproc_handle proc_handle)
{
    HUserHandleData *handle_data;

    Hcpar 参数名称, 参数值;
    HAllocStringMem(proc_handle, 64);
    HGetCElemH1(proc_handle, 1, &HandleTypeUser, &handle_data);

    HGetSPar(proc_handle, 2, STRING_PAR, &参数名称, 1);
    HGetSPar(proc_handle, 3, LONG_PAR, &参数值, 1);

    int re = GXSetEnumValue(handle_data->相机句柄, 参数名称.par.s, 参数值.par.l);
    Return_Herror return H_MSG_TRUE;
}
Herror HGXSetBoolValue(Hproc_handle proc_handle)
{
    HUserHandleData *handle_data;

    Hcpar 参数名称, 参数值;
    HAllocStringMem(proc_handle, 64);
    HGetCElemH1(proc_handle, 1, &HandleTypeUser, &handle_data);

    HGetSPar(proc_handle, 2, STRING_PAR, &参数名称, 1);
    HGetSPar(proc_handle, 3, LONG_PAR, &参数值, 1);

    if (参数值.par.l == 0)
    {
        int re = GXSetBoolValue(handle_data->相机句柄, 参数名称.par.s, 0);
        Return_Herror return H_MSG_TRUE;
    }
    else if (参数值.par.l == 1)
    {
        int re = GXSetBoolValue(handle_data->相机句柄, 参数名称.par.s, 1);
        Return_Herror return H_MSG_TRUE;
    }
    else
    {
         return 10000 * H__LINE__ ;
    }
}
Herror HGXSetIntValue(Hproc_handle proc_handle)
{
    HUserHandleData *handle_data;

    Hcpar 参数名称, 参数值;
    HAllocStringMem(proc_handle, 64);
    HGetCElemH1(proc_handle, 1, &HandleTypeUser, &handle_data);

    HGetSPar(proc_handle, 2, STRING_PAR, &参数名称, 1);
    HGetSPar(proc_handle, 3, LONG_PAR, &参数值, 1);
    int re = GXSetIntValue(handle_data->相机句柄, 参数名称.par.s, 参数值.par.l);
    Return_Herror return H_MSG_TRUE;
}

Herror HGXGetFloatValue(Hproc_handle proc_handle)
{
    HUserHandleData *handle_data;
    Hcpar 参数名称;
    HAllocStringMem(proc_handle, 64);
    HGetCElemH1(proc_handle, 1, &HandleTypeUser, &handle_data);
    HGetSPar(proc_handle, 2, STRING_PAR, &参数名称, 1);

    GX_FLOAT_VALUE stFloatValue;
    int re = GXGetFloatValue(handle_data->相机句柄, 参数名称.par.s, &stFloatValue);
    Return_Herror

    double dValue = stFloatValue.dCurValue;
    HPutElem(proc_handle, 1, &dValue, 1, DOUBLE_PAR);
    return H_MSG_TRUE;
}

Herror HGXGetEnumValue(Hproc_handle proc_handle)
{
    HUserHandleData *handle_data;
    Hcpar 参数名称;
    HAllocStringMem(proc_handle, 64);
    HGetCElemH1(proc_handle, 1, &HandleTypeUser, &handle_data);
    HGetSPar(proc_handle, 2, STRING_PAR, &参数名称, 1);

    GX_ENUM_VALUE stEnumValue;
    int re = GXGetEnumValue(handle_data->相机句柄, 参数名称.par.s, &stEnumValue);
    Return_Herror

    INT4_8 nValue = (INT4_8)stEnumValue.stCurValue.nCurValue;
    HPutElem(proc_handle, 1, &nValue, 1, LONG_PAR);
    return H_MSG_TRUE;
}

Herror HGXGetBoolValue(Hproc_handle proc_handle)
{
    HUserHandleData *handle_data;
    Hcpar 参数名称;
    HAllocStringMem(proc_handle, 64);
    HGetCElemH1(proc_handle, 1, &HandleTypeUser, &handle_data);
    HGetSPar(proc_handle, 2, STRING_PAR, &参数名称, 1);

    bool bValue;
    int re = GXGetBoolValue(handle_data->相机句柄, 参数名称.par.s, &bValue);
    Return_Herror

    INT4_8 nValue = bValue ? 1 : 0;
    HPutElem(proc_handle, 1, &nValue, 1, LONG_PAR);
    return H_MSG_TRUE;
}

Herror HGXGetIntValue(Hproc_handle proc_handle)
{
    HUserHandleData *handle_data;
    Hcpar 参数名称;
    HAllocStringMem(proc_handle, 64);
    HGetCElemH1(proc_handle, 1, &HandleTypeUser, &handle_data);
    HGetSPar(proc_handle, 2, STRING_PAR, &参数名称, 1);

    GX_INT_VALUE stIntValue;
    int re = GXGetIntValue(handle_data->相机句柄, 参数名称.par.s, &stIntValue);
    Return_Herror

    INT4_8 nValue = (INT4_8)stIntValue.nCurValue;
    HPutElem(proc_handle, 1, &nValue, 1, LONG_PAR);
    return H_MSG_TRUE;
}