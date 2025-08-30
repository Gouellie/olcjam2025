/**
 * @file Vessel.cpp
 * @date 25-Aug-2025
 */

#include "Vessel.h"
#include "Vacuum.h"

void Vessel::OnCreate()
{
    olcjam2025::GetInstance().SetActiveVesselID(GetGUID());

    // Init the camera
    m_Camera = orxObject_CreateFromConfig("Camera");
    orxObject_GetPosition(m_Camera, &m_PreviousCameraPos);

    // Required so that the Camera is moving in local space
    orxObject_SetOwner(m_Camera, GetOrxObject());

    m_CameraBox.SetCamera(m_Camera);
    m_CameraBox.SetTarget(this);

    for (orxOBJECT* pstChild = orxObject_GetOwnedChild(GetOrxObject());
        pstChild != orxNULL;
        pstChild = orxObject_GetOwnedSibling(pstChild))
    {
        if (orxString_Compare(orxObject_GetName(pstChild), "Vacuum") == 0)
        {
            m_VacuumGUID = orxStructure_GetGUID(pstChild);
            m_CameraBox.SetVacuumID(m_VacuumGUID);
            break;
        }
    }

    orxConfig_GetListVector("MovingSpeed", 0, &m_vPlayerSpeed);
    orxConfig_GetListVector("MovingSpeed", 1, &m_vPlayerHighSpeed);

    m_ShapesImpulseMultiplier = orxConfig_GetFloat("ShapesImpulseMultiplier");
    m_ShapesCollisionFlag = (orxU16)orxPhysics_GetCollisionFlagValue("shape");

    //orxOBJECT* pstCompass = orxObject_CreateFromConfig("VesselCompass");
    //orxObject_SetOwner(pstCompass, GetOrxObject());
}

void Vessel::OnDelete()
{
}

void Vessel::Update(const orxCLOCK_INFO &_rstInfo)
{
    if (m_IsDocking == false) 
    {
        MovePlayer(_rstInfo);

        if (orxOBJECT* pstNearest = GetNearestStarBase()) 
        {
            DrawCompassToObject(pstNearest);
        }

        if (m_IsDocked == false) 
        {
            // Zoom Out?
            if (orxInput_HasBeenActivated("Zoom"))
            {
                m_IsZooming = orxTRUE;
                orxObject_AddTimeLineTrack(m_Camera, "ZoomOut");
            }
            // Zoom In?
            else if (orxInput_HasBeenDeactivated("Zoom"))
            {
                m_IsZooming = orxFALSE;
                orxObject_AddTimeLineTrack(m_Camera, "ZoomIn");
            }
        }
    }

    m_CameraBox.Update(_rstInfo);
}

void Vessel::MovePlayer(const orxCLOCK_INFO& _rstInfo)
{
    // Update player position
    orxVECTOR PlayerMove, PlayerPos, PlayerSpeed;

    orxVector_Set(&PlayerMove,
        orxInput_GetValue("Right") - orxInput_GetValue("Left"),
        orxInput_GetValue("Down") - orxInput_GetValue("Up"),
        orxFLOAT_0);

    if (orxVector_AreEqual(&PlayerMove, &orxVECTOR_0))
    {
        if (m_IsMoving) 
        {
            AddTrack("VesselMovingStopTrack");
        }
        m_IsMoving = orxFALSE;
    }
    else 
    {
        orxVector_Normalize(&PlayerMove, &PlayerMove);

        orxVector_Mulf(&PlayerMove,
            orxVector_Mul(&PlayerMove, &PlayerMove, orxVector_Lerp(&PlayerSpeed, &m_vPlayerSpeed, &m_vPlayerHighSpeed, orxInput_GetValue("Fast"))),
            _rstInfo.fDT);

        orxObject_SetPosition(GetOrxObject(), orxVector_Add(&PlayerPos,
            orxObject_GetPosition(GetOrxObject(), &PlayerPos),
            orxVector_Round(&PlayerMove, &PlayerMove)));

        if (!m_IsMoving)
        {
            AddTrack("VesselMovingStartTrack");
        }

        m_IsMoving = orxTRUE;
    }
}

void Vessel::SetIsDocking(orxBOOL isDocking)
{
    m_IsDocking = isDocking;
    if (m_IsDocking) 
    {
        if (Vacuum* pstVacuum = (Vacuum*)olcjam2025::GetInstance().GetObject(m_VacuumGUID))
        {
            pstVacuum->SetIsBeamLocked(orxTRUE);
            pstVacuum->Enable(orxFALSE, orxTRUE);
            m_CameraBox.SetIsVacuumLocked(orxTRUE);

            if (m_IsZooming) 
            {
                m_IsZooming = orxFALSE;
                orxObject_AddTimeLineTrack(m_Camera, "ZoomIn");
            }
        }
    }
}

void Vessel::SetIsDocked(orxBOOL isDocked)
{
    m_IsDocked = isDocked;
    if (Vacuum* pstVacuum = (Vacuum*)olcjam2025::GetInstance().GetObject(m_VacuumGUID))
    {
        pstVacuum->SetIsBeamLocked(m_IsDocked);
        pstVacuum->Enable(m_IsDocked == false, orxTRUE);
        m_CameraBox.SetIsVacuumLocked(m_IsDocked);
    }
}

void Vessel::OnCollide(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart, const orxVECTOR& _rvPosition, const orxVECTOR& _rvNormal) 
{
    if (orxBody_GetPartSelfFlags(_pstColliderPart) == m_ShapesCollisionFlag) 
    {
        orxVECTOR impulse;
        orxVector_Neg(&impulse, &_rvNormal);
        orxVector_Mulf(&impulse, &impulse, m_ShapesImpulseMultiplier);
        orxBody_ApplyImpulse(orxBody_GetPartBody(_pstColliderPart), &impulse, &_rvPosition);
    }
}

orxOBJECT* Vessel::GetNearestStarBase()
{
    orxOBJECT* pstObject = orxNULL, * pstNearest = orxNULL;
    orxVECTOR worldPos, pos;
    GetPosition(worldPos, orxTRUE);

    orxFLOAT nearestDistance = orxFLOAT_MAX;

    do {
        orxSTRINGID id = orxString_GetID("Starbase");
        pstObject = orxObject_GetNext(pstObject, id);
        if (pstObject != orxNULL) {
            orxObject_GetWorldPosition(pstObject, &pos);
            orxFLOAT distance = orxVector_GetDistance(&pos, &worldPos);
            if (distance < nearestDistance)
            {
                nearestDistance = distance;
                pstNearest = pstObject;
            }
        }
    } while (pstObject);

    if (pstNearest != orxNULL)
    {
        return pstNearest;
    }

    return nullptr;
}

void Vessel::DrawCompassToObject(orxOBJECT* _pstDestination)
{
    orxVIEWPORT* pstViewport;
    pstViewport = orxViewport_Get("MainViewport");

    orxVECTOR vesselPosition, destinationPos;
    GetPosition(vesselPosition, orxTRUE);
    orxObject_GetWorldPosition(_pstDestination, &destinationPos);

    orxRender_GetScreenPosition(&vesselPosition, pstViewport, &vesselPosition);
    orxRender_GetScreenPosition(&destinationPos, pstViewport, &destinationPos);

    //orxDisplay_DrawLine(&vesselPosition, &destinationPos, orxRGBA_Set(255,255,255,255));
}
