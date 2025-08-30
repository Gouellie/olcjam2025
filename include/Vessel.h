/**
 * @file Vessel.h
 * @date 25-Aug-2025
 */

#ifndef __VESSEL_H__
#define __VESSEL_H__

#include "olcjam2025.h"

#include "CameraBox.h"

/** Vessel Class
 */
class Vessel : public ScrollObject
{
public:

                void            SetIsDocking(orxBOOL isDocking);
                void            SetIsDocked(orxBOOL isDocked);

protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);

                void            OnCollide(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart, const orxVECTOR& _rvPosition, const orxVECTOR& _rvNormal);

private:

                orxVECTOR       m_PreviousCameraPos;
                orxOBJECT*      m_Camera;
                CameraBox       m_CameraBox;

                orxVECTOR       m_vPlayerSpeed;
                orxVECTOR       m_vPlayerHighSpeed;

                orxBOOL         m_IsZooming;
                orxBOOL         m_IsDocking;
                orxBOOL         m_IsDocked;

                orxU64          m_VacuumGUID;

                orxFLOAT        m_ShapesImpulseMultiplier;
                orxU16          m_ShapesCollisionFlag;

                orxBOOL         m_IsMoving;

private:

                void            MovePlayer(const orxCLOCK_INFO& _rstInfo);
                orxOBJECT*      GetNearestStarBase();
                void            DrawCompassToObject(orxOBJECT* pos);
                orxBOOL         GetIsObjectInView(const orxVIEWPORT* _pstViewport, const orxVECTOR& _worldPosition);
                orxBOOL         GetCompassWorldPositionForViewport(const orxVIEWPORT* _pstViewport, const orxVECTOR& _origin, const orxVECTOR& _dest, orxVECTOR& result);
};

#endif // __VESSEL_H__
