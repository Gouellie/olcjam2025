/**
 * @file Vessel.cpp
 * @date 25-Aug-2025
 */

#include "Vessel.h"
#include "CameraBox.h"

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
            m_CameraBox.SetVacuumID(orxStructure_GetGUID(pstChild));
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
    // Update player (and camera) position
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

    // Zoom Out?
    if (orxInput_HasBeenActivated("Zoom"))
    {
        orxObject_AddTimeLineTrack(m_Camera, "ZoomOut");
    }
    // Zoom In?
    else if (orxInput_HasBeenDeactivated("Zoom"))
    {
        orxObject_AddTimeLineTrack(m_Camera, "ZoomIn");
    }

    //if (orxInput_HasBeenActivated("Vacuum"))
    //{
    //    orxObject_AddFX(m_Camera, "MiniShake");
    //}
    //else if (orxInput_HasBeenDeactivated("Vacuum"))
    //{
    //    orxObject_RemoveFX(m_Camera, "MiniShake");
    //}

    if (orxInput_HasBeenActivated("Scan")) 
    {
        //orxConfig_PushSection("Runtime");
        //orxU64 shortGridID = orxConfig_GetU64("ShortRangeGrid");
        //orxConfig_PopSection();

        //LoadingGrid* pstShortRangeGrid = (LoadingGrid*)olcjam2025::GetInstance().GetObject(shortGridID);

        //if (pstShortRangeGrid != orxNULL) 
        //{
        //    orxOBJECT* cell = pstShortRangeGrid->GetCellAtCoordinates(0, 2);
        //    orxFLOAT cellSize = pstShortRangeGrid->GetCellSize();
        //    orxOBJECT* logo = orxObject_CreateFromConfig("Logo");
        //    orxObject_SetParent(logo, cell);
        //    orxObject_SetOwner(logo, cell);
        //    orxVECTOR position = { cellSize/2.f , cellSize/2.f };
        //    orxObject_SetPosition(logo, &position);
        //}
    }

    m_CameraBox.Update(_rstInfo);
}
