/**
 * @file Starbase.h
 * @date 28-Aug-2025
 */

#ifndef __STARBASE_H__
#define __STARBASE_H__

#include "olcjam2025.h"

/** Starbase Class
 */
class Starbase : public ScrollObject
{
public:


protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);

                void            OnCollide(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart, const orxVECTOR& _rvPosition, const orxVECTOR& _rvNormal);

private:

                orxU64          m_RadialMenuGUID;
                orxFLOAT        m_DockingSpeed;
                orxBOOL         m_bIsDocking;
                orxBOOL         m_bIsDocked;

                orxVECTOR       m_DockedPosition;
};

#endif // __STARBASE_H__
