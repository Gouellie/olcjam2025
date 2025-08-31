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

                orxFLOAT        GetShapeDamageBlow() const { return m_ShapeDamageBlow; }
                orxFLOAT        GetShapeDamageBounce() const { return m_ShapeDamageBounce; }
                void            SetIsInTractorBeam(orxBOOL isInTractorBeam) { m_IsInTractorBeam = isInTractorBeam; }

protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);

private:

                orxVECTOR       m_vOriginalScale;
                orxVECTOR       m_vPreviousSpeed;

                orxBOOL         m_IsInTractorBeam;
                orxFLOAT        m_ShapeDamageBlow;
                orxFLOAT        m_ShapeDamageBounce;
};

#endif // __SHAPE_H__
