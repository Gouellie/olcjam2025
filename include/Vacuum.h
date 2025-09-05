/**
 * @file Vacuum.h
 * @date 24-Aug-2025
 */

#ifndef __VACUUM_H__
#define __VACUUM_H__

#include <stack>

#include "olcjam2025.h"

 /** Vacuum Class
  */
class Vacuum : public ScrollObject
{
public:

    orxBOOL         GetIsBeamLocked() const { return m_IsBeamLocked; }
    void            SetIsBeamLocked(orxBOOL isBeamLocked);
    void            SetIsVesselDocked(orxBOOL isVesselDocked) { m_IsIsVesselDocked = isVesselDocked; };

protected:

    void            OnCreate();
    void            OnDelete();
    void            Update(const orxCLOCK_INFO& _rstInfo);

private:

    orxBOOL         m_IsBeamActive;
    orxBOOL         m_IsBeamLocked;
    orxBOOL         m_IsIsVesselDocked;
    orxFLOAT        m_DesiredRotation = orxFLOAT_0;
    orxFLOAT        m_RotationSpeed;
    orxU64          m_vacuumHeadGUID;
    orxU64          m_vacuumBeamGUID;

    void            SetIsBeamActive(orxBOOL isBeamActive);
};

class VacuumHead : public ScrollObject
{
public:


protected:

    void            OnCreate();
    void            OnDelete();
    void            Update(const orxCLOCK_INFO& _rstInfo);
    void            OnCollide(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart, const orxVECTOR& _rvPosition, const orxVECTOR& _rvNormal);

private:
    orxBOOL         CanFireShape(const orxCLOCK_INFO& _rstInfo);
    void            FireShape();

    orxFLOAT        m_WaitForFire;
    orxFLOAT        m_FireDelay;
    orxFLOAT        m_FireSpeed;

    orxFLOAT        m_DepositDelay;
    orxFLOAT        m_WaitForDeposit;

    std::stack<orxU64> m_collection;
};

#endif // __VACUUM_H__
