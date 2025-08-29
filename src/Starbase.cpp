/**
 * @file Starbase.cpp
 * @date 28-Aug-2025
 */

#include "Starbase.h"
#include "Vessel.h"

void Starbase::OnCreate()
{
    m_DockingSpeed = orxConfig_GetFloat("DockingSpeed");
}

void Starbase::OnDelete()
{
}

void Starbase::Update(const orxCLOCK_INFO &_rstInfo)
{
    if (m_bIsDocking) 
    {
        if (Vessel* pstVessel = (Vessel*)olcjam2025::GetInstance().GetObject(olcjam2025::GetInstance().GetActiveVesselID()))
        {
            orxVECTOR destination, vesselPosition;
            GetPosition(destination, orxTRUE);

            pstVessel->GetPosition(vesselPosition, orxTRUE);
            orxVector_Lerp(&vesselPosition, &vesselPosition, &destination, m_DockingSpeed * _rstInfo.fDT);
            pstVessel->SetPosition(vesselPosition, orxTRUE);

            if (orxVector_GetDistance(&vesselPosition, &destination) < 0.1f) 
            {
                pstVessel->SetIsDocking(orxFALSE);
                pstVessel->SetIsDocked(orxTRUE);
                m_bIsDocking = orxFALSE;
            }
        }
    }

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
}

void Starbase::OnCollide(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart, const orxVECTOR& _rvPosition, const orxVECTOR& _rvNormal)
{
    if (Vessel* pstVessel = (Vessel*)_poCollider) 
    {
        pstVessel->SetIsDocking(orxTRUE);
        m_bIsDocking = orxTRUE;
    }
}

void Starbase::OnSeparate(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart)
{
    if (Vessel* pstVessel = (Vessel*)_poCollider)
    {
        pstVessel->SetIsDocked(orxFALSE);
    }
}