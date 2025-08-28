#ifndef _ORXCONTAINERBANK_H_
#define _ORXCONTAINERBANK_H_

#include "orx.h"

//! Prototypes

orxSTATUS                                   orxContainerBank_RegisterContainer(orxOBJECT* _pstObject);
orxSTATUS                                   orxContainerBank_UnregisterContainer(orxOBJECT* _pstObject);

/** Misc defines
 */
#define orxCONTAINER_KU32_BANK_SIZE                     64

  /** Static structure
   */
typedef struct __orxCONTAINER_STATIC_t
{
  orxBANK* pstContainerBank;               /**< Container bank */

} orxCONTAINER_STATIC
;
/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

 /** Static data
  */
static orxCONTAINER_STATIC sstObject;

#ifdef orxCONTAINER_IMPL

orxSTATUS orxContainerBank_RegisterContainer(orxOBJECT* _pstObject)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  // Valid?
  if (_pstObject != orxNULL)
  {
    orxU64* pstGUID;
    /* Adds it to the bank */
    pstGUID = (orxU64*)orxBank_Allocate(sstObject.pstContainerBank);
    orxASSERT(pstGUID != orxNULL);
    *pstGUID = orxStructure_GetGUID(_pstObject);;

    // Update status
    eResult = orxSTATUS_SUCCESS;
  }

  // Done!
  return eResult;
}

orxSTATUS orxContainerBank_UnregisterContainer(orxOBJECT* _pstObject)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;
  if (_pstObject != orxNULL) 
  {
      orxU64 objectID = orxStructure_GetGUID(_pstObject);
      orxU64* pstGUID;
      /* For all containers objects */
      for (pstGUID = (orxU64*)orxBank_GetNext(sstObject.pstContainerBank, orxNULL);
          pstGUID != orxNULL;
          pstGUID = (orxU64*)orxBank_GetNext(sstObject.pstContainerBank, pstGUID))
      {
          if (*pstGUID == objectID) 
          {
              orxBank_Free(sstObject.pstContainerBank, pstGUID);
              eResult = orxSTATUS_SUCCESS;
              break;
          }
      }
  }

  // Done!
  return eResult;
}

#endif // orxCONTAINER_IMPL

#endif // _ORXCONTAINERBANK_H_
