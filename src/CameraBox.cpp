#include "CameraBox.h"

#include <math/orxMath.h>
#include <math/orxVector.h>

void CameraBox::SetTarget(ScrollObject* target)
{
    m_Target = target;
    m_TargetLock = m_Target->GetPosition(m_TargetLock);
}

void CameraBox::Update(const orxCLOCK_INFO& _rstInfo)
{
    orxConfig_PushSection("CameraBox");
	// Special case for "mining" activity

	// Check if player is outside of bounds


	// Set target position based on where player is

	// Lerp Camera towards target position

    orxVECTOR delta = orxVECTOR_0;

    //X Axis
    orxVECTOR targetPos = m_Target->GetPosition(targetPos);
    orxFLOAT dx = targetPos.fX - m_TargetLock.fX;
    orxVECTOR cameraPrevPos = orxVECTOR_0;
    cameraPrevPos = *orxObject_GetPosition(m_Camera, &cameraPrevPos);

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
            delta.fY = orxMath_Abs(dy) - boundTop;
    }
    else
    {
        const orxFLOAT boundBottom = orxConfig_GetFloat("BoundBottom");
        if (orxMath_Abs(dy) > boundBottom)
            delta.fY = -(orxMath_Abs(dy) - boundBottom);
    }

    //Move the camera and smoothing
    m_DesiredPos = *orxVector_Add(&m_DesiredPos, &m_TargetLock, &delta);
    m_TargetLock = m_DesiredPos;
    orxVECTOR pos = *orxVector_Lerp(&pos, &cameraPrevPos, &m_DesiredPos, orxConfig_GetFloat("LerpSpeed") * _rstInfo.fDT);
    orxObject_SetPosition(m_Camera, &pos);

    orxConfig_PopSection();
}