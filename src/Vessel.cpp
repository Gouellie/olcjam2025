/**
 * @file Vessel.cpp
 * @date 25-Aug-2025
 */

#include "Vessel.h"

void Vessel::OnCreate()
{
    // Init the camera
    m_Camera = orxObject_CreateFromConfig("Camera");
    orxObject_GetPosition(m_Camera, &m_PreviousCameraPos);

    // Attach Camera to Vessel
    orxObject_SetParent(m_Camera, GetOrxObject());
}

void Vessel::OnDelete()
{
    orxObject_SetLifeTime(m_Camera, orxFLOAT_0);
}

void Vessel::Update(const orxCLOCK_INFO &_rstInfo)
{
    PushConfigSection();

    // Temporary solution, will eventually be allowed to move and aim the vacuum at the same time
    if (!orxInput_IsActive("Vacuum"))
    {
        // Update player (and camera) position
        orxVECTOR PlayerMove, PlayerSpeed, PlayerHighSpeed, PlayerPos;
        orxVector_Mulf(&PlayerMove,
            orxVector_Mul(&PlayerMove,
                orxVector_Set(&PlayerMove,
                    orxInput_GetValue("Right") - orxInput_GetValue("Left"),
                    orxInput_GetValue("Down") - orxInput_GetValue("Up"),
                    orxFLOAT_0),
                orxVector_Lerp(&PlayerSpeed,
                    orxConfig_GetListVector("MovingSpeed", 0, &PlayerSpeed),
                    orxConfig_GetListVector("MovingSpeed", 1, &PlayerHighSpeed),
                    orxInput_GetValue("Fast"))),
            _rstInfo.fDT);
        orxObject_SetPosition(GetOrxObject(), orxVector_Add(&PlayerPos,
            orxObject_GetPosition(GetOrxObject(), &PlayerPos),
            orxVector_Round(&PlayerMove, &PlayerMove)));
    }

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

    if (orxInput_HasBeenActivated("Vacuum"))
    {
        m_Vacuum = orxObject_CreateFromConfig("Vacuum");
        orxObject_SetOwner(m_Vacuum, GetOrxObject());
        orxObject_SetParent(m_Vacuum, GetOrxObject());
    }
    else if (orxInput_HasBeenDeactivated("Vacuum"))
    {
        if (orxOBJECT(m_Vacuum) != orxNULL)
        {
            orxObject_SetLifeTime(m_Vacuum, orxFLOAT_0);
        }
    }

    PopConfigSection();
}
