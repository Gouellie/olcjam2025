/**
 * @file World.h
 * @date 24-Aug-2025
 */

#ifndef __WORLD_H__
#define __WORLD_H__

#include "olcjam2025.h"

/** World Class
 */
class World : public ScrollObject
{
public:

protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);

private:

                void            ApplySettings();

                orxVECTOR       m_PreviousPlayerPos;
                orxHASHTABLE*   m_WorldTable;
                orxS32          m_Settings;
                orxU64          m_Universe;
};

#endif // __WORLD_H__
