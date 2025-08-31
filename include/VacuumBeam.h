/**
 * @file VacuumBeam.h
 * @date 24-Aug-2025
 */

#ifndef __VACUUM_BEAM_H__
#define __VACUUM_BEAM_H__

#include <vector>
#include "olcjam2025.h"

/** Object Class
 */
class VacuumBeam : public ScrollObject
{
public:


protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);

                void            OnCollide(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart, const orxVECTOR& _rvPosition, const orxVECTOR& _rvNormal);
                void            OnSeparate(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart);
private:

                std::vector<orxU64>                m_collidingIDs;
                orxFLOAT                           m_vacuumStrength;
                orxFLOAT                           m_vacuumDistanceMultiplier;
                orxFLOAT                           m_vacuumStrengthMultiplierNegativeShapes;
};

#endif // __VACUUM_BEAM_H__
