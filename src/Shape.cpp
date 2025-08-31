/**
 * @file Shape.cpp
 * @date 27-Aug-2025
 */

#include "Shape.h"
#include "MoreMath.h"

void Shape::OnCreate()
{
    m_ShapeDamageBlow = orxConfig_GetFloat("DamageBlow");
    m_ShapeDamageBounce = orxConfig_GetFloat("DamageBounce");
    GetScale(m_vOriginalScale);
    GetSpeed(m_vPreviousSpeed);
}

void Shape::OnDelete()
{
}

void Shape::Update(const orxCLOCK_INFO &_rstInfo)
{
    if (m_IsInTractorBeam) 
    {
        orxVECTOR vSpeed;
        GetSpeed(vSpeed);
        if (!orxVector_AreEqual(&vSpeed, &m_vPreviousSpeed))
        {
            orxVECTOR vScale;
            orxFLOAT speed = orxVector_GetSize(&vSpeed);
            orxFLOAT scale = orxREMAP(orxFLOAT_0, orxFLOAT_1, m_vOriginalScale.fX, m_vOriginalScale.fX * 0.6f, orxMath_SmootherStep(80.f, 3000.f, speed));
            orxVector_Set(&vScale, scale, scale, 1.0f);
            SetScale(vScale);

            m_vPreviousSpeed = vSpeed;
        }
    }
    else
    {
        orxVECTOR vScale;
        GetScale(vScale);
        if (!orxVector_AreEqual(&vScale, &m_vOriginalScale))
        {
            orxVector_Lerp(&vScale, &vScale, &m_vOriginalScale, _rstInfo.fDT * 10.0f);
            SetScale(vScale);
        }
    }
}