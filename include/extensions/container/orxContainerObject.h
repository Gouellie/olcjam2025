//! Includes

#ifndef _ORXCONTAINEROBJECT_H_
#define _ORXCONTAINEROBJECT_H_

#include "../Scroll/Scroll.h"
#include "orxContainerBank.h"

 /** Config defines
  */
#define orxCONTAINEROBJECT_KZ_CONFIG_MARGIN                   "Margin"
#define orxCONTAINEROBJECT_KZ_CONFIG_SPACING                  "Spacing"

#define orxCONTAINEROBJECT_KZ_CONFIG_CONTAINER_TYPE           "ContainerType"
#define orxCONTAINEROBJECT_KZ_CONFIG_STACK_PANEL              "StackPanel"
#define orxCONTAINEROBJECT_KZ_CONFIG_STACK_GRID               "Grid"

#define orxCONTAINEROBJECT_KZ_CONFIG_ORIENTATION              "Orientation"
#define orxCONTAINEROBJECT_KZ_CONFIG_ORIENTATION_HORIZONTAL   "horizontal"
#define orxCONTAINEROBJECT_KZ_CONFIG_ORIENTATION_VERTICAL     "vertical"

#define orxCONTAINEROBJECT_KZ_CONFIG_ALIGNMENT                "Alignment"

typedef struct __orxCONTAINER_MARGIN_t
{
  orxFLOAT fLeft;
  orxFLOAT fTop;
  orxFLOAT fRight;
  orxFLOAT fBottom;
} orxCONTAINER_MARGIN;

typedef enum __orxCONTAINER_TYPE_t
{
  orxCONTAINER_TYPE_STACK_PANEL = 0,
  orxCONTAINER_TYPE_GRID,

  orxCONTAINER_TYPE_NONE = orxENUM_NONE           /**< Invalid status */

} orxCONTAINER_TYPE;

typedef enum __orxCONTAINER_ORIENTATION_t
{
  orxCONTAINER_ORIENTATION_VERTICAL = 0,
  orxCONTAINER_ORIENTATION_HORIZONTAL,

  orxCONTAINER_ORIENTATION_NONE = orxENUM_NONE    /**< Invalid status */

} orxCONTAINER_ORIENTATION;

//! orxContainerObject class
class orxContainerObject : public ScrollObject
{

public:

                virtual void                  GetMargin(orxVECTOR& vMargin)   const;
                virtual void                  GetOrigin(orxVECTOR& vOrigin)   const;

                virtual void                  SortChildren();
                virtual void                  FitChildInRect(orxOBJECT* _pstObject, const orxOBOX& p_rect);

protected:

                void                          OnCreate();
                void                          OnDelete();
                void                          Update(const orxCLOCK_INFO& _rstInfo);

                virtual void                  SetChildOrigin(orxOBJECT* _pstChildObject, orxVECTOR& _vChildOrigin);
                virtual void                  GetAnchorOffset(const orxVECTOR& vSize, orxVECTOR& vSpacing) const;

private:

                orxCONTAINER_TYPE             m_eContainerType            = orxCONTAINER_TYPE_STACK_PANEL;
                orxCONTAINER_ORIENTATION      m_eContainerOrientation     = orxCONTAINER_ORIENTATION_VERTICAL;

                orxS32                        m_s32Spacing                = orxS32(0);
                orxCONTAINER_MARGIN           m_stMargin                  = {};

                orxU32                        m_u32AlingFlags             = orxGRAPHIC_KU32_FLAG_ALIGN_CENTER;

                orxBOOL                       m_bNeedUpdate               = orxTRUE;
                orxVECTOR                     m_vPreviousSize;
                orxFLOAT                      m_fRotation;

                virtual void                  GetFinalOffset(const orxVECTOR& vRectangleSize, const orxVECTOR& vChildSize, orxVECTOR& vOffset) const;

private:

};

#ifdef orxCONTAINER_IMPL

void orxContainerObject::OnCreate()
{
  // cache size
  GetSize(m_vPreviousSize);
  m_fRotation = GetRotation();

  /* Default initialization */
  m_eContainerType        = orxCONTAINER_TYPE::orxCONTAINER_TYPE_STACK_PANEL;
  m_eContainerOrientation = orxCONTAINER_ORIENTATION::orxCONTAINER_ORIENTATION_VERTICAL;

  /* Set Container Type */
  if (orxConfig_HasValue(orxCONTAINEROBJECT_KZ_CONFIG_CONTAINER_TYPE))
  {
    const orxSTRING zContainerType = orxConfig_GetString(orxCONTAINEROBJECT_KZ_CONFIG_CONTAINER_TYPE);
    if (orxString_Compare(zContainerType, orxCONTAINEROBJECT_KZ_CONFIG_STACK_PANEL) == 0)
    {
      m_eContainerType = orxCONTAINER_TYPE::orxCONTAINER_TYPE_STACK_PANEL;
    }
    else if (orxString_Compare(zContainerType, orxCONTAINEROBJECT_KZ_CONFIG_STACK_GRID) == 0)
    {
      m_eContainerType = orxCONTAINER_TYPE::orxCONTAINER_TYPE_GRID;
    }
  }

  /* Set Container Orientation */
  if (orxConfig_HasValue(orxCONTAINEROBJECT_KZ_CONFIG_ORIENTATION))
  {
    const orxSTRING zContainerOrientation = orxConfig_GetString(orxCONTAINEROBJECT_KZ_CONFIG_ORIENTATION);
    if (orxString_Compare(zContainerOrientation, orxCONTAINEROBJECT_KZ_CONFIG_ORIENTATION_HORIZONTAL) == 0)
    {
      m_eContainerOrientation = orxCONTAINER_ORIENTATION::orxCONTAINER_ORIENTATION_HORIZONTAL;
    }
    else
    {
      m_eContainerOrientation = orxCONTAINER_ORIENTATION::orxCONTAINER_ORIENTATION_VERTICAL;
    }
  }

  /* Set Spacing */
  if (orxConfig_HasValue(orxCONTAINEROBJECT_KZ_CONFIG_SPACING))
  {
    m_s32Spacing = orxConfig_GetS32(orxCONTAINEROBJECT_KZ_CONFIG_SPACING);
  }

  /* Set Margin */
  if (orxConfig_HasValue(orxCONTAINEROBJECT_KZ_CONFIG_MARGIN))
  {
    orxS32 sMarginCount;
    sMarginCount = orxConfig_GetListCount(orxCONTAINEROBJECT_KZ_CONFIG_MARGIN);
    if (sMarginCount == 1)
    {
      /* Uniform Margin */
      orxFLOAT fMargin_Uniform = orxConfig_GetListFloat(orxCONTAINEROBJECT_KZ_CONFIG_MARGIN, 0);
      m_stMargin = { fMargin_Uniform, fMargin_Uniform, fMargin_Uniform, fMargin_Uniform };
    }
    else if (sMarginCount == 2)
    {
      /* Horizontal - Vertical Margin */
      orxFLOAT fMargin_Horizontal = orxConfig_GetListFloat(orxCONTAINEROBJECT_KZ_CONFIG_MARGIN, 0);
      orxFLOAT fMargin_Vertical   = orxConfig_GetListFloat(orxCONTAINEROBJECT_KZ_CONFIG_MARGIN, 1);
      m_stMargin = { fMargin_Horizontal, fMargin_Vertical, fMargin_Horizontal, fMargin_Vertical };
    }
    else if (sMarginCount == 4)
    {
      /* Custom Margin */
      orxFLOAT fMargin_Left   = orxConfig_GetListFloat(orxCONTAINEROBJECT_KZ_CONFIG_MARGIN, 0);
      orxFLOAT fMargin_Top    = orxConfig_GetListFloat(orxCONTAINEROBJECT_KZ_CONFIG_MARGIN, 1);
      orxFLOAT fMargin_Right  = orxConfig_GetListFloat(orxCONTAINEROBJECT_KZ_CONFIG_MARGIN, 2);
      orxFLOAT fMargin_Bottom = orxConfig_GetListFloat(orxCONTAINEROBJECT_KZ_CONFIG_MARGIN, 3);
      m_stMargin = { fMargin_Left, fMargin_Top, fMargin_Right, fMargin_Bottom };
    }
  }

  if (orxConfig_HasValue(orxCONTAINEROBJECT_KZ_CONFIG_ALIGNMENT))
  {
    m_u32AlingFlags = orxGraphic_GetAlignFlags(orxConfig_GetString(orxCONTAINEROBJECT_KZ_CONFIG_ALIGNMENT));
  }

  /* Register Object */
  orxContainerBank_RegisterContainer(GetOrxObject());
}

void orxContainerObject::OnDelete()
{
    orxContainerBank_UnregisterContainer(GetOrxObject());
}

void orxContainerObject::Update(const orxCLOCK_INFO& _rstInfo)
{
  orxFLOAT fCurrentRotation = GetRotation();
  if (fCurrentRotation != m_fRotation)
  {
    m_fRotation = fCurrentRotation;
    m_bNeedUpdate = orxTRUE;
  }

  orxVECTOR vCurrentSize;
  GetSize(vCurrentSize);
  if (orxVector_GetDistance(&vCurrentSize, &m_vPreviousSize))
  {
    orxVector_Copy(&m_vPreviousSize, &vCurrentSize);
    m_bNeedUpdate = orxTRUE;
  }
}

void orxContainerObject::GetOrigin(orxVECTOR& vOrigin) const
{
  orxOBOX stBoundingBox;
  orxObject_GetBoundingBox(GetOrxObject(), &stBoundingBox);

  orxVECTOR vTopLeft;
  orxVector_Sub(&vTopLeft, &(stBoundingBox.vPosition), &(stBoundingBox.vPivot));

  orxVector_Copy(&vOrigin, &vTopLeft);

  /* Align right? */
  if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_RIGHT))
  {
    orxVector_Add(&vOrigin, &vOrigin, &(stBoundingBox.vX));
  }
  else if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_LEFT))
  {
    // nothing
  }
  /* Align center */
  else
  {
    orxVECTOR vHalfX;
    orxVector_Mulf(&vHalfX, &(stBoundingBox.vX),0.5f);
    orxVector_Add(&vOrigin, &vOrigin, &vHalfX);
  }

  /* Align bottom? */
  if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_BOTTOM))
  {
    orxVector_Add(&vOrigin, &vOrigin, &(stBoundingBox.vY));
  }
  else if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_TOP))
  {
    // nothing
  }
  /* Align center */
  else
  {
    orxVECTOR vHalfY;
    orxVector_Mulf(&vHalfY, &(stBoundingBox.vY), 0.5f);
    orxVector_Add(&vOrigin, &vOrigin, &vHalfY);
  }
}

void orxContainerObject::SortChildren()
{
  if (m_bNeedUpdate)
  {
    m_bNeedUpdate = orxFALSE;

    orxVECTOR vOrigin, vMargin, vAnchor;
    GetOrigin(vOrigin);
    GetMargin(vMargin);
    orxVector_Copy(&vAnchor, &vOrigin);
    orxVector_Add(&vAnchor, &vAnchor, &vMargin);

    orxVECTOR vRectDimension, vAnchorOffset, vSize;
    vRectDimension  = orxVECTOR_0;
    vAnchorOffset   = orxVECTOR_0;
    vSize           = orxVECTOR_0;

    orxU8 childCount = 0;
    for (orxOBJECT* pstChild = orxObject_GetOwnedChild(GetOrxObject());
      pstChild != orxNULL;
      pstChild = orxObject_GetOwnedSibling(pstChild))
    {
      /* Layout Children according to Container Orientation and calculate Rectangle Dimension */
      orxObject_GetSize(pstChild, &vSize);
      SetChildOrigin(pstChild, vAnchor);
      GetAnchorOffset(vSize, vAnchorOffset);
      orxVector_Add(&vAnchor, &vAnchor, &vAnchorOffset);

      switch (m_eContainerOrientation)
      {
        case orxCONTAINER_ORIENTATION::orxCONTAINER_ORIENTATION_HORIZONTAL:
          vRectDimension.fX += vSize.fX;
          vRectDimension.fY = orxMAX(vSize.fY, vRectDimension.fY);
          break;
        case orxCONTAINER_ORIENTATION::orxCONTAINER_ORIENTATION_VERTICAL:
          vRectDimension.fX = orxMAX(vSize.fX, vRectDimension.fX);
          vRectDimension.fY += vSize.fY;
          break;
      }

      childCount++;
    }

    /* No children, nothing to sort */
    if (childCount == 0)
    {
      return;
    }

    /* Top Left alignment requires no further steps */
    if (orxFLAG_TEST_ALL(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_TOP | orxGRAPHIC_KU32_FLAG_ALIGN_LEFT))
    {
      return;
    }

    orxS32 spacing = (childCount - 1) * m_s32Spacing;

    switch (m_eContainerOrientation)
    {
      case orxCONTAINER_ORIENTATION::orxCONTAINER_ORIENTATION_HORIZONTAL:
        vRectDimension.fX += spacing;
        break;
      case orxCONTAINER_ORIENTATION::orxCONTAINER_ORIENTATION_VERTICAL:
        vRectDimension.fY += spacing;
        break;
    }

    orxVECTOR vPos, vOffset;
    for (orxOBJECT* pstChild = orxObject_GetOwnedChild(GetOrxObject());
      pstChild != orxNULL;
      pstChild = orxObject_GetOwnedSibling(pstChild))
    {
      orxObject_GetPosition(pstChild, &vPos);
      orxObject_GetSize(pstChild, &vSize);
      GetFinalOffset(vRectDimension, vSize, vOffset);

      orxVector_Add(&vPos, &vPos, &vOffset);
      orxObject_SetPosition(pstChild, &vPos);
    }
  }
}

void orxContainerObject::SetChildOrigin(orxOBJECT* _pstChildObject, orxVECTOR& _vChildOrigin)
{
  orxOBOX stBoundingBox;
  orxObject_GetBoundingBox(_pstChildObject, &stBoundingBox);

  orxVECTOR vPosition, vObjectOrigin, vOffset, vCenter;
  orxObject_GetPosition(_pstChildObject, &vPosition);
  orxOBox_GetCenter(&stBoundingBox, &vCenter);

  orxFLOAT fZ = vPosition.fZ;

  /* Gets current origin TOP LEFT */
  orxVector_Sub(&vObjectOrigin, &(stBoundingBox.vPosition), &(stBoundingBox.vPivot));

  /* Gets offset to destination */
  orxVector_Sub(&vOffset, &_vChildOrigin, &vObjectOrigin);

  /* Clear Z */
  vOffset.fZ = orxFLOAT_0;

  /* Update Position with Offset */
  orxVector_Add(&vPosition, &vOffset, &vPosition);

  /* Update Position */
  orxObject_SetPosition(_pstChildObject, &vPosition);
}

void orxContainerObject::FitChildInRect(orxOBJECT* _pstObject, const orxOBOX& p_rect)
{
  
}

void orxContainerObject::GetMargin(orxVECTOR& vMargin) const
{
  /* Init */
  vMargin = orxVECTOR_0;

  /* Align left? */
  if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_LEFT))
  {
    vMargin.fX = m_stMargin.fLeft;
  }
  /* Align right? */
  else if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_RIGHT))
  {
    vMargin.fX = -m_stMargin.fRight;
  }

  /* Align top? */
  if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_TOP))
  {
    vMargin.fY = m_stMargin.fTop;
  }
  /* Align bottom? */
  else if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_BOTTOM))
  {
    vMargin.fY = -m_stMargin.fBottom;
  }

  orxFLOAT fRotation = GetRotation();
  if (fRotation) {
    orxVector_2DRotate(&vMargin, &vMargin, GetRotation());
  }
}

void orxContainerObject::GetAnchorOffset(const orxVECTOR& vSize, orxVECTOR& vAnchorOffset) const
{
  /* Init */
  vAnchorOffset = orxVECTOR_0;
  if (m_eContainerOrientation == orxCONTAINER_ORIENTATION::orxCONTAINER_ORIENTATION_VERTICAL)
  {
    /* top going down */
    vAnchorOffset.fX = orxFLOAT_0;
    vAnchorOffset.fY = vSize.fY + m_s32Spacing;
    vAnchorOffset.fZ = orxFLOAT_0;
  }
  else
  {
    /* left going right */
    vAnchorOffset.fX = vSize.fX + m_s32Spacing;
    vAnchorOffset.fY = orxFLOAT_0;
    vAnchorOffset.fZ = orxFLOAT_0;
  }

  orxFLOAT fRotation = GetRotation();
  if (fRotation) {
    orxVector_2DRotate(&vAnchorOffset, &vAnchorOffset, GetRotation());
  }
}

inline void orxContainerObject::GetFinalOffset(const orxVECTOR& vRectangleSize, const orxVECTOR& vChildSize, orxVECTOR& vOffset) const
{
  vOffset = orxVECTOR_0;

  if (m_eContainerOrientation == orxCONTAINER_ORIENTATION::orxCONTAINER_ORIENTATION_VERTICAL) /* top going down */
  {
    if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_RIGHT)) /* right aligned */
    {
      vOffset.fX -= vChildSize.fX;
    }
    else if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_LEFT) == false) /* no left/right alignment */
    {
      /* centered horizontally */
      vOffset.fX -= (vChildSize.fX / 2.f);
    }

    if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_BOTTOM)) /* bottom aligned */
    {
      vOffset.fY -= vRectangleSize.fY;
    }
    else if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_TOP) == false) /* centered */
    {
      vOffset.fY -= (vRectangleSize.fY / 2.f);
    }
  }
  else /* left going right */
  {
    if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_RIGHT))
    {
      vOffset.fX -= vRectangleSize.fX;
    }
    else if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_LEFT) == false) /* no left/right alignment */
    {
      /* centered horizontally */
      vOffset.fX -= (vRectangleSize.fX / 2.f);
    }

    if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_BOTTOM)) /* bottom aligned */
    {
      vOffset.fY -= vChildSize.fY;
    }
    else if (orxFLAG_TEST(m_u32AlingFlags, orxGRAPHIC_KU32_FLAG_ALIGN_TOP) == false) /* no top/bottom alignment */
    {
      /* centered vertically */
      vOffset.fY = -(vChildSize.fY / 2.f);
    }
  }
}

#endif // orxCONTAINER_IMPL

#endif // _ORXCONTAINEROBJECT_H_
