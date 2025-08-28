//! Includes

#ifndef _orxCONTAINER_H_
#define _orxCONTAINER_H_

#include "orx.h"
#include "orxContainerBank.h"
#include "orxContainerObject.h"

//! Prototypes

void                                        orxContainer_Init();
void                                        orxContainer_Exit();

/** Misc defines
 */
#define orxCONTAINER_KU32_TEXT_BUFFER_SIZE              1024
#define orxCONTAINER_KST_DEFAULT_COLOR                  orx2RGBA(255, 0, 0, 255)

/** Config defines
 */
#define orxCONTAINER_KZ_CONFIG_SECTION                  "Containers"

#define orxCONTAINER_KZ_CONFIG_SHOW_DEBUG               "ShowDebug"
#define orxCONTAINER_KZ_CONFIG_CONTAINER_NAME           "Container"

/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

#ifdef orxCONTAINER_IMPL

static void orxContainer_DrawContainerName(orxOBJECT* _pstObject, orxVIEWPORT* _pstViewport, orxVECTOR _vOrigin)
{
  const orxFONT* pstFont;

  /* Gets default font */
  pstFont = orxFont_Get(orxFONT_KZ_DEFAULT_FONT_NAME);

  /* Valid? */
  if (pstFont != orxNULL)
  {
    orxBITMAP* pstBitmap;
    orxDISPLAY_TRANSFORM  stTextTransform;
    orxCHAR               acBuffer[32];

    /* Gets its bitmap */
    pstBitmap = orxTexture_GetBitmap(orxFont_GetTexture(pstFont));

    /* Clears text transform */
    orxMemory_Zero(&stTextTransform, sizeof(orxDISPLAY_TRANSFORM));

    /* Valid? */
    if (_pstViewport != orxNULL)
    {
      //orxAABOX  stBox;
      orxFLOAT  fWidth, fHeight;

      /* Gets its box & size */
      orxViewport_GetRelativeSize(_pstViewport, &fWidth, &fHeight);

      /* Inits transform's scale */
      stTextTransform.fScaleX = (orx2F(1.0f) * fWidth);
      stTextTransform.fScaleY = (orx2F(1.0f) * fHeight);

      orxVECTOR offSet = { orx2F(5.0f), orx2F(5.0f) };
      orxVector_2DRotate(&offSet, &offSet, orxObject_GetRotation(_pstObject));

      /* Inits transform's destination */
      stTextTransform.fDstX = _vOrigin.fX + offSet.fX;
      stTextTransform.fDstY = _vOrigin.fY + offSet.fY;

      stTextTransform.fRotation = orxObject_GetRotation(_pstObject);

      /* Writes string */
      orxString_NPrint(acBuffer, sizeof(acBuffer), orxObject_GetName(_pstObject));

      /* Displays it */
      orxDisplay_TransformText(acBuffer, pstBitmap, orxFont_GetMap(pstFont), &stTextTransform, orxCONTAINER_KST_DEFAULT_COLOR, orxDISPLAY_SMOOTHING_OFF, orxDISPLAY_BLEND_MODE_ALPHA);
    }
  }

  /* Done! */
  return;
}

static void orxContainer_DrawPointForViewPort(const orxVECTOR& _vPosition, orxVIEWPORT* _pstViewPort)
{
  orxVECTOR vScreenPos;
  orxRender_GetScreenPosition(&_vPosition, _pstViewPort, &vScreenPos);
  orxDisplay_DrawCircle(&vScreenPos, orxFLOAT(5.0f), orxCONTAINER_KST_DEFAULT_COLOR, orxTRUE);
}

static void orxContainer_DrawPoint(const orxVECTOR& vPosition)
{
  orxVIEWPORT* pstViewport;

  /* For all viewports */
  for (pstViewport = orxVIEWPORT(orxStructure_GetFirst(orxSTRUCTURE_ID_VIEWPORT));
    pstViewport != orxNULL;
    pstViewport = orxVIEWPORT(orxStructure_GetNext(pstViewport)))
  {
    /* Is enabled and supports debug? */
    if ((orxViewport_IsEnabled(pstViewport) != orxFALSE)
      && (!orxStructure_GetFlags(pstViewport, orxVIEWPORT_KU32_FLAG_NO_DEBUG)))
    {
      orxCAMERA* pstCamera;

      /* Gets viewport camera */
      pstCamera = orxViewport_GetCamera(pstViewport);

      /* Valid? */
      if (pstCamera != orxNULL)
      {
        orxContainer_DrawPointForViewPort(vPosition, pstViewport);
      }
    }
  }
}

static void orxContainer_DrawBoundingBox(orxOBJECT* _pstObject)
{
  orxOBOX stBoundingBox;
  orxObject_GetBoundingBox(_pstObject, &stBoundingBox);

  orxVECTOR vOrigin;
  /* Gets origin */
  orxVector_Sub(&vOrigin, &(stBoundingBox.vPosition), &(stBoundingBox.vPivot));

  orxVIEWPORT* pstViewport;

  /* For all viewports */
  for (pstViewport = orxVIEWPORT(orxStructure_GetFirst(orxSTRUCTURE_ID_VIEWPORT));
    pstViewport != orxNULL;
    pstViewport = orxVIEWPORT(orxStructure_GetNext(pstViewport)))
  {
    /* Is enabled and supports debug? */
    if ((orxViewport_IsEnabled(pstViewport) != orxFALSE)
      && (!orxStructure_GetFlags(pstViewport, orxVIEWPORT_KU32_FLAG_NO_DEBUG)))
    {
      orxCAMERA* pstCamera;

      /* Gets viewport camera */
      pstCamera = orxViewport_GetCamera(pstViewport);

      /* Valid? */
      if (pstCamera != orxNULL)
      {
        orxAABOX    stFrustum;
        orxVECTOR   vCameraPosition;
        orxFLOAT    fZ;
        orxS32      i;

        /* Gets camera position */
        orxFrame_GetPosition(orxCamera_GetFrame(pstCamera), orxFRAME_SPACE_GLOBAL, &vCameraPosition);

        /* Gets its frustum */
        orxCamera_GetFrustum(pstCamera, &stFrustum);

        /* Stores its Z */
        fZ = stFrustum.vTL.fZ + vCameraPosition.fZ;

        orxVECTOR vTopLeft, vTopRight, vBottomRight, vBottomLeft;

        /* Sets it */
        orxVector_Set(&vTopLeft, (vOrigin.fX), (vOrigin.fY), fZ);
        orxVector_Set(&vTopRight, (vOrigin.fX + stBoundingBox.vX.fX), (vOrigin.fY + stBoundingBox.vX.fY), fZ);
        orxVector_Set(&vBottomRight, (vOrigin.fX + stBoundingBox.vX.fX + stBoundingBox.vY.fX), (vOrigin.fY + stBoundingBox.vX.fY + stBoundingBox.vY.fY), fZ);
        orxVector_Set(&vBottomLeft, (vOrigin.fX + stBoundingBox.vY.fX), (vOrigin.fY + stBoundingBox.vY.fY), fZ);

        const int vertexListSize = 4;
        orxVECTOR avVertexList[] = { vTopLeft, vTopRight, vBottomRight, vBottomLeft };

        for (i = 0; i < vertexListSize; i++) {
          /* Stores its screen position */
          orxRender_GetScreenPosition(&avVertexList[i], pstViewport, &(avVertexList[i]));
        }

        /* Draws polygon */
        orxDisplay_DrawPolygon(avVertexList, (orxS32)4, orxCONTAINER_KST_DEFAULT_COLOR, orxFALSE);

        /* Draws name (top left) */
        orxContainer_DrawContainerName(_pstObject, pstViewport, avVertexList[0]);
      }
    }
  }
}

static void orxContainer_Update(const orxCLOCK_INFO* _pstClockInfo, void* _pContext)
{
  // Done!
  return;
}

static int orxContainer_GetObjectDepth(const orxOBJECT* _pstObject)
{
  orxU8 uDepth = 0;
  orxSTRUCTURE* pstOwner;

  for (pstOwner = orxObject_GetOwner(_pstObject);
    pstOwner != orxNULL;
    pstOwner = orxStructure_GetOwner(pstOwner))
  {
    uDepth++;
  }

  return uDepth;
}

static int orxContainer_CompareObjectDepth(const void* _psta, const void* _pstb)
{
  orxU8 uDepth_a, uDepth_b;

  const orxOBJECT* psta = *(const orxOBJECT**)_psta;
  const orxOBJECT* pstb = *(const orxOBJECT**)_pstb;

  if (psta == orxNULL || pstb == orxNULL)
    return 0;

  uDepth_a = orxContainer_GetObjectDepth(psta);
  uDepth_b = orxContainer_GetObjectDepth(pstb);

  if (uDepth_a < uDepth_b)
    return -1;
  if (uDepth_a > uDepth_b)
    return 1;

  return 0;
}

static orxSTATUS orxFASTCALL orxContainer_EventHandler(const orxEVENT *_pstEvent)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(_pstEvent->eType == orxEVENT_TYPE_RENDER);

  if (sstObject.pstContainerBank == orxNULL) 
  {
      eResult = orxSTATUS_FAILURE;
      return eResult;
  }

  /* End of rendering? */
  if(_pstEvent->eID == orxRENDER_EVENT_STOP)
  {
    orxS32                i, listSize = 0;
    orxOBJECT*            listSorted[orxCONTAINER_KU32_BANK_SIZE]{};
    orxU64*               pstGUID;
    orxContainerObject*   poContainerObject;

    /* For all containers objects */
    for (pstGUID = (orxU64*)orxBank_GetNext(sstObject.pstContainerBank, orxNULL);
        pstGUID != orxNULL;
        pstGUID = (orxU64*)orxBank_GetNext(sstObject.pstContainerBank, pstGUID))
    {
      poContainerObject = (orxContainerObject*)ScrollBase::GetInstance().GetObject(*pstGUID);

      /* Still valid? */
      if (poContainerObject != orxNULL)
      {
          if (poContainerObject->IsEnabled()) 
          {
              listSorted[listSize] = poContainerObject->GetOrxObject();
              listSize++;
          }
      }
    }

    if (listSize > 1)
    {
      /* Sorted by depth */
      qsort(listSorted, listSize, sizeof(orxOBJECT*), orxContainer_CompareObjectDepth);
    }

    for (i = 0; i < listSize; i++)
    {
      /* Get ScrollObject */
      orxContainerObject* poContainerObject = (orxContainerObject*)orxObject_GetUserData(listSorted[i]);

      poContainerObject->SortChildren();

      /* Pushes config section */
      orxConfig_PushSection(orxCONTAINER_KZ_CONFIG_SECTION);
      /* Show debug? */
      if (orxConfig_GetBool(orxCONTAINER_KZ_CONFIG_SHOW_DEBUG))
      {
        orxContainer_DrawBoundingBox(listSorted[i]);
      }
      /* Pops config section */
      orxConfig_PopSection();
    }
  }

  /* Done! */
  return eResult;
}

void orxContainer_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  // Register clock callback to render inspector windows on each frame
  eResult = orxClock_Register(orxClock_Get(orxCLOCK_KZ_CORE), orxContainer_Update, orxNULL, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_NORMAL);

  /* Success? */
  if (eResult != orxSTATUS_FAILURE)
  {
    /* Creates banks */
    sstObject.pstContainerBank = orxBank_Create(orxCONTAINER_KU32_BANK_SIZE, sizeof(orxU64), orxBANK_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);
  }

  /* Adds event handler */
  orxEvent_AddHandler(orxEVENT_TYPE_RENDER, orxContainer_EventHandler);
  orxEvent_SetHandlerIDFlags(orxContainer_EventHandler, orxEVENT_TYPE_RENDER, orxNULL, orxEVENT_GET_FLAG(orxRENDER_EVENT_STOP), orxEVENT_KU32_MASK_ID_ALL);

  // Done!
  return;
}

void orxContainer_Exit()
{
  // Unregister clock callback
  orxClock_Unregister(orxClock_Get(orxCLOCK_KZ_CORE), orxContainer_Update);

  /* Deletes banks */
   orxBank_Delete(sstObject.pstContainerBank);
   sstObject.pstContainerBank = orxNULL;

  // Done!
  return;
}

#endif // orxCONTAINER_IMPL

#endif // _orxCONTAINER_H_
