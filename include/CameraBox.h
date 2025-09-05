#pragma once

#include "olcjam2025.h"

class CameraBox
{
public:

    void GetCameraPosition(orxVECTOR& position) const { position = m_CameraPos; }

	orxU64 m_VacuumGUID;
	orxOBJECT* m_Camera{ nullptr };
	ScrollObject* m_Target{ nullptr };
	orxVECTOR m_CameraPos{ orxVECTOR_0 };
	orxVECTOR m_DesiredPos{ orxVECTOR_0 };
	orxVECTOR m_TargetLock{ orxVECTOR_0 };
	orxVECTOR m_BeamFacing{ orxVECTOR_Y };

	CameraBox() = default;
	void Update(const orxCLOCK_INFO& _rstInfo);

	void SetCamera(orxOBJECT* camera) { m_Camera = camera; }
	orxOBJECT* GetCamera() const { return m_Camera; }

	void SetTarget(ScrollObject* target);
	ScrollObject* GetTarget() const { return m_Target; }

	void SetVacuumID(orxU64 vacuumId) { m_VacuumGUID = vacuumId; }

	orxBOOL GetBeamActive() const;
	orxFLOAT GetBeamPosition() const;
	orxFLOAT GetBeamLength() const;
	orxBOOL TargetIsMoving() const;
};

