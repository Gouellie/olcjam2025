/**
 * @file Vessel.h
 * @date 25-Aug-2025
 */

#ifndef __VESSEL_H__
#define __VESSEL_H__

#include "olcjam2025.h"

#include"CameraBox.h"

/** Vessel Class
 */
class Vessel : public ScrollObject
{
public:


protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);

private:

                orxVECTOR       m_PreviousCameraPos;
                orxOBJECT*      m_Camera;
                CameraBox       m_CameraBox;

                orxVECTOR       m_vPlayerSpeed;
                orxVECTOR       m_vPlayerHighSpeed;
};

#endif // __VESSEL_H__
