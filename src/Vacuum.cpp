/**
 * @file Vacuum.cpp
 * @date 25-Aug-2025
 */

#include "Vacuum.h"

void Vacuum::OnCreate()
{
}

void Vacuum::OnDelete()
{
}

void Vacuum::Update(const orxCLOCK_INFO &_rstInfo)
{
}

void Vacuum::OnCollide(ScrollObject* _poCollider, orxBODY_PART* _pstPart, orxBODY_PART* _pstColliderPart, const orxVECTOR& _rvPosition, const orxVECTOR& _rvNormal) 
{
    PushConfigSection();
    orxFLOAT vacuumStrenght = orxConfig_GetFloat("Strenght");
    PopConfigSection();

    orxVECTOR vVacuumOrigin, vColliderPosition, vDirection;
    GetPosition(vVacuumOrigin, orxTRUE);

    _poCollider->GetPosition(vColliderPosition, orxTRUE);

    orxVector_Sub(&vDirection, &vVacuumOrigin, &vColliderPosition);

    orxVector_Normalize(&vDirection, &vDirection);

    orxVector_Mulf(&vDirection, &vDirection, vacuumStrenght);

    orxObject_ApplyImpulse(_poCollider->GetOrxObject(), &vDirection, orxNULL);

}