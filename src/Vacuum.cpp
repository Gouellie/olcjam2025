/**
 * @file Vacuum.cpp
 * @date 25-Aug-2025
 */

#include "Vacuum.h"
#include "MoreMath.h"

void Vacuum::OnCreate()
{
    m_RotationSpeed = orxConfig_GetFloat("RotationSpeed");

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
    orxVECTOR VacuumHead;

    if (olcjam2025::GetInstance().GetIsUsingPad())
    {
        orxVector_Set(&VacuumHead,
            orxInput_GetValue("AimRight") - orxInput_GetValue("AimLeft"),
            orxInput_GetValue("AimDown") - orxInput_GetValue("AimUp"),
            orxFLOAT_0);

        if (!orxVector_AreEqual(&VacuumHead, &orxVECTOR_0))
        {
            orxVector_Normalize(&VacuumHead, &VacuumHead);
            m_DesiredRotation = get_angle(VacuumHead);
        }
    }
    else if (orxOBJECT* pstVessel = orxOBJECT(orxStructure_Get(olcjam2025::GetInstance().GetActiveVesselID())))
    {
        orxVECTOR mousePosition, vesselPosition;
        orxMouse_GetPosition(&mousePosition);
        orxObject_GetWorldPosition(pstVessel, &vesselPosition);
        orxRender_GetScreenPosition(&vesselPosition, orxViewport_Get("MainViewport"), &vesselPosition);

        orxVector_Sub(&VacuumHead, &mousePosition, &vesselPosition);

        m_DesiredRotation = get_angle(VacuumHead);
    }

    SetRotation(lerp_angle(GetRotation(), m_DesiredRotation, _rstInfo.fDT * m_RotationSpeed));

    if (!m_IsBeamLocked && orxInput_HasBeenActivated("Vacuum"))
    {
        SetIsBeamActive(orxTRUE);
    }
    else if (orxInput_HasBeenDeactivated("Vacuum"))
    {
        SetIsBeamActive(orxFALSE);
    }
}

void Vacuum::SetIsBeamLocked(orxBOOL isBeamLocked)
{
    m_IsBeamLocked = isBeamLocked;
    if (m_IsBeamLocked) 
    {
        SetIsBeamActive(orxFALSE);
    }
}

void Vacuum::SetIsBeamActive(orxBOOL isBeamActive)
{
    if (isBeamActive)
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
    else
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
        orxConfig_PushSection("Tally");
        orxS32 tally = orxConfig_GetS32(_poCollider->GetName());
        orxConfig_SetS32(_poCollider->GetName(), tally + 1);
        orxConfig_PopSection();

        AddFX("VacuumBlowFX");

        _poCollider->SetLifeTime(0);
    }
}
