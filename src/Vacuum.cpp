/**
 * @file Vacuum.cpp
 * @date 25-Aug-2025
 */

#include "Vacuum.h"

static orxFLOAT orxFASTCALL angle_difference(orxFLOAT p_from, orxFLOAT p_to) {
    orxFLOAT difference = orxMath_Mod(p_to - p_from, orxMATH_KF_2_PI);
    return orxMath_Mod(2.0f * difference, orxMATH_KF_2_PI) - difference;
}

static orxFLOAT orxFASTCALL lerp_angle(orxFLOAT p_from, orxFLOAT p_to, orxFLOAT p_weight) {
    return p_from + angle_difference(p_from, p_to) * p_weight;
}

static orxFLOAT orxFASTCALL get_angle(orxVECTOR& p_v) 
{
    return orxMath_ATan(p_v.fY, p_v.fX) - orxMATH_KF_PI_BY_2;
}

void Vacuum::OnCreate()
{
    orxMouse_GetPosition(&m_previousMousePos);

    for (orxOBJECT* pstChild = orxObject_GetOwnedChild(GetOrxObject());
        pstChild != orxNULL;
        pstChild = orxObject_GetOwnedSibling(pstChild))
    {
        if (orxString_Compare(orxObject_GetName(pstChild), "VacuumHead") == 0)
        {
            m_vacuumHeadGUID = orxStructure_GetGUID(pstChild);
            break;
        }
    }
}

void Vacuum::OnDelete()
{
}

void Vacuum::Update(const orxCLOCK_INFO &_rstInfo)
{
    PushConfigSection();

    orxVECTOR mousePosition;
    orxMouse_GetPosition(&mousePosition);

    orxVECTOR VacuumHead;
    if (orxVector_AreEqual(&mousePosition, &m_previousMousePos)) 
    {
        orxVector_Set(&VacuumHead,
            orxInput_GetValue("AimRight") - orxInput_GetValue("AimLeft"),
            orxInput_GetValue("AimDown") - orxInput_GetValue("AimUp"),
            orxFLOAT_0);
    }
    else
    {
        if (!orxVector_AreEqual(&VacuumHead, &orxVECTOR_0))
        {
            orxConfig_PushSection("Runtime");
            orxU64 vesselGUID = orxConfig_GetU64("Vessel");
            orxConfig_PopSection();

            if (orxOBJECT* pstVessel = orxOBJECT(orxStructure_Get(vesselGUID)))
            {
                orxVECTOR vesselPosition, mousePositionWorld;
                orxRender_GetWorldPosition(&mousePosition, orxNULL, &mousePositionWorld);
                orxObject_GetPosition(pstVessel, &vesselPosition);
                orxVector_Sub(&VacuumHead, &mousePositionWorld, &vesselPosition);
                orxVector_Normalize(&VacuumHead, &VacuumHead);
                m_DesiredRotation = get_angle(VacuumHead);
            }

            m_previousMousePos = mousePosition;
        }
    }


    SetRotation(lerp_angle(GetRotation(), m_DesiredRotation, _rstInfo.fDT * orxConfig_GetFloat("RotationSpeed")));

    if (orxInput_HasBeenActivated("Vacuum"))
    {
        if (orxOBJECT* vacuumHead = orxOBJECT(orxStructure_Get(m_vacuumHeadGUID))) 
        {
            orxObject_SetTargetAnim(vacuumHead, "Active");
        }
        orxOBJECT* vacuumBeam = orxObject_CreateFromConfig("VacuumBeam");
        orxObject_SetOwner(vacuumBeam, GetOrxObject());
        orxObject_SetParent(vacuumBeam, GetOrxObject());
        m_vacuumBeamGUID = orxStructure_GetGUID(vacuumBeam);
    }
    else if (orxInput_HasBeenDeactivated("Vacuum"))
    {
        if (orxOBJECT* vacuumHead = orxOBJECT(orxStructure_Get(m_vacuumHeadGUID)))
        {
            orxObject_SetTargetAnim(vacuumHead, "Inactive");
        }
        if (orxOBJECT* vacuumBeam = orxOBJECT(orxStructure_Get(m_vacuumBeamGUID)))
        {
            orxObject_SetLifeTime(vacuumBeam, orxFLOAT_0);
        }
    }

    PopConfigSection();
}

void VacuumHead::OnCreate()
{
}

void VacuumHead::OnDelete()
{
}

void VacuumHead::Update(const orxCLOCK_INFO& _rstInfo)
{
}

void VacuumHead::OnCollide(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart, const orxVECTOR& _rvPosition, const orxVECTOR& _rvNormal)
{
    if (orxInput_IsActive("Vacuum")) 
    {
        _poCollider->SetLifeTime(0);
    }
}
