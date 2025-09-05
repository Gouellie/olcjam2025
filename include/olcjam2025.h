/**
 * @file olcjam2025.h
 * @date 24-Aug-2025
 */

#ifndef __olcjam2025_H__
#define __olcjam2025_H__

#include "Scroll/Scroll.h"

/** Game Class
 */
class olcjam2025 : public Scroll<olcjam2025>
{
public:

                orxBOOL         GetIsUsingPad() const { return m_bIsUsingPad; }
                void            SetIsUsingPad(const orxBOOL isUsingPad) { m_bIsUsingPad = isUsingPad; }

private:

                orxSTATUS       Bootstrap() const;

                void            Update(const orxCLOCK_INFO &_rstInfo);
                void            CameraUpdate(const orxCLOCK_INFO &_rstInfo);

                orxSTATUS       Init();
                orxSTATUS       Run();
                void            Exit();
                void            BindObjects();

private:
                
                orxBOOL         m_bIsUsingPad       = orxFALSE;

                orxBOOL         m_bIsInTitleScreen      = orxTRUE;
                orxBOOL         m_bIsOptionScreenShown  = orxFALSE;
};

#endif // __olcjam2025_H__
