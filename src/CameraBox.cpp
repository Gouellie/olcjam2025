#include "CameraBox.h"

#include <math/orxMath.h>
#include <math/orxVector.h>

#include "Gauge.h"

void CameraBox::SetTarget(ScrollObject* target)
{
    m_Target = target;
    m_TargetLock = m_Target->GetPosition(m_TargetLock);
}

void CameraBox::Update(const orxCLOCK_INFO& _rstInfo)
{
    orxConfig_PushSection("CameraBox");
	// Special case for "mining" activity

    orxVECTOR delta = orxVECTOR_0;

    orxVECTOR targetPos = m_Target->GetPosition(targetPos);

    orxVECTOR cameraPrevPos = orxVECTOR_0;
    cameraPrevPos = *orxObject_GetPosition(m_Camera, &cameraPrevPos);

    orxVECTOR speed = orxVECTOR_0;
    speed = m_Target->GetSpeed(speed);
    // I'd like to have something more generic instead, but this is a patch
    //if (orxVector_IsNull(&speed))
    if (!TargetIsMoving())
    {
        m_TargetLock = targetPos;
    }
    else
    {
        //X Axis
        orxFLOAT dx = targetPos.fX - m_TargetLock.fX;

        if (targetPos.fX > m_TargetLock.fX)
        {
            const orxFLOAT boundRight = orxConfig_GetFloat("BoundRight");
            if (orxMath_Abs(dx) > boundRight)
            {
                delta.fX = orxMath_Abs(dx) - boundRight;
            }
        }
        else
        {
            const orxFLOAT boundLeft = orxConfig_GetFloat("BoundLeft");
            if (orxMath_Abs(dx) > boundLeft)
            {
                delta.fX = -(orxMath_Abs(dx) - boundLeft);
            }
        }

        //Y Axis
        orxFLOAT dy = targetPos.fY - m_TargetLock.fY;
        if (dy > 0)
        {
            const orxFLOAT boundTop = orxConfig_GetFloat("BoundTop");
            if (orxMath_Abs(dy) > boundTop)
            {
                delta.fY = orxMath_Abs(dy) - boundTop;
            }
        }
        else
        {
            const orxFLOAT boundBottom = orxConfig_GetFloat("BoundBottom");
            if (orxMath_Abs(dy) > boundBottom)
            {
                delta.fY = -(orxMath_Abs(dy) - boundBottom);
            }
        }
    }

    //Move the camera and smoothing
    m_DesiredPos = *orxVector_Add(&m_DesiredPos, &m_TargetLock, &delta);

    if (GetBeamActive())
    {
        m_BeamFacing = orxVECTOR_Y;
        orxVector_2DRotate(&m_BeamFacing, &m_BeamFacing, GetBeamPosition());
        m_BeamFacing = *orxVector_Mulf(&m_BeamFacing, &m_BeamFacing, GetBeamLength() * orxConfig_GetFloat("FacingRatio"));
        m_DesiredPos = *orxVector_Add(&m_DesiredPos, &m_DesiredPos, &m_BeamFacing);
    }

    m_TargetLock = m_DesiredPos;
    orxVECTOR pos = *orxVector_Lerp(&pos, &cameraPrevPos, &m_DesiredPos, orxConfig_GetFloat("LerpSpeed") * _rstInfo.fDT);
    orxObject_SetPosition(m_Camera, &pos);

    m_CameraPos = pos;

    orxConfig_PopSection();
}

orxBOOL CameraBox::GetBeamActive() const
{
    if (m_VacuumLocked) 
    {
        return orxFALSE;
    }
    if (Gauge* shapeGauge = (Gauge*)olcjam2025::GetInstance().GetObject("Runtime", "GaugeShapes")) 
    {
        if (shapeGauge->GetIsMaxedOut()) 
        {
            return orxFALSE;
        }
    }

    return orxInput_IsActive("Vacuum");
}

orxFLOAT CameraBox::GetBeamPosition() const
{
    if (orxOBJECT* vacuum = orxOBJECT(orxStructure_Get(m_VacuumGUID)))
    {
        return orxObject_GetRotation(vacuum);
    }

    return orxFLOAT_0;
}

orxFLOAT CameraBox::GetBeamLength() const
{
    orxVECTOR beamSize = orxVECTOR_0;
    orxConfig_PushSection("VacuumBeam");
    beamSize = *orxConfig_GetVector("Size", &beamSize);
    orxConfig_PopSection();
    return beamSize.fY;
}

orxBOOL CameraBox::TargetIsMoving() const
{
    return orxInput_IsActive("Right") | orxInput_IsActive("Left") | orxInput_IsActive("Down") | orxInput_IsActive("Up");
}