/**
 * @file Gauge.cpp
 * @date 31-Aug-2025
 */

#include "Gauge.h"

void Gauge::OnCreate()
{
    m_pstGaugeFill = orxObject_CreateFromConfig("GaugeFill");
    orxObject_SetParent(m_pstGaugeFill, GetOrxObject());
    orxObject_SetOwner(m_pstGaugeFill, GetOrxObject());
}

void Gauge::OnDelete()
{
}

void Gauge::Update(const orxCLOCK_INFO &_rstInfo)
{
}
