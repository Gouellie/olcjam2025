#pragma once

#include "olcjam2025.h"

class CameraBox
{
public:
	orxOBJECT* m_Camera{ nullptr };
	ScrollObject* m_Target{ nullptr };
	orxVECTOR m_DesiredPos{ orxVECTOR_0 };
	orxVECTOR m_TargetLock{ orxVECTOR_0 };

	CameraBox() = default;
	void Update(const orxCLOCK_INFO& _rstInfo);

	void SetCamera(orxOBJECT* camera) { m_Camera = camera; }
	orxOBJECT* GetCamera() const { return m_Camera; }

	void SetTarget(ScrollObject* target);
	ScrollObject* GetTarget() const { return m_Target; }
};

