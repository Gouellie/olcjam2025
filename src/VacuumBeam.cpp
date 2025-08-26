/**
 * @file VacuumBeam.cpp
 * @date 26-Aug-2025
 */

#include "VacuumBeam.h"

void VacuumBeam::OnCreate()
{
    m_collidingIDs.reserve(10);
}

void VacuumBeam::OnDelete()
{
}

void VacuumBeam::Update(const orxCLOCK_INFO &_rstInfo)
{
    PushConfigSection();
    orxFLOAT vacuumStrength = orxConfig_GetFloat("Strength");
    PopConfigSection();

    for (auto iter = m_collidingIDs.begin(); iter != m_collidingIDs.end(); iter++)
    {
        if (orxOBJECT* collider = orxOBJECT(orxStructure_Get(*iter))) 
        {
            orxVECTOR vVacuumOrigin, vColliderPosition, vDirection;

            GetPosition(vVacuumOrigin, orxTRUE);
            orxObject_GetPosition(collider, &vColliderPosition);

            orxVector_Sub(&vDirection, &vVacuumOrigin, &vColliderPosition);

            orxVector_Normalize(&vDirection, &vDirection);

            orxVector_Mulf(&vDirection, &vDirection, vacuumStrength);

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