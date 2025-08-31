/**
 * @file Gauge.cpp
 * @date 31-Aug-2025
 */

#include "Gauge.h"

void Gauge::OnCreate()
{
    m_GaugeMax          = orxConfig_GetFloat("GaugeMax");
    m_DepletionTime     = orxConfig_GetFloat("DepletionTime");
    m_RefillTime        = orxConfig_GetFloat("RefillTime");
    m_DownTime          = orxConfig_GetFloat("DownTime");
    m_Threshold         = orxConfig_GetFloat("Threshold");
    m_CurrentValue      = orxConfig_GetFloat("Value");

    m_AutoRefill        = orxConfig_GetBool("AutoRefill");

    m_pstGaugeFill = orxObject_CreateFromConfig("GaugeFill");
    orxObject_SetParent(m_pstGaugeFill, GetOrxObject());
    orxObject_SetOwner(m_pstGaugeFill, GetOrxObject());

    SetGaugeScale();
}

void Gauge::OnDelete()
{
}

void Gauge::Update(const orxCLOCK_INFO &_rstInfo)
{
    if (m_IsInDowntime) 
    {
        m_TimeInDowntime += _rstInfo.fDT;
        if (m_TimeInDowntime >= m_DownTime)
        {
            m_IsInDowntime = orxFALSE;
        }
    }
    else if (m_AutoRefill)
    {
        Refill(_rstInfo);
    }

    SetGaugeScale();
}

void Gauge::SetGaugeScale()
{
    orxFLOAT gaugeFillScalar = orxREMAP(orxFLOAT_0, m_GaugeMax, orxFLOAT_0, orxFLOAT_1, m_CurrentValue);

    orxVECTOR scale;
    orxObject_GetScale(m_pstGaugeFill, &scale);
    orxFLOAT currentScale = scale.fY;

    orxVector_Set(&scale, orxFLOAT_1, gaugeFillScalar, orxFLOAT_1);
    orxObject_SetScale(m_pstGaugeFill, &scale);

    m_DesiredScale = gaugeFillScalar;

    if (m_CurrentValue > m_Threshold)
    {
        if (m_IsInThreshold && m_Threshold > orxFLOAT_0)
        {
            orxObject_AddTimeLineTrack(m_pstGaugeFill, "GaugeDowntimeOutTrack");
        }
        m_IsInThreshold = orxFALSE;
    }
}

orxBOOL Gauge::CanDeplete() const
{
    if (m_IsInDowntime && m_DownTime > orxFLOAT_0)
    {
        /* wait for downtime */
        return orxFALSE;
    }
    if (m_IsInThreshold) 
    {
        /* wait for threshold */
        return orxFALSE;
    }

    return m_CurrentValue > orxFLOAT_0;
}

orxFLOAT Gauge::Deplete(const orxCLOCK_INFO& _rstInfo)
{
    if (m_CurrentValue > orxFLOAT_0) 
    {
        orxFLOAT depletionRate = orxREMAP(orxFLOAT_0, m_DepletionTime, orxFLOAT_0, m_GaugeMax, _rstInfo.fDT);
        m_CurrentValue = orxMAX(m_CurrentValue - depletionRate, orxFLOAT_0);
    }

    m_IsInThreshold = m_IsInDowntime = m_CurrentValue == orxFLOAT_0;
    m_TimeInDowntime = orxFLOAT_0;

    if (m_IsInThreshold && m_Threshold > orxFLOAT_0) 
    {
        orxObject_AddTimeLineTrack(m_pstGaugeFill, "GaugeDowntimeInTrack");
    }

    m_AutoRefill    = orxFALSE;
    return m_CurrentValue;
}

orxFLOAT Gauge::Refill(const orxCLOCK_INFO& _rstInfo)
{
    if (m_CurrentValue < m_GaugeMax)
    {
        orxFLOAT refillRate = orxREMAP(orxFLOAT_0, m_RefillTime, orxFLOAT_0, m_GaugeMax, _rstInfo.fDT);
        m_CurrentValue = orxMIN(m_CurrentValue + refillRate, m_GaugeMax);
    }

    return m_CurrentValue;
}

void Gauge::Decrement(orxFLOAT amount)
{
    m_CurrentValue = orxMIN(orxMAX(orxFLOAT_0, m_CurrentValue - amount), m_GaugeMax);
}
