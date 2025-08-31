/**
 * @file Vacuum.cpp
 * @date 25-Aug-2025
 */

#include "Vacuum.h"
#include "Gauge.h"
#include "MoreMath.h"

void Vacuum::OnCreate()
{
    Gauge* poGaugeBoost = (Gauge*)olcjam2025::GetInstance().GetObject("Runtime", "GaugeShapes");
    m_GaugeShapesGUID = poGaugeBoost->GetGUID();

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

    Gauge* poGaugeBoost = (Gauge*)olcjam2025::GetInstance().GetObject(m_GaugeShapesGUID);

    if (orxInput_HasBeenActivated("Vacuum"))
    {
        if (m_IsBeamLocked)
        {
            if (!m_IsIsVesselDocked) 
            {
                AddTrack("VacuumBeamIsLockedTrack");
            }
        }
        else if (poGaugeBoost->GetIsMaxedOut())
        {
            poGaugeBoost->AddTrack("GaugeShapesIsFullTrack");
            SetIsBeamActive(orxFALSE);
        }
        else 
        {
            SetIsBeamActive(orxTRUE);
        }
    }
    else if (orxInput_HasBeenDeactivated("Vacuum"))
    {
        SetIsBeamActive(orxFALSE);
    }
    else if (poGaugeBoost->GetIsMaxedOut())
    {
        if (m_IsBeamActive) 
        {
            poGaugeBoost->AddTrack("GaugeShapesIsFullTrack");
            SetIsBeamActive(orxFALSE);
        }
    }
}

void Vacuum::SetIsBeamLocked(orxBOOL isBeamLocked)
{
    m_IsBeamLocked = isBeamLocked;
    if (m_IsBeamLocked) 
    {
        if (m_IsBeamActive) 
        {
            AddTrack("VacuumBeamIsLockedTrack");
        }

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

    m_IsBeamActive = isBeamActive;
}

void VacuumHead::OnCreate()
{
    Gauge* poGaugeBoost = (Gauge*)olcjam2025::GetInstance().GetObject("Runtime", "GaugeShapes");
    m_GaugeShapesGUID = poGaugeBoost->GetGUID();

    m_WaitForFire = orxConfig_GetFloat("WaitForFire");
    m_FireSpeed   = orxConfig_GetFloat("FireSpeed");
}

void VacuumHead::OnDelete()
{
}

void VacuumHead::Update(const orxCLOCK_INFO& _rstInfo)
{
    Vacuum* parent = (Vacuum*)GetParent();
    if (parent->GetIsBeamLocked())
    {
        return;
    }

    if (orxInput_IsActive("Fire"))
    {
        if (CanFireShape(_rstInfo)) 
        {
            FireShape();
        }
    }
}

orxBOOL VacuumHead::CanFireShape(const orxCLOCK_INFO& _rstInfo)
{
    m_FireDelay = orxMAX(orxFLOAT_0, m_FireDelay - _rstInfo.fDT);
    return m_FireDelay == orxFLOAT_0;
}

void VacuumHead::FireShape()
{
    m_FireDelay = m_WaitForFire;

    Gauge* poGaugeShapes = (Gauge*)olcjam2025::GetInstance().GetObject(m_GaugeShapesGUID);
    if (m_collection.empty())
    {
        AddTrack("VacuumFireShapeShootingBlankTrack");
        poGaugeShapes->AddTrack("GaugeShapesIsEmptyTrack");
        m_FireDelay = m_WaitForFire;
        return;
    }

    orxU64 shapeID = m_collection.top();
    m_collection.pop();

    poGaugeShapes->SetCurrentValue(orxS2F(m_collection.size()));

    orxVECTOR pos, impulse;

    if (ScrollObject* poShape = olcjam2025::GetInstance().GetObject(shapeID)) 
    {
        GetOwnedChild()->GetPosition(pos, orxTRUE);

        poShape->SetPosition(pos, orxTRUE);
        poShape->Enable(orxTRUE);

        orxConfig_PushSection("Tally");
        orxS32 tally = orxConfig_GetS32(poShape->GetName());
        orxConfig_SetS32(poShape->GetName(), tally - 1);
        orxConfig_PopSection();

        orxFLOAT rotation = GetRotation(orxTRUE);

        orxVector_Copy(&impulse, &orxVECTOR_Y);
        orxVector_Mulf(&impulse, &impulse, m_FireSpeed);
        orxVector_2DRotate(&impulse, &impulse, rotation);
        orxObject_SetSpeed(poShape->GetOrxObject(), &impulse);

        AddTrack("VacuumFireShapeTrack");
    }
}


void VacuumHead::OnCollide(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart, const orxVECTOR& _rvPosition, const orxVECTOR& _rvNormal)
{
    if (orxInput_IsActive("Vacuum")) 
    {
        orxConfig_PushSection("Tally");
        orxS32 tally = orxConfig_GetS32(_poCollider->GetName());
        orxConfig_SetS32(_poCollider->GetName(), tally + 1);
        orxConfig_PopSection();

        AddTrack("VacuumBlowShapeTrack");

        Gauge* poGaugeBoost = (Gauge*)olcjam2025::GetInstance().GetObject(m_GaugeShapesGUID);
        poGaugeBoost->SetCurrentValue(poGaugeBoost->GetCurrentValue() + 1.0f);

        m_collection.push(_poCollider->GetGUID());
        _poCollider->Enable(orxFALSE);
    }
}


