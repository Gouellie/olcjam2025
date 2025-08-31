/**
 * @file Gauge.h
 * @date 30-Aug-2025
 */

#ifndef __GAUGE_H__
#define __GAUGE_H__

#include "olcjam2025.h"

/** Gauge Class
 */
class Gauge : public ScrollObject
{
public:

                void                SetGaugeMax(orxFLOAT gaugeMax) { m_GaugeMax = gaugeMax; }
                void                SetDepletionTime(orxFLOAT depletionTime) { m_DepletionTime = depletionTime; }
                void                SetRefillTime(orxFLOAT refillTime) { m_RefillTime = refillTime; }
                void                SetDownTime(orxFLOAT downTime) { m_DownTime = downTime; }
                void                SetThreshold(orxFLOAT threshold) { m_Threshold = threshold; }
                void                SetAutoRefill(orxBOOL autoRefill) { m_AutoRefill = autoRefill; }

                orxFLOAT            GetCurrentValue() const { return m_CurrentValue; }
                orxBOOL             CanDeplete() const;
                void                Exhaust() { m_CurrentValue = m_GaugeMax; }
                void                RefillToMax() { m_CurrentValue = m_GaugeMax; }
                orxFLOAT            Deplete(const orxCLOCK_INFO& _rstInfo);
                orxFLOAT            Refill(const orxCLOCK_INFO& _rstInfo);

protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);

private:

                orxOBJECT*      m_pstGaugeFill;

                // configs
                orxFLOAT        m_GaugeMax;
                orxFLOAT        m_DepletionTime;
                orxFLOAT        m_RefillTime;
                orxFLOAT        m_DownTime;
                orxFLOAT        m_Threshold;
                orxBOOL         m_AutoRefill;

                // runtime
                orxFLOAT        m_CurrentValue;
                orxFLOAT        m_TimeInDowntime;
                orxFLOAT        m_DesiredScale;
                orxBOOL         m_IsInDowntime;
                orxBOOL         m_IsInThreshold;

                void            SetGaugeScale();
};

#endif // __GAUGE_H__
