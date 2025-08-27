#pragma once

#include "olcjam2025.h"

static orxFLOAT orxFASTCALL angle_difference(orxFLOAT p_from, orxFLOAT p_to) {
    orxFLOAT difference = orxMath_Mod(p_to - p_from, orxMATH_KF_2_PI);
    return orxMath_Mod(2.0f * difference, orxMATH_KF_2_PI) - difference;
}

static orxFLOAT orxFASTCALL lerp_angle(orxFLOAT p_from, orxFLOAT p_to, orxFLOAT p_weight) {
    return p_from + angle_difference(p_from, p_to) * p_weight;
}

static orxFLOAT orxFASTCALL get_angle(orxVECTOR& p_v)
{
    return orxMath_ATan(p_v.fY, p_v.fX) - orxMATH_KF_PI_BY_2;
}
