/**
 * @file Vessel.cpp
 * @date 25-Aug-2025
 */

#include "Vessel.h"
#include "Vacuum.h"
#include "Gauge.h"
#include "Starbase.h"
#include "Shape.h"
#include "MoreMath.h"
#include "Utils.h"

void Vessel::OnCreate()
{
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
    m_StarbaseShieldCollisionFlag = (orxU16)orxPhysics_GetCollisionFlagValue("starbase_shield_phantom");

    m_pstCompass = orxObject_CreateFromConfig("VesselCompass");
    orxObject_SetOwner(m_pstCompass, GetOrxObject());

    m_IsInvincible = orxTRUE;
}

void Vessel::OnDelete()
{
}

void Vessel::Update(const orxCLOCK_INFO &_rstInfo)
{
    if (m_IsInvincible) 
    {
        m_TimeAlive += _rstInfo.fDT;
        if (m_TimeAlive < orxFLOAT(0.2f)) 
        {
            if (orxOBJECT* pstNearest = GetNearestStarBase())
            {
                DrawCompassToObject(pstNearest);
            }
            return;
        }
        m_IsInvincible = orxFALSE;
    }
    
    Gauge* poGaugeDeposit = (Gauge*)olcjam2025::GetInstance().GetObject("GaugeDeposit");
    if (poGaugeDeposit->GetIsMaxedOut())
    {
        if (!m_GameOver)
        {
            orxObject_CreateFromConfig("GameWon");
            m_GameOver = orxTRUE;
        }
    }

    Gauge* poGaugeHealth = (Gauge*)olcjam2025::GetInstance().GetObject("GaugeHealth");
    if (poGaugeHealth->GetIsDepleted()) 
    {
        if (!m_GameOver) 
        {
            orxObject_CreateFromConfig("GameOver");
            m_GameOver = orxTRUE;
        }
        if (m_IsMoving)
        {
            AddTrack("VesselMovingStopTrack");
            m_IsMoving = orxFALSE;
        }
        return;
    }

    if (m_IsDocking == false) 
    {
        if (IsPlayerReturningToBase(_rstInfo) == orxFALSE)
        {
            MovePlayer(_rstInfo);
        }

        if (orxOBJECT* pstNearest = GetNearestStarBase()) 
        {
            DrawCompassToObject(pstNearest);
        }

        if (m_IsDocked)
        {
            if (orxInput_HasBeenActivated("TeleportPrevious"))
            {
                if (orxOBJECT* next = GetPreviousStarBase())
                {
                    orxVECTOR pos;
                    orxObject_GetWorldPosition(next, &pos);
                    SetPosition(pos, orxTRUE);
                }
            }
            else if (orxInput_HasBeenActivated("TeleportNext"))
            {
                if (orxOBJECT* next = GetNextStarBase())
                {
                    orxVECTOR pos;
                    orxObject_GetWorldPosition(next, &pos);
                    SetPosition(pos, orxTRUE);
                }
            }
        }
    }

    // Zoom Out?
    if (orxInput_HasBeenActivated("Zoom"))
    {
        m_IsZooming = orxTRUE;
        orxObject_AddTimeLineTrack(m_Camera, "ZoomOut");
    }
    // Zoom In?
    else if (m_IsZooming && orxInput_HasBeenDeactivated("Zoom"))
    {
        m_IsZooming = orxFALSE;
        orxObject_AddTimeLineTrack(m_Camera, "ZoomIn");
    }

    m_CameraBox.Update(_rstInfo);
}

orxBOOL Vessel::IsPlayerReturningToBase(const orxCLOCK_INFO& _rstInfo)
{
    if (m_IsInsideShield) 
    {
        return orxFALSE;
    }

    Gauge* poGauge = (Gauge*)olcjam2025::GetInstance().GetObject(m_GaugeReturnToBaseGUID);

    if (orxInput_HasBeenActivated("ReturnToStarBase"))
    {
        orxOBJECT* gauge = orxObject_CreateFromConfig("GaugeReturnToBase");
        m_GaugeReturnToBaseGUID = orxStructure_GetGUID(gauge);
        return orxTRUE;
    }
    else if (orxInput_IsActive("ReturnToStarBase"))
    {
        if (poGauge) 
        {
            poGauge->Refill(_rstInfo);

            if (poGauge->GetIsMaxedOut())
            {
                poGauge->SetLifeTime(orxFLOAT_0);
                m_GaugeReturnToBaseGUID = orxU64_UNDEFINED;

                orxOBJECT* sb = GetNearestStarBase();
                if (sb != orxNULL)
                {
                    orxVECTOR pos;
                    orxObject_GetWorldPosition(sb, &pos);
                    SetPosition(pos, orxTRUE);
                }
            }
        }

        return orxTRUE;
    }
    else if (poGauge) 
    {
        poGauge->SetLifeTime(orxFLOAT_0);
        m_GaugeReturnToBaseGUID = orxU64_UNDEFINED;
    }

    return orxFALSE;
}

void Vessel::MovePlayer(const orxCLOCK_INFO& _rstInfo)
{
    Gauge* poGaugeBoost = (Gauge*)olcjam2025::GetInstance().GetObject("GaugeBoost");

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
        poGaugeBoost->SetAutoRefill(orxTRUE);
        m_IsMoving = orxFALSE;
    }
    else 
    {
        orxFLOAT playerBoosting = orxFLOAT_0;
        if (orxInput_GetValue("Fast") && poGaugeBoost->CanDeplete())
        {
            if (poGaugeBoost->Deplete(_rstInfo) > orxFLOAT_0) 
            {
                playerBoosting = orxFLOAT_1;
            }
        }
        else 
        {
            poGaugeBoost->SetAutoRefill(orxTRUE);
        }

        orxVector_Normalize(&PlayerMove, &PlayerMove);

        orxVector_Mulf(&PlayerMove,
            orxVector_Mul(&PlayerMove, &PlayerMove, orxVector_Lerp(&PlayerSpeed, &m_vPlayerSpeed, &m_vPlayerHighSpeed, playerBoosting)),
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

orxBOOL Vessel::GetBeamActive() const 
{
    if (Vacuum* vacuum = (Vacuum*)olcjam2025::GetInstance().GetObject(m_VacuumGUID))
    {
        if (vacuum->GetIsBeamLocked())
        {
            return orxFALSE;
        }
    }
    if (Gauge* shapeGauge = (Gauge*)olcjam2025::GetInstance().GetObject("GaugeShapes"))
    {
        if (shapeGauge->GetIsMaxedOut())
        {
            return orxFALSE;
        }
    }

    return orxInput_IsActive("Vacuum");
}

void Vessel::SetIsInsideStarbaseShield(orxBOOL value)
{
    if (Vacuum* pstVacuum = (Vacuum*)olcjam2025::GetInstance().GetObject(m_VacuumGUID))
    {
        pstVacuum->SetIsBeamLocked(value);
        pstVacuum->Enable(~value, orxTRUE);
    }

    Gauge* poGaugeHealth = (Gauge*)olcjam2025::GetInstance().GetObject("GaugeHealth");
    poGaugeHealth->SetAutoRefill(value);

    m_IsInsideShield = value;
}


void Vessel::SetIsDocked(orxBOOL isDocked)
{
    if (Vacuum* pstVacuum = (Vacuum*)olcjam2025::GetInstance().GetObject(m_VacuumGUID))
    {
        pstVacuum->SetIsVesselDocked(isDocked);
    }

    m_IsDocked = isDocked;
}


void Vessel::GetCameraPosition(orxVECTOR& position) const
{
    m_CameraBox.GetCameraPosition(position);
}

void Vessel::OnCollide(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart, const orxVECTOR& _rvPosition, const orxVECTOR& _rvNormal) 
{
    if (orxBody_GetPartSelfFlags(_pstColliderPart) == m_ShapesCollisionFlag) 
    {
        if (orxString_SearchString(_poCollider->GetName(), "Negative"))
        {
            if (m_IsInvincible) 
            {
                _poCollider->SetLifeTime(orxFLOAT_0);
            }
            else 
            {
                AddTrack("VesselDamageBounceTrack");
                orxBody_RemovePart(_pstColliderPart);
                _poCollider->AddTrack("NegativeShapeCollisionTrack");
                Gauge* poGaugeHealth = (Gauge*)olcjam2025::GetInstance().GetObject("GaugeHealth");
                poGaugeHealth->Decrement(((Shape*)_poCollider)->GetShapeDamageBounce());
            }
        }
        else 
        {
            if (!m_IsInvincible)
            {
                orxVECTOR impulse;
                orxVector_Neg(&impulse, &_rvNormal);
                orxVector_Mulf(&impulse, &impulse, m_ShapesImpulseMultiplier);
                orxBody_ApplyImpulse(orxBody_GetPartBody(_pstColliderPart), &impulse, &_rvPosition);

                AddSound("Sound_BounceFromShip");
            }
        }
    }
    if (orxBody_GetPartSelfFlags(_pstColliderPart) == m_StarbaseShieldCollisionFlag)
    {
        SetIsInsideStarbaseShield(orxTRUE);
    }
}

void Vessel::OnSeparate(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart) 
{
    if (orxBody_GetPartSelfFlags(_pstColliderPart) == m_StarbaseShieldCollisionFlag)
    {
        SetIsInsideStarbaseShield(orxFALSE);
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

orxOBJECT* Vessel::GetPreviousStarBase()
{
    orxSTRINGID id = orxString_GetID("Starbase");

    orxOBJECT* previous = orxNULL;
    orxOBJECT* nearest = GetNearestStarBase();
    orxOBJECT* tmp = orxObject_GetNext(orxNULL, id);
    if (nearest == tmp)
    {
        // nearest is the first, we need to get last in the group
        while (tmp != orxNULL) 
        {
            previous = tmp;
            tmp = orxObject_GetNext(tmp, id);
        }
        return previous;
    }
    else 
    {
        while (tmp != orxNULL && tmp != nearest)
        {
            previous = tmp;
            tmp = orxObject_GetNext(tmp, id);
        }
        return previous;
    }

    return orxNULL;
}

orxOBJECT* Vessel::GetNextStarBase()
{
    orxOBJECT* nearest = GetNearestStarBase();
    orxSTRINGID id = orxString_GetID("Starbase");
    orxOBJECT* next = orxObject_GetNext(nearest, id);
    if (next == orxNULL)
    {
        // getting first
        next = orxObject_GetNext(next, id);
    }

    // nearest can be null
    return next;
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
