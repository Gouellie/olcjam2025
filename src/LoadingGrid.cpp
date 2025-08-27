/**
 * @file LoadingGrid.cpp
 * @date 24-Aug-2025
 */

#include "LoadingGrid.h"

void LoadingGrid::OnCreate()
{
    PushConfigSection(orxTRUE);

    // The String is evaluated and the command is executed. 
    // This set's an arbitrary offset that the spawned Objects uses
    orxConfig_GetString("SetBasePosition");

    m_CellTable = orxHashTable_Create(8192, orxHASHTABLE_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);
    m_CellSize = orxConfig_GetFloat("CellSize");
    m_LoadingRange = orxConfig_GetS32("LoadingRange");
    m_CellSpawner = orxConfig_GetString("CellSpawner");

    PopConfigSection();
}

void LoadingGrid::OnDelete()
{
    orxHashTable_Delete(m_CellTable);
}

void LoadingGrid::Update(const orxCLOCK_INFO &_rstInfo)
{
    orxConfig_PushSection("Runtime");
    orxU64 vesselGUID = orxConfig_GetU64("Vessel");
    orxConfig_PopSection();

    orxConfig_PushSection("Cell");
    orxConfig_SetString("Spawner", m_CellSpawner);
    orxConfig_PopSection();

    orxVECTOR PlayerPos;

    orxOBJECT* pstVessel;
    if (pstVessel = orxOBJECT(orxStructure_Get(vesselGUID)))
    {
        orxObject_GetPosition(pstVessel, &PlayerPos);
    }
    else 
    {
        PlayerPos = orxVECTOR_0;
    }

    // Get cell positions
    orxVECTOR CellPos, PreviousCellPos;

    orxVector_Round(&CellPos, orxVector_Divf(&CellPos, &PlayerPos, m_CellSize));
    orxVector_Round(&PreviousCellPos, orxVector_Divf(&PreviousCellPos, &m_PreviousPlayerPos, m_CellSize));

    // For all neighboring cells
    for (orxS32 i = -m_LoadingRange; i <= m_LoadingRange; i++)
    {
        for (orxS32 j = -m_LoadingRange; j <= m_LoadingRange; j++)
        {
            // Create/Enable new neighbor cells
            orxS32 x = orxF2S(CellPos.fX) + i, y = orxF2S(CellPos.fY) + j;
            orxU64 CellID = ((orxU64)x << 32) | (orxU32)y;
            orxOBJECT* Cell = (orxOBJECT*)orxHashTable_Get(m_CellTable, CellID);
            if (!Cell)
            {
                // Create new node
                orxVECTOR Pos;
                Cell = orxObject_CreateFromConfig("Cell");

                orxObject_SetParent(Cell, GetOrxObject());
                orxObject_SetOwner(Cell, GetOrxObject());
                orxObject_SetPosition(Cell, orxVector_Set(&Pos, m_CellSize * orxS2F(x), m_CellSize * orxS2F(y), orxFLOAT_0));

                orxHashTable_Add(m_CellTable, CellID, (void*)Cell);
            }
            else
            {
                // Wasn't node enabled?
                if (!orxObject_IsEnabled(Cell))
                {
                    // Re-activate it
                    orxObject_EnableRecursive(Cell, orxTRUE);
                }
            }

            // Disable out-of-range cells
            x = orxF2S(PreviousCellPos.fX) + i, y = orxF2S(PreviousCellPos.fY) + j;
            CellID = ((orxU64)x << 32) | (orxU32)y;
            if ((x < orxF2S(CellPos.fX) - m_LoadingRange) || (x > orxF2S(CellPos.fX) + m_LoadingRange) || (y < orxF2S(CellPos.fY) - m_LoadingRange) || (y > orxF2S(CellPos.fY) + m_LoadingRange))
            {
                Cell = (orxOBJECT*)orxHashTable_Get(m_CellTable, CellID);
                orxASSERT(Cell);
                orxASSERT(orxObject_IsEnabled(Cell));

                // De-activate it
                orxObject_EnableRecursive(Cell, orxFALSE);
            }
        }
    }

    // Update previous player position
    orxVector_Copy(&m_PreviousPlayerPos, &PlayerPos);
}