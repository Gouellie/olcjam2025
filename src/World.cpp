/**
 * @file World.cpp
 * @date 24-Aug-2025
 */

#include "World.h"

static orxSTATUS orxFASTCALL EventHandler(const orxEVENT* _pstEvent)
{
    // Sets cell as spawned object's owner
    orxObject_SetOwner(orxOBJECT(_pstEvent->hRecipient), orxStructure_GetOwner(_pstEvent->hSender));

    return orxSTATUS_SUCCESS;
}

void World::ApplySettings()
{
    orxConfig_SetParent("World", orxConfig_GetListString("Settings", m_Settings));
    orxConfig_GetString("UpdateSettings");
    orxHashTable_Clear(m_WorldTable);
    orxOBJECT* universe = orxOBJECT(orxStructure_Get(m_Universe));
    if (universe) {
        orxObject_SetLifeTime(universe, orxFLOAT_0);
    }
    m_Universe = orxStructure_GetGUID(orxObject_CreateFromConfig("Universe"));
}

void World::OnCreate()
{
    // Init our world
    m_WorldTable = orxHashTable_Create(8192, orxHASHTABLE_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);

    // Init the camera
    m_Camera = orxObject_CreateFromConfig("Camera");
    orxObject_GetPosition(m_Camera, &m_PreviousCameraPos);

    // Create universe
    m_Universe = orxStructure_GetGUID(orxObject_CreateFromConfig("Universe"));

    // Init settings
    m_Settings = orxConfig_GetListCount("Settings") / 2;
    ApplySettings();

    // Register event handler to set the cell as owner of spawned objects
    orxEvent_AddHandler(orxEVENT_TYPE_SPAWNER, &EventHandler);
    orxEvent_SetHandlerIDFlags(&EventHandler, orxEVENT_TYPE_SPAWNER, orxNULL, orxEVENT_GET_FLAG(orxSPAWNER_EVENT_SPAWN), orxEVENT_KU32_MASK_ID_ALL);
}

void World::OnDelete()
{
    orxHashTable_Delete(m_WorldTable);
    orxObject_SetLifeTime(m_Camera, orxFLOAT_0);
    orxObject_SetLifeTime(orxOBJECT(orxStructure_Get(m_Universe)), orxFLOAT_0);
}

void World::Update(const orxCLOCK_INFO &_rstInfo)
{
    orxConfig_PushSection("World");

    // Update camera
    orxVECTOR CameraMove, CameraSpeed, CameraHighSpeed, CameraPos;
    orxVector_Mulf(&CameraMove,
        orxVector_Mul(&CameraMove,
            orxVector_Set(&CameraMove,
                orxInput_GetValue("Right") - orxInput_GetValue("Left"),
                orxInput_GetValue("Down") - orxInput_GetValue("Up"),
                orxFLOAT_0),
            orxVector_Lerp(&CameraSpeed,
                orxConfig_GetListVector("CameraSpeed", 0, &CameraSpeed),
                orxConfig_GetListVector("CameraSpeed", 1, &CameraHighSpeed),
                orxInput_GetValue("Fast"))),
        _rstInfo.fDT);
    orxObject_SetPosition(m_Camera, orxVector_Add(&CameraPos,
        orxObject_GetPosition(m_Camera, &CameraPos),
        orxVector_Round(&CameraMove, &CameraMove)));

    // Change settings
    if (orxInput_HasBeenActivated("CycleUp") || orxInput_HasBeenActivated("CycleDown"))
    {
        orxS32 NewSettings = orxInput_IsActive("CycleUp") ? orxMIN(m_Settings + 1, orxConfig_GetListCount("Settings") - 1) : orxMAX(m_Settings - 1, 0);
        if (NewSettings != m_Settings)
        {
            m_Settings = NewSettings;
            ApplySettings();
            orxObject_CreateFromConfig("ClearSnap");
            orxVector_Copy(&m_PreviousCameraPos, &CameraPos);
        }
    }

    // Get cell positions
    orxVECTOR CellPos, PreviousCellPos;
    orxFLOAT  CellSize = orxConfig_GetFloat("CellSize");
    orxVector_Round(&CellPos, orxVector_Divf(&CellPos, &CameraPos, CellSize));
    orxVector_Round(&PreviousCellPos, orxVector_Divf(&PreviousCellPos, &m_PreviousCameraPos, CellSize));

    // For all neighboring cells
    for (orxS32 i = -1; i <= 1; i++)
    {
        for (orxS32 j = -1; j <= 1; j++)
        {
            // Create/Enable new neighbor cells
            orxS32 x = orxF2S(CellPos.fX) + i, y = orxF2S(CellPos.fY) + j;
            orxU64 CellID = ((orxU64)x << 32) | (orxU32)y;
            orxOBJECT* Cell = (orxOBJECT*)orxHashTable_Get(m_WorldTable, CellID);
            if (!Cell)
            {
                // Create new node
                orxVECTOR Pos;
                Cell = orxObject_CreateFromConfig("Cell");
                orxObject_SetPosition(Cell, orxVector_Set(&Pos, CellSize * orxS2F(x), CellSize * orxS2F(y), orxFLOAT_0));
                orxHashTable_Add(m_WorldTable, CellID, (void*)Cell);
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
            if ((x < orxF2S(CellPos.fX) - 1) || (x > orxF2S(CellPos.fX) + 1) || (y < orxF2S(CellPos.fY) - 1) || (y > orxF2S(CellPos.fY) + 1))
            {
                Cell = (orxOBJECT*)orxHashTable_Get(m_WorldTable, CellID);
                orxASSERT(Cell);
                orxASSERT(orxObject_IsEnabled(Cell));

                // De-activate it
                orxObject_EnableRecursive(Cell, orxFALSE);
            }
        }
    }

    // Update previous camera position
    orxVector_Copy(&m_PreviousCameraPos, &CameraPos);

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

    // Snapshot?
    if (orxInput_HasBeenActivated("Snap"))
    {
        // Create its viewport
        orxViewport_CreateFromConfig("SnapViewport");

        // Store position
        orxConfig_PushSection("Runtime");
        orxConfig_SetVector("SnapPos", &m_PreviousCameraPos);
        orxConfig_PopSection();

        // Create snapshot
        orxObject_CreateFromConfig("Snapshot");
    }
    else
    {
        // Delete its viewport
        if (orxViewport_Get("SnapViewport"))
        {
            orxViewport_Delete(orxViewport_Get("SnapViewport"));
        }

        // Recall?
        if (orxInput_HasBeenActivated("Recall"))
        {
            orxVECTOR Pos;

            // Restore position
            orxConfig_PushSection("Runtime");
            if (orxConfig_GetVector("SnapPos", &Pos))
            {
                orxObject_AddUniqueFX(m_Camera, "Recall");
            }
            orxConfig_PopSection();
        }
    }

    orxConfig_PopSection(); // World
}
