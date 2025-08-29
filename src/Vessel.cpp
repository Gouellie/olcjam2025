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
}

void Vessel::OnDelete()
{
}

void Vessel::Update(const orxCLOCK_INFO &_rstInfo)
{
    if (m_IsDocking == false) 
    {
        MovePlayer(_rstInfo);

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

    orxVector_Mulf(&PlayerMove,
        orxVector_Mul(&PlayerMove,
            orxVector_Set(&PlayerMove,
                orxInput_GetValue("Right") - orxInput_GetValue("Left"),
                orxInput_GetValue("Down") - orxInput_GetValue("Up"),
                orxFLOAT_0),
            orxVector_Lerp(&PlayerSpeed, &m_vPlayerSpeed, &m_vPlayerHighSpeed, orxInput_GetValue("Fast"))),
        _rstInfo.fDT);

    orxObject_SetPosition(GetOrxObject(), orxVector_Add(&PlayerPos,
        orxObject_GetPosition(GetOrxObject(), &PlayerPos),
        orxVector_Round(&PlayerMove, &PlayerMove)));
}

void Vessel::SetIsDocking(orxBOOL isDocking)
{
    m_IsDocking = isDocking;
    if (m_IsDocking) 
    {
        if (Vacuum* pstVacuum = (Vacuum*)olcjam2025::GetInstance().GetObject(m_VacuumGUID))
        {
            pstVacuum->SetIsBeamLocked(orxTRUE);
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
        m_CameraBox.SetIsVacuumLocked(m_IsDocked);
    }
}