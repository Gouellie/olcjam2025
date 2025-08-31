/**
 * @file Gauge.h
 * @date 30-Aug-2025
 */

#ifndef __GAUGE_H__
#define __GAUGE_H__

#include "olcjam2025.h"

/** Gauge Class
 */
class Gauge : public ScrollObject
{
public:


protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);


private:

                orxOBJECT*      m_pstGaugeFill;
};

#endif // __GAUGE_H__
