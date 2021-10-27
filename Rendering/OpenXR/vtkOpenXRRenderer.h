/*=========================================================================

Program:   Visualization Toolkit

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkOpenXRRenderer
 * @brief   OpenXR renderer
 *
 * vtkOpenXRRenderer is a concrete implementation of the abstract class
 * vtkRenderer.  vtkOpenXRRenderer interfaces to the OpenXR rendering library.
 */

#ifndef vtkOpenXRRenderer_h
#define vtkOpenXRRenderer_h

#include "vtkNew.h" // To init FloorActor in header
#include "vtkOpenGLRenderer.h"
#include "vtkRenderingOpenXRModule.h" // For export macro

class vtkActor;

class VTKRENDERINGOPENXR_EXPORT vtkOpenXRRenderer : public vtkOpenGLRenderer
{
public:
  static vtkOpenXRRenderer* New();
  vtkTypeMacro(vtkOpenXRRenderer, vtkOpenGLRenderer);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  using vtkVRRenderer::ResetCamera;

  /**
   * Automatically set up the camera based on a specified bounding box
   * (xmin,xmax, ymin,ymax, zmin,zmax). Camera will reposition itself so
   * that its focal point is the center of the bounding box, and adjust its
   * distance and position to preserve its initial view plane normal
   * (i.e., vector defined from camera position to focal point). Note: if
   * the view plane is parallel to the view up axis, the view up axis will
   * be reset to one of the three coordinate axes.
   */
  void ResetCamera(const double bounds[6]) override;

  using vtkRenderer::ResetCameraClippingRange;

  /**
   * Reset the camera clipping range based on a bounding box.
   */
  void ResetCameraClippingRange(const double bounds[6]) override;

  /**
   * Create a new Camera suitable for use with this type of Renderer.
   */
  VTK_NEWINSTANCE vtkCamera* MakeCamera() override;

  /**
   * Concrete open gl render method.
   */
  void DeviceRender() override;

  /**
   * Show the floor of the VR world
   */
  virtual void SetShowFloor(bool);
  virtual bool GetShowFloor() { return this->ShowFloor; }

protected:
  vtkOpenXRRenderer();
  ~vtkOpenXRRenderer() override = default;

  vtkNew<vtkActor> FloorActor;
  bool ShowFloor = false;

private:
  vtkOpenXRRenderer(const vtkOpenXRRenderer&) = delete;
  void operator=(const vtkOpenXRRenderer&) = delete;
};

#endif
