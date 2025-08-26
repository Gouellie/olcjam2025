#pragma once

#include "olcjam2025.h"

class CameraBox : public ScrollObject
{
public:
	orxCAMERA* ownerCamera;
	orxOBJECT* m_Target;
	orxVECTOR m_DesiredPos;
	orxVECTOR m_TargetLock;

	void OnCreate() override;
	void OnDelete() override;
	void Update(const orxCLOCK_INFO& _rstInfo) override;
};

