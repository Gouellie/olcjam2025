/**
 * @file Vacuum.h
 * @date 24-Aug-2025
 */

#ifndef __VACUUM_H__
#define __VACUUM_H__

#include "olcjam2025.h"

 /** Vacuum Class
  */
class Vacuum : public ScrollObject
{
public:


protected:

    void            OnCreate();
    void            OnDelete();
    void            Update(const orxCLOCK_INFO& _rstInfo);

private:

    orxFLOAT        m_DesiredRotation = orxFLOAT_0;
    orxU64          m_vacuumHeadGUID;
    orxU64          m_vacuumBeamGUID;
};

#endif // __VACUUM_H__
