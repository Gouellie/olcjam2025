/**
 * @file Starbase.h
 * @date 28-Aug-2025
 */

#ifndef __STARBASE_H__
#define __STARBASE_H__

#include "olcjam2025.h"

/** Starbase Class
 */
class Starbase : public ScrollObject
{
public:


protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);


private:
};

#endif // __STARBASE_H__
