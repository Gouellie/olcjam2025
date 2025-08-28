/**
 * @file LoadingGrid.h
 * @date 24-Aug-2025
 */

#ifndef __LOADING_GRID_H__
#define __LOADING_GRID_H__

#include "olcjam2025.h"

/** LoadingGrid Class
 */
class LoadingGrid : public ScrollObject
{
public:

                const orxOBJECT*    GetCellAtPosition(const orxVECTOR& position) const;
                const orxFLOAT      GetCellSize() const { return m_CellSize; }

protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);

private:

                orxHASHTABLE*   m_CellTable;
                orxFLOAT        m_CellSize;
                orxS32          m_LoadingRange;
                 
                const orxSTRING m_CellType;

                orxVECTOR       m_PreviousPlayerPos;
};

#endif // __LOADING_GRID_H__
