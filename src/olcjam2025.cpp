/**
 * @file olcjam2025.cpp
 * @date 24-Aug-2025
 */

#define __SCROLL_IMPL__
#include "olcjam2025.h"
#undef __SCROLL_IMPL__

#include "Object.h"
#include "LoadingGrid.h"
#include "Shape.h"
#include "Vacuum.h"
#include "VacuumBeam.h"
#include "Vessel.h"
#include "orxExtensions.h"

#ifdef __orxMSVC__

/* Requesting high performance dedicated GPU on hybrid laptops */
__declspec(dllexport) unsigned long NvOptimusEnablement        = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#endif // __orxMSVC__

static orxSTRINGID CellStringID;

static orxSTATUS orxFASTCALL EventHandler(const orxEVENT* _pstEvent)
{
    orxSTRUCTURE* pstSender = orxStructure_GetOwner(_pstEvent->hSender);
    if (orxObject_GetGroupID(orxOBJECT(pstSender)) == CellStringID)
    {
        // Sets cell as spawned object's owner
        orxObject_SetOwner(orxOBJECT(_pstEvent->hRecipient), pstSender);
    }

    return orxSTATUS_SUCCESS;
}

/** Update function, it has been registered to be called every tick of the core clock
 */
void olcjam2025::Update(const orxCLOCK_INFO &_rstClockInfo)
{
  // Should quit?
  if(orxInput_HasBeenActivated("Quit"))
  {
    // Send close event
    orxEvent_SendShort(orxEVENT_TYPE_SYSTEM, orxSYSTEM_EVENT_CLOSE);
  }
}

/** Camera Update function, it has been registered to be called every tick of the core clock, after physics & objects have been updated
*/
void olcjam2025::CameraUpdate(const orxCLOCK_INFO &_rstClockInfo)
{
  // Get MainCamera proxy object
  ScrollObject *poMainCamera = olcjam2025::GetInstance().GetObject("MainCamera");
  if(poMainCamera)
  {
    // Update & move the camera here
    // [...]

    // Round its position
    orxVECTOR vPosition;
    poMainCamera->SetPosition(*orxVector_Round(&vPosition, &poMainCamera->GetPosition(vPosition)));
  }
}

/** Init function, it is called when all orx's modules have been initialized
 */
orxSTATUS olcjam2025::Init()
{
  // Init extensions
  InitExtensions();

  // Push [Main] as the default config section
  orxConfig_PushSection("Main");

  // Create the viewports
  for(orxS32 i = 0, iCount = orxConfig_GetListCount("ViewportList"); i < iCount; i++)
  {
    orxViewport_CreateFromConfig(orxConfig_GetListString("ViewportList", i));
  }

  CellStringID = orxString_GetID("Cell");

  // Register event handler to set the cell as owner of spawned objects
  orxEvent_AddHandler(orxEVENT_TYPE_SPAWNER, &EventHandler);
  orxEvent_SetHandlerIDFlags(&EventHandler, orxEVENT_TYPE_SPAWNER, orxNULL, orxEVENT_GET_FLAG(orxSPAWNER_EVENT_SPAWN), orxEVENT_KU32_MASK_ID_ALL);

  // Create the scene
  CreateObject("Startup");

  // Done!
  return orxSTATUS_SUCCESS;
}

/** Run function, it should not contain any game logic
 */
orxSTATUS olcjam2025::Run()
{
  // Return orxSTATUS_FAILURE to instruct orx to quit
  return orxSTATUS_SUCCESS;
}

/** Exit function, it is called before exiting from orx
 */
void olcjam2025::Exit()
{
  // Exit from extensions
  ExitExtensions();

  // Let orx clean all our mess automatically. :)
}

/** BindObjects function, ScrollObject-derived classes are bound to config sections here
 */
void olcjam2025::BindObjects()
{
  BindObject(Object);
  BindObject(LoadingGrid);
  BindObject(Shape);
  BindObject(Vacuum);
  BindObject(VacuumHead);
  BindObject(VacuumBeam);
  BindObject(Vessel);
  BindObject(orxContainerObject);
}

/** Bootstrap function, it is called before config is initialized, allowing for early resource storage definitions
 */
orxSTATUS olcjam2025::Bootstrap() const
{
  // Bootstrap extensions
  BootstrapExtensions();

  // Return orxSTATUS_FAILURE to prevent orx from loading the default config file
  return orxSTATUS_SUCCESS;
}

/** Main function
 */
int main(int argc, char **argv)
{
  // Execute our game
  olcjam2025::GetInstance().Execute(argc, argv);

  // Done!
  return EXIT_SUCCESS;
}
