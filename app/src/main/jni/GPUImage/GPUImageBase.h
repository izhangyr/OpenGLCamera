//
// Created by yongfali on 2016/2/23.
//

#ifndef _E_GPUIMAGEBASE_H_
#define _E_GPUIMAGEBASE_H_

namespace e {

    struct Size{
        int cx;
        int cy;
        Size(void)
        {
            cx = cy = 0;
        }

        Size(int cx, int cy)
        {
            this->cx = cx;
            this->cy = cy;
        }

        Size(const Size & s)
        {
            cx = s.cx;
            cy = s.cy;
        }
    };

}

#endif
