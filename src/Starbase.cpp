/**
 * @file Starbase.cpp
 * @date 28-Aug-2025
 */

#include "Starbase.h"
#include "Vessel.h"
#include "Utils.h"

void Starbase::OnCreate()
{
    m_DockingSpeed = orxConfig_GetFloat("DockingSpeed");
}

void Starbase::OnDelete()
{
}

void Starbase::Update(const orxCLOCK_INFO &_rstInfo)
{
    Vessel* pstVessel = (Vessel*)olcjam2025::GetInstance().GetObject("Vessel");
    if (pstVessel == orxNULL)
        return;

    if (m_bIsDocking) 
    {
        orxVECTOR destination, vesselPosition;
        GetPosition(destination, orxTRUE);

        pstVessel->GetPosition(vesselPosition, orxTRUE);
        orxVector_Lerp(&vesselPosition, &vesselPosition, &destination, m_DockingSpeed * _rstInfo.fDT);
        pstVessel->SetPosition(vesselPosition, orxTRUE);

        if (orxVector_GetDistance(&vesselPosition, &destination) < 1.0f) 
        {
            AddTrack("StarbaseShipDockedTrack");
            pstVessel->SetIsDocking(orxFALSE);
            pstVessel->SetIsDocked(orxTRUE);
            m_bIsDocking = orxFALSE;
            m_bIsDocked = orxTRUE;

            if (orxOBJECT* pstRadialMenu = orxOBJECT(orxStructure_Get(m_RadialMenuGUID)))
            {
                orxObject_SetLifeTime(pstRadialMenu, 0);
                m_RadialMenuGUID = orxU64_UNDEFINED;
            }

            m_DockedPosition = vesselPosition;
        }
    }
    else if (m_bIsDocked)
    {
        orxVECTOR pos;
        pstVessel->GetPosition(pos);
        if (orxVector_AreEqual(&m_DockedPosition, &pos))
        {
            AdjustRadialMenuToOffset();
        }
        else 
        {
            pstVessel->SetIsDocked(orxFALSE);
            AddTrack("StarbaseShipUndockedTrack");
            if (orxOBJECT* pstRadialMenu = orxOBJECT(orxStructure_Get(m_RadialMenuGUID)))
            {
                orxObject_SetLifeTime(pstRadialMenu, 0);
            }

            m_bIsDocked = orxFALSE;
            m_RadialMenuGUID = orxU64_UNDEFINED;
        }
    }
}

void Starbase::OnCollide(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart, const orxVECTOR& _rvPosition, const orxVECTOR& _rvNormal)
{
    if (Vessel* pstVessel = (Vessel*)_poCollider)
    {
        pstVessel->SetIsDocking(orxTRUE);
        m_bIsDocking = orxTRUE;
        AddTrack("StarbaseShipDockingTrack");
    }
}

void Starbase::AdjustRadialMenuToOffset()
{
    orxVECTOR startBPos, res;
    GetPosition(startBPos);

    if (ConvertWorldPositionForViewport(orxViewport_Get("MainViewport"), orxViewport_Get("HUDViewport"), startBPos, res)) 
    {
        if (m_RadialMenuGUID == orxU64_UNDEFINED)
        {
            orxOBJECT* radialMenu = orxObject_CreateFromConfig("RadialMenuStarBase");
            orxObject_SetOwner(radialMenu, GetOrxObject());
            m_RadialMenuGUID = orxStructure_GetGUID(radialMenu);
            orxObject_SetPosition(radialMenu, &res);
        }
        else if (orxOBJECT* pstRadialMenu = orxOBJECT(orxStructure_Get(m_RadialMenuGUID)))
        {
            orxObject_SetPosition(pstRadialMenu, &res);
        }
    }
}