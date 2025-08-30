/**
 * @file Vessel.cpp
 * @date 25-Aug-2025
 */

#include "Vessel.h"
#include "Vacuum.h"
#include "MoreMath.h"

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

    m_pstCompass = orxObject_CreateFromConfig("VesselCompass");
    orxObject_SetOwner(m_pstCompass, GetOrxObject());
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

    orxVECTOR vesselPosition, destinationPos, res;
    GetPosition(vesselPosition, orxTRUE);
    orxObject_GetWorldPosition(_pstDestination, &destinationPos);

    if (GetIsObjectInView(pstViewport, destinationPos))
    {
        /* Object is in view, no compass needed */
        orxObject_Enable(m_pstCompass, orxFALSE);
        return;
    }
    
    if (GetCompassWorldPositionForViewport(pstViewport, vesselPosition, destinationPos, res))
    {
        if (orxRender_GetScreenPosition(&res, pstViewport, &res) != orxNULL) 
        {
            orxObject_Enable(m_pstCompass, orxTRUE);
            orxVECTOR facing;
            orxVector_Sub(&facing, &vesselPosition, &destinationPos);
            orxObject_SetRotation(m_pstCompass, get_angle(facing));
            orxRender_GetWorldPosition(&res, orxViewport_Get("HUDViewport"), &res);
            orxObject_SetWorldPosition(m_pstCompass, &res);
        }
    }
}

orxBOOL Vessel::GetIsObjectInView(const orxVIEWPORT* _pstViewport,const orxVECTOR& _worldPosition)
{
    orxVECTOR dummy;
    orxRender_GetScreenPosition(&_worldPosition, _pstViewport, &dummy);
    return orxRender_GetWorldPosition(&dummy, _pstViewport, &dummy) != orxNULL;
}

orxBOOL Vessel::GetCompassWorldPositionForViewport(const orxVIEWPORT* _pstViewport, const orxVECTOR& _origin, const orxVECTOR& _dest, orxVECTOR& result)
{
    orxCAMERA* pstCamera;
    orxAABOX  stFrustum;
    orxVECTOR vCameraPosition;

    pstCamera = orxViewport_GetCamera(_pstViewport);

    /* Gets camera position */
    orxFrame_GetPosition(orxCamera_GetFrame(pstCamera), orxFRAME_SPACE_GLOBAL, &vCameraPosition);

    /* Gets its frustum */
    orxCamera_GetFrustum(pstCamera, &stFrustum);

    /* To adjust frustum to scale */
    orxOBJECT* parent = orxOBJECT(orxCamera_GetParent(pstCamera));
    orxVECTOR scale;
    orxObject_GetScale(parent, &scale);

    /* Add offset from screen border */
    orxVECTOR frameOffset;
    orxVector_Set(&frameOffset, 100.0f, 100.0f, 0.0f);
    orxVector_Mulf(&frameOffset, &frameOffset, scale.fX);

    /* Get TopLeft-BottomRight Position in WorldSpace */
    orxVECTOR topLeftPos, bottomRightPos, bottomLeftPos, topRightPos;
    orxVector_Copy(&topLeftPos, &stFrustum.vTL);
    orxVector_Mulf(&topLeftPos, &topLeftPos, scale.fX);
    orxVector_Neg(&bottomRightPos, &topLeftPos);
    orxVector_Add(&topLeftPos, &topLeftPos, &vCameraPosition);
    orxVector_Add(&topLeftPos, &topLeftPos, &frameOffset);
    orxVector_Add(&bottomRightPos, &bottomRightPos, &vCameraPosition);
    orxVector_Sub(&bottomRightPos, &bottomRightPos, &frameOffset);

    /* Set BottomLeft TopRight */
    orxVector_Set(&bottomLeftPos, topLeftPos.fX, bottomRightPos.fY, orxFLOAT_0);
    orxVector_Set(&topRightPos, bottomRightPos.fX, topLeftPos.fY, orxFLOAT_0);

    /* Find intersection with Frame if any */
    orxVECTOR intersectionPoint;
    if (doIntersect(topLeftPos, topRightPos, _origin, _dest, intersectionPoint) || 
        doIntersect(topRightPos, bottomRightPos, _origin, _dest, intersectionPoint) ||
        doIntersect(bottomRightPos, bottomLeftPos, _origin, _dest, intersectionPoint) ||
        doIntersect(topLeftPos, bottomLeftPos, _origin, _dest, intersectionPoint))
    {
        orxVector_Copy(&result, &intersectionPoint);
        return orxTRUE;
    }

    return orxFALSE;
}
