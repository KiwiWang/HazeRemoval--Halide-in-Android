#include <jni.h>
#include <android/log.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "halide_generated.h"
#include <HalideRuntime.h>

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"halide_native",__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"halide_native",__VA_ARGS__)

#define DEBUG 1

extern "C" void halide_set_error_handler(int (*handler)(void *user_context, const char *));
extern "C" int halide_host_cpu_count();
extern "C" int64_t halide_current_time_ns();
extern "C" int halide_copy_to_host(void *, buffer_t *);
extern "C" int halide_copy_to_dev(void *, buffer_t *);
extern "C" int halide_dev_malloc(void *, buffer_t *);
extern "C" int halide_dev_free(void *, buffer_t *);

int handler(void */* user_context */, const char *msg) {
    LOGE("%s", msg);
}

extern "C" {
JNIEXPORT jintArray JNICALL Java_com_example_hellohalide_CameraActivity_processFrame(
    JNIEnv *env, jobject obj, jintArray jSrc, jint j_w, jint j_h) {

    const int w = j_w, h = j_h;

    halide_set_error_handler(handler);

    //unsigned char *src = (unsigned char *)env->GetByteArrayElements(jSrc, NULL);
    jint *src_org = (jint *)env->GetIntArrayElements(jSrc, NULL);
    uint8_t src[w*h*3];
    for(int i=0; i<w*h*3; i++)
        src[i] = (uint8_t)(src_org[i]);

    if (!src) {
        LOGD("src is null\n");
        return NULL;
    }


    /*
    static bool first_call = true;
    static unsigned counter = 0;
    static unsigned times[16];
    if (first_call) {
        LOGD("According to Halide, host system has %d cpus\n", halide_host_cpu_count());
        LOGD("Resetting buffer format");
        first_call = false;
        for (int t = 0; t < 16; t++) times[t] = 0;
    }
    */

    uint8_t output[w*h*3];
    uint8_t *dst = &output[0];
    // If we're using opencl, use the gpu backend for it.
    halide_set_ocl_device_type("gpu");

    buffer_t srcBuf = {0};
    buffer_t dstBuf = {0};

    if (dst) {
        srcBuf.host = (uint8_t *)src;
        srcBuf.host_dirty = true;
        srcBuf.extent[0] = w;
        srcBuf.extent[1] = h;
        srcBuf.extent[2] = 3;
        srcBuf.extent[3] = 0;
        srcBuf.stride[0] = 1;
        srcBuf.stride[1] = w;
        srcBuf.stride[2] = w*h;
        srcBuf.min[0] = 0;
        srcBuf.min[1] = 0;
        srcBuf.elem_size = 1;

        dstBuf.host = dst;
        dstBuf.extent[0] = w;
        dstBuf.extent[1] = h;
        dstBuf.extent[2] = 3;
        dstBuf.extent[3] = 0;
        dstBuf.stride[0] = 1;
        dstBuf.stride[1] = w;
        dstBuf.stride[2] = w*h;
        dstBuf.min[0] = 0;
        dstBuf.min[1] = 0;
        dstBuf.elem_size = 1;

        //int64_t t1 = halide_current_time_ns();
        halide_generated(&srcBuf, &dstBuf);

        if (dstBuf.dev) {
            halide_copy_to_host(NULL, &dstBuf);
        }

        //int64_t t2 = halide_current_time_ns();
        //unsigned elapsed_us = (t2 - t1)/1000;

        /*
        times[counter & 15] = elapsed_us;
        counter++;
        unsigned min = times[0];
        for (int i = 1; i < 16; i++) {
            if (times[i] < min) min = times[i];
        }
        LOGD("Time taken: %d (%d)", elapsed_us, min);
        */
    }

    jint fill[w*h*3];
    jintArray result;
    result = env->NewIntArray(w*h*3);
    if(result==NULL){
        LOGE("result array fails");
        return NULL;
    }
    int ind;
    int channel_size = w*h;
    for(int j=0; j<h;j++){
        for(int i=0; i<w; i++){
            ind = j*w+i;
            fill[ind] = (jint)output[ind];
            fill[channel_size + ind] = (jint)output[channel_size+ind];
            fill[2*channel_size + ind] = (jint)output[2*channel_size + ind];
        }// for int i
    }// for int j
    env->SetIntArrayRegion(result, 0, w*h*3, fill);
    env->ReleaseIntArrayElements(jSrc, (jint *)src, 0);

    return result;
}
}
