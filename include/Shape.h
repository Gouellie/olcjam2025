/**
 * @file Shape.h
 * @date 27-Aug-2025
 */

#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "olcjam2025.h"

/** Shape Class
 */
class Shape : public ScrollObject
{
public:


protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);

private:

                orxVECTOR       m_vOriginalScale;
                orxVECTOR       m_vPreviousSpeed;
};

#endif // __SHAPE_H__
