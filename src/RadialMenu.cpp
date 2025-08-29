/**
 * @file RadialMenu.cpp
 * @date 29-Aug-2025
 */

#include "RadialMenu.h"
#include "MoreMath.h"

void RadialMenu::OnCreate()
{
    m_RotationSpeed = orxConfig_GetFloat("RotationSpeed");
    m_Radius = orxConfig_GetFloat("Radius");
    m_AngleOffset = orxConfig_GetFloat("AngleOffset") * orxMATH_KF_DEG_TO_RAD;

    m_AimCollisionFlag = (orxU16)orxPhysics_GetCollisionFlagValue("radialMenuSelectionAim");
    m_OptionCollisionFlag = (orxU16)orxPhysics_GetCollisionFlagValue("radialMenuOption");

    // always call before creating the selection aim
    LayoutOptions();

    m_pstSelectionAim = orxObject_CreateFromConfig("RadialMenuSelectionAim");
    orxObject_SetParent(m_pstSelectionAim, GetOrxObject());
    orxObject_SetOwner(m_pstSelectionAim, GetOrxObject());

    orxObject_SetRotation(m_pstSelectionAim, GetAimingAngle());
}

void RadialMenu::OnDelete()
{
}

void RadialMenu::Update(const orxCLOCK_INFO &_rstInfo)
{
    m_DesiredRotation = GetAimingAngle();

    orxFLOAT realRotation = lerp_angle(orxObject_GetRotation(m_pstSelectionAim), m_DesiredRotation, _rstInfo.fDT * m_RotationSpeed);
    orxObject_SetRotation(m_pstSelectionAim, realRotation);

    orxVECTOR rayOrigin, rayDirection, dummy;
    GetPosition(rayOrigin);
    orxVector_Set(&rayDirection, 0.0f, m_Radius * 2.0f, 0.0f);
    orxVector_2DRotate(&rayDirection, &rayDirection, realRotation);

    orxOBJECT* pstObject = orxNULL;
    pstObject = orxObject_Raycast(&rayOrigin, &rayDirection, m_AimCollisionFlag, m_OptionCollisionFlag, orxTRUE, &dummy, &dummy);
    if (pstObject != orxNULL)
    {
        if (RadialMenuOption* button = (RadialMenuOption*)olcjam2025::GetInstance().GetObject(orxStructure_GetGUID(pstObject)))
        {
            if (m_pstSelectionFocus != pstObject)
            {
                orxObject_AddFX(pstObject, "RadialMenuOptionFocused");
                m_pstSelectionFocusLabel = orxObject_CreateFromConfig(button->GetLabelName());
                orxObject_SetParent(m_pstSelectionFocusLabel, GetOrxObject());
                orxObject_SetOwner(m_pstSelectionFocusLabel, GetOrxObject());
            }

            if (button->GetIsSingleShot()) 
            {
                if (orxInput_HasBeenActivated("Activate"))
                {
                    orxInput_SetValue(button->GetInputName(), orxFLOAT_1);
                }
            }
            else if (orxInput_IsActive("Activate"))
            {
                orxInput_SetValue(button->GetInputName(), orxFLOAT_1);
            }
        }
    }
    else if (m_pstSelectionFocus != orxNULL) 
    {
        orxObject_AddFX(m_pstSelectionFocus, "RadialMenuOptionUnfocused");
        orxObject_SetLifeTime(m_pstSelectionFocusLabel, orxFLOAT_0);
    }

    m_pstSelectionFocus = pstObject;
}

void RadialMenu::LayoutOptions()
{
    orxVECTOR pos;
    orxVector_Set(&pos, 0.0f, -m_Radius, 0.0f);
    orxS32 count = orxConfig_GetListCount("ChildList");
    orxFLOAT angleStep = (orxMATH_KF_2_PI / orxS2F(count));
    orxOBJECT* pstChild = orxObject_GetOwnedChild(GetOrxObject());

    if (m_AngleOffset) 
    {
        orxVector_2DRotate(&pos, &pos, m_AngleOffset);
    }

    for (int i = 0; pstChild != orxNULL; i++) 
    {
        orxObject_SetRotation(pstChild, angleStep*orxS2F(i) + m_AngleOffset);
        orxObject_SetPosition(pstChild, &pos);
        orxVector_2DRotate(&pos, &pos, angleStep);
        pstChild = orxObject_GetOwnedSibling(pstChild);
    }
}

orxFLOAT RadialMenu::GetAimingAngle()
{
    orxVECTOR aimingDirection;

    if (olcjam2025::GetInstance().GetIsUsingPad())
    {
        orxVector_Set(&aimingDirection,
            orxInput_GetValue("AimRight") - orxInput_GetValue("AimLeft"),
            orxInput_GetValue("AimDown") - orxInput_GetValue("AimUp"),
            orxFLOAT_0);

        return get_angle(aimingDirection);
    }
    else
    {
        orxVECTOR mousePosition, position;
        orxMouse_GetPosition(&mousePosition);
        GetPosition(position);
        orxRender_GetScreenPosition(&position, orxViewport_Get("HUDViewport"), &position);
        orxVector_Sub(&aimingDirection, &mousePosition, &position);

        return get_angle(aimingDirection);
    }

    return orxFLOAT_0;
}

void RadialMenuOption::OnCreate()
{
    m_InputName = orxConfig_GetString("InputName");
    m_LabelName = orxConfig_GetString("Label");
    m_IsSingleShot = orxConfig_GetBool("IsSingleShot");
}

void RadialMenuOption::OnDelete()
{
}

void RadialMenuOption::Update(const orxCLOCK_INFO& _rstInfo)
{
}
