#include <windows.h>
#include "avisynth.h"

typedef float matrix[12];
typedef float rgbtuple[3];

class ColorMatrixTransform : public GenericVideoFilter {
    matrix transformMatrix;
public:
    ColorMatrixTransform(PClip _child, matrix theMatrix, IScriptEnvironment* env);
    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
};

ColorMatrixTransform::ColorMatrixTransform(PClip _child, matrix theMatrix, IScriptEnvironment* env) :
    GenericVideoFilter(_child) {
    if (!vi.IsPlanar() || !vi.IsRGB() || vi.BitsPerComponent() != 32) {
        env->ThrowError("ColorMatrixTransform: 32 bit float RGBPS only!");
    }
    memcpy(transformMatrix,theMatrix,sizeof(matrix));
}


PVideoFrame __stdcall ColorMatrixTransform::GetFrame(int n, IScriptEnvironment* env) {

    PVideoFrame src = child->GetFrame(n, env);
    PVideoFrame dst = env->NewVideoFrame(vi);

    float *srcp[3];
    float *dstp[3];
    int src_pitch[3], dst_pitch[3], row_size[3], height[3];
    //int p, x, y;

    int planes[] = { PLANAR_R, PLANAR_G, PLANAR_B };

    for (int p = 0; p < 3; p++) {
        srcp[p] = (float*)src->GetReadPtr(planes[p]);
        dstp[p] = (float*)dst->GetWritePtr(planes[p]);

        src_pitch[p] = src->GetPitch(planes[p]);
        dst_pitch[p] = dst->GetPitch(planes[p]);
        row_size[p] = dst->GetRowSize(planes[p])/sizeof(float);
        height[p] = dst->GetHeight(planes[p]);
    }

    if (height[0] != height[1] || height[1] != height[2]) {
        env->ThrowError("ColorMatrixTransform: Heights of all RGB planes must be same!");
    }
    if (row_size[0] != row_size[1] || row_size[1] != row_size[2]) {
        env->ThrowError("ColorMatrixTransform: Rowsize (width) of all RGB planes must be same!");
    }

    float transformMatrixLocal0 = transformMatrix[0]; // stupid but the auto-vectorizer doesnt like arrays?
    float transformMatrixLocal1 = transformMatrix[1];
    float transformMatrixLocal2 = transformMatrix[2];
    float transformMatrixLocal3 = transformMatrix[3];
    float transformMatrixLocal4 = transformMatrix[4];
    float transformMatrixLocal5 = transformMatrix[5];
    float transformMatrixLocal6 = transformMatrix[6];
    float transformMatrixLocal7 = transformMatrix[7];
    float transformMatrixLocal8 = transformMatrix[8];
    float transformMatrixLocal9 = transformMatrix[9];
    float transformMatrixLocal10 = transformMatrix[10];
    float transformMatrixLocal11 = transformMatrix[11];

    //rgbtuple srcColor;
    //rgbtuple dstColor;
//#pragma omp parallel for 
    for (int y = 0; y < height[0]; y++) {
        float* srcpLocal[3];
        float* dstpLocal[3];
        for (int p = 0; p < 3; p++) {
            srcpLocal[p] = (float*)((char*)srcp[p] + src_pitch[p]*y);
            dstpLocal[p] = (float*)((char*)dstp[p] + dst_pitch[p]*y);
        }
        int rowSize0 = row_size[0];
        /*for (int x = 0; x < rowSize0; x++) {
            dstpLocal[0][x] = srcpLocal[0][x]*transformMatrix[0]+ srcpLocal[1][x] * transformMatrix[1]+ srcpLocal[2][x] * transformMatrix[2]+ transformMatrix[3];
            dstpLocal[1][x] = srcpLocal[0][x]*transformMatrix[4]+ srcpLocal[1][x] * transformMatrix[5]+ srcpLocal[2][x] * transformMatrix[6]+ transformMatrix[7];
            dstpLocal[2][x] = srcpLocal[0][x]*transformMatrix[8]+ srcpLocal[1][x] * transformMatrix[9]+ srcpLocal[2][x] * transformMatrix[10]+ transformMatrix[11];
        }*/
        float* dstpLocal0 = dstpLocal[0]; // This is all a bit awkward, but it helps the auto-vectorizer.
        float* dstpLocal1 = dstpLocal[1];
        float* dstpLocal2 = dstpLocal[2];
        float* srcpLocal0 = srcpLocal[0];
        float* srcpLocal1 = srcpLocal[1];
        float* srcpLocal2 = srcpLocal[2];
        
        //memcpy(transformMatrixLocal, transformMatrix, sizeof(matrix));
        for (int x = 0; x < rowSize0; x++) {
            dstpLocal0[x] = srcpLocal0[x] * transformMatrixLocal0 +srcpLocal1[x] * transformMatrixLocal1 + srcpLocal2[x] * transformMatrixLocal2 + transformMatrixLocal3;
            dstpLocal1[x] = srcpLocal0[x] * transformMatrixLocal4 + srcpLocal1[x] * transformMatrixLocal5 + srcpLocal2[x] * transformMatrixLocal6 + transformMatrixLocal7;
            dstpLocal2[x] = srcpLocal0[x] * transformMatrixLocal8 + srcpLocal1[x] * transformMatrixLocal9 + srcpLocal2[x] * transformMatrixLocal10 + transformMatrixLocal11;
        }
    }
    
    return dst;
}

AVSValue __cdecl Create_ColorMatrixTransform(AVSValue args, void* user_data, IScriptEnvironment* env) {
    matrix theMatrix;
    memset(&theMatrix, 0, sizeof(matrix));

    float* matrixPointer = (float*)&theMatrix;
    matrixPointer[0] = args[1].AsFloatf();
    matrixPointer[1] = args[2].AsFloatf();
    matrixPointer[2] = args[3].AsFloatf();
    matrixPointer[3] = args[4].AsFloatf();
    matrixPointer[4] = args[5].AsFloatf();
    matrixPointer[5] = args[6].AsFloatf();
    matrixPointer[6] = args[7].AsFloatf();
    matrixPointer[7] = args[8].AsFloatf();
    matrixPointer[8] = args[9].AsFloatf();
    matrixPointer[9] = args[10].AsFloatf();
    matrixPointer[10] = args[11].AsFloatf();
    matrixPointer[11] = args[12].AsFloatf();

    return new ColorMatrixTransform(args[0].AsClip(),theMatrix, env);
}

const AVS_Linkage* AVS_linkage = 0;

extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit3(IScriptEnvironment * env, const AVS_Linkage* const vectors) {
    AVS_linkage = vectors;
    env->AddFunction("ColorMatrixTransform", "cffffffffffff", Create_ColorMatrixTransform, 0);
    return "Color matrix transform";
}