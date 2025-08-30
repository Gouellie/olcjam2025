#pragma once

#include "orx.h"

static orxBOOL GetIsObjectInView(const orxVIEWPORT* _pstViewport, const orxVECTOR& _worldPosition)
{
    orxVECTOR dummy;
    orxRender_GetScreenPosition(&_worldPosition, _pstViewport, &dummy);
    return orxRender_GetWorldPosition(&dummy, _pstViewport, &dummy) != orxNULL;
}

static orxBOOL ConvertWorldPositionForViewport(const orxVIEWPORT* _pstFrom, const orxVIEWPORT* _pstTo, orxVECTOR& pos, orxVECTOR& res)
{
    orxVECTOR dummy;
    orxRender_GetScreenPosition(&pos, _pstFrom, &dummy);
    if (orxRender_GetWorldPosition(&dummy, _pstTo, &dummy) != orxNULL) 
    {
        orxVector_Copy(&res, &dummy);
        return orxTRUE;
    }

    return orxFALSE;
}