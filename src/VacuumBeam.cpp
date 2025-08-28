/**
 * @file VacuumBeam.cpp
 * @date 26-Aug-2025
 */

#include "VacuumBeam.h"

void VacuumBeam::OnCreate()
{
    m_collidingIDs.reserve(10);
    m_vacuumStrength = orxConfig_GetFloat("Strength");
    m_vacuumDistanceMultiplier = orxConfig_GetFloat("DistanceMult");
}

void VacuumBeam::OnDelete()
{
}

void VacuumBeam::Update(const orxCLOCK_INFO &_rstInfo)
{
    for (auto iter = m_collidingIDs.begin(); iter != m_collidingIDs.end(); iter++)
    {
        if (orxOBJECT* collider = orxOBJECT(orxStructure_Get(*iter))) 
        {
            orxVECTOR vVacuumOrigin, vColliderPosition, vDirection, vSize;

            GetSize(vSize);
            GetPosition(vVacuumOrigin, orxTRUE);
            orxObject_GetWorldPosition(collider, &vColliderPosition);

            orxVector_Sub(&vDirection, &vVacuumOrigin, &vColliderPosition);

            const orxFLOAT distanceToOrigin = orxVector_GetDistance(&vVacuumOrigin, &vColliderPosition);

            const orxFLOAT strengthMultiplier = orxMath_Pow(2, orxREMAP(orxFLOAT_0, orxFLOAT_1, m_vacuumDistanceMultiplier, orxFLOAT_1, orxMath_SmootherStep(vSize.fY / 2.0f, vSize.fY * 3.0f, distanceToOrigin)));

            orxVector_Normalize(&vDirection, &vDirection);

            orxVector_Mulf(&vDirection, &vDirection, m_vacuumStrength * strengthMultiplier * _rstInfo.fDT);

            orxObject_ApplyForce(collider, &vDirection, orxNULL);
        }
    }
}

void VacuumBeam::OnCollide(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart, const orxVECTOR& _rvPosition, const orxVECTOR& _rvNormal)
{
   m_collidingIDs.push_back(orxStructure_GetGUID(_poCollider->GetOrxObject()));
}

void VacuumBeam::OnSeparate(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart) 
{
    orxU64 guid = orxStructure_GetGUID(_poCollider->GetOrxObject());
    for (auto iter = m_collidingIDs.begin(); iter != m_collidingIDs.end();) {
        if (*iter == guid) {
            iter = m_collidingIDs.erase(iter);
        }
        else {
            ++iter;
        }
    }
}