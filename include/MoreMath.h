#pragma once

#include "orx.h"

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

// Function to check if a point lies on a given line segment
static orxBOOL onSegment(const orxVECTOR& p, const orxVECTOR& q, const orxVECTOR& r) {
    if (q.fX <= orxMAX(p.fX, r.fX) && q.fX >= orxMIN(p.fX, r.fX)
        && q.fY <= orxMAX(p.fY, r.fY) && q.fY >= orxMIN(p.fY, r.fY))
        return orxTRUE;
    return orxFALSE;
}

// Function to find the orientation of the ordered triplet
// (p, q, r) 0 -> p, q and r are collinear 1 -> Clockwise 2
// -> Counterclockwise
static orxS32 orientation(const orxVECTOR& p, const orxVECTOR& q, const orxVECTOR& r) {
    orxFLOAT val = (q.fY - p.fY) * (r.fX - q.fX)
        - (q.fX - p.fX) * (r.fY - q.fY);
    if (val == 0)

        // Collinear
        return 0;

    // Clock or counter clock wise
    return (val > 0) ? 1 : 2;
}

// Function to check if two lines (p1-q1 and p2-q2) intersect
static orxBOOL doIntersect(const orxVECTOR& p1, const orxVECTOR& q1, const orxVECTOR& p2, const orxVECTOR& q2,
    orxVECTOR& res) {
    res.fZ = orxFLOAT_0;

    // Find the four orientations needed for general and
    // special cases
    orxS32 o1 = orientation(p1, q1, p2);
    orxS32 o2 = orientation(p1, q1, q2);
    orxS32 o3 = orientation(p2, q2, p1);
    orxS32 o4 = orientation(p2, q2, q1);

    // General case: lines intersect if they have different
    // orientations
    if (o1 != o2 && o3 != o4) {

        // Compute intersection point
        orxFLOAT a1 = q1.fY - p1.fY;
        orxFLOAT b1 = p1.fX - q1.fX;
        orxFLOAT c1 = a1 * p1.fX + b1 * p1.fY;
        
        orxFLOAT a2 = q2.fY - p2.fY;
        orxFLOAT b2 = p2.fX - q2.fX;
        orxFLOAT c2 = a2 * p2.fX + b2 * p2.fY;

        orxFLOAT determinant = a1 * b2 - a2 * b1;

        if (determinant != 0)
        {
            res.fX = (c1 * b2 - c2 * b1) / determinant;
            res.fY = (a1 * c2 - a2 * c1) / determinant;
            return orxTRUE;
        }
    }

    // Special Cases: check if the lines are collinear and
    // overlap
    if (o1 == 0 && onSegment(p1, p2, q1))
        return orxTRUE;
    if (o2 == 0 && onSegment(p1, q2, q1))
        return orxTRUE;
    if (o3 == 0 && onSegment(p2, p1, q2))
        return orxTRUE;
    if (o4 == 0 && onSegment(p2, q1, q2))
        return orxTRUE;

    // Lines do not intersect in any case
    return orxFALSE;
}
