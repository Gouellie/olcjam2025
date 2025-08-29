/**
 * @file RadialMenu.h
 * @date 29-Aug-2025
 */

#ifndef __RADIAL_MENU_H__
#define __RADIAL_MENU_H__

#include "olcjam2025.h"

class RadialMenuOption : public ScrollObject 
{
public:

    const orxSTRING GetInputName() { return m_InputName; }
    const orxSTRING GetLabelName() { return m_LabelName; }
    orxBOOL         GetIsSingleShot() { return m_IsSingleShot; }

protected:

    void            OnCreate();
    void            OnDelete();
    void            Update(const orxCLOCK_INFO& _rstInfo);

private:

    const orxSTRING m_InputName;
    const orxSTRING m_LabelName;

    orxBOOL         m_IsSingleShot;

};

/** RadialMenu Class
 */
class RadialMenu : public ScrollObject
{
public:


protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);

private:
                orxFLOAT        GetAimingAngle();
                void            LayoutOptions();

                orxOBJECT*      m_pstSelectionAim;
                orxOBJECT*      m_pstSelectionFocus         = orxNULL;
                orxOBJECT*      m_pstSelectionFocusLabel    = orxNULL;
                orxFLOAT        m_RotationSpeed;
                orxFLOAT        m_Radius;
                orxFLOAT        m_AngleOffset;
                orxFLOAT        m_DesiredRotation = orxFLOAT_0;

                orxU16          m_AimCollisionFlag;
                orxU16          m_OptionCollisionFlag;
};

#endif // __RADIAL_MENU_H__


