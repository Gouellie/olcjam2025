/**
 * @file Starbase.cpp
 * @date 28-Aug-2025
 */

#include "Starbase.h"

void Starbase::OnCreate()
{
}

void Starbase::OnDelete()
{
}

void Starbase::Update(const orxCLOCK_INFO &_rstInfo)
{
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
