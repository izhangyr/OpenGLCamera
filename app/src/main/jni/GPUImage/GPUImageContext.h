//
// Created by yongfali on 2016/2/23.
//

#ifndef _E_GPUIMAGECONTEXT_H_
#define _E_GPUIMAGECONTEXT_H_

namespace e{

    class GPUImageContext
    {
    public:
        GPUImageContext(void);
        virtual ~GPUImageContext(void);
        static bool SupportsFastTextureUpload(void);
        static void UseImageProcessingContext(void);
        static GPUImageFramebufferCache* SharedFramebufferCache(void);
    private:

    };

}

#endif
