// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkCamera.h"
#include "vtkColorTransferFunction.h"
#include "vtkFiniteDifferenceGradientEstimator.h"
#include "vtkFixedPointVolumeRayCastMapper.h"
#include "vtkImageClip.h"
#include "vtkPiecewiseFunction.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkStructuredPoints.h"
#include "vtkStructuredPointsReader.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"

#include "vtkDebugLeaks.h"
#include "vtkRegressionTestImage.h"
#include "vtkTestUtilities.h"

int TestMinIntensityRendering(int argc, char* argv[])
{

  // Create the renderers, render window, and interactor
  vtkRenderWindow* renWin = vtkRenderWindow::New();
  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);
  vtkRenderer* ren = vtkRenderer::New();
  renWin->AddRenderer(ren);

  // Read the data from a vtk file
  char* fname = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/ironProt.vtk");
  vtkStructuredPointsReader* reader = vtkStructuredPointsReader::New();
  reader->SetFileName(fname);
  reader->Update();
  delete[] fname;

  // Create a transfer function mapping scalar value to opacity
  vtkPiecewiseFunction* oTFun = vtkPiecewiseFunction::New();
  oTFun->AddSegment(0, 1.0, 256, 0.1);

  vtkColorTransferFunction* cTFun = vtkColorTransferFunction::New();
  cTFun->AddRGBPoint(0, 1.0, 1.0, 1.0);
  cTFun->AddRGBPoint(255, 1.0, 1.0, 1.0);

  // Need to crop to actually see minimum intensity
  vtkImageClip* clip = vtkImageClip::New();
  clip->SetInputConnection(reader->GetOutputPort());
  clip->SetOutputWholeExtent(0, 66, 0, 66, 30, 37);
  clip->ClipDataOn();

  vtkVolumeProperty* property = vtkVolumeProperty::New();
  property->SetScalarOpacity(oTFun);
  property->SetColor(cTFun);
  property->SetInterpolationTypeToLinear();

  vtkFixedPointVolumeRayCastMapper* mapper = vtkFixedPointVolumeRayCastMapper::New();
  mapper->SetBlendModeToMinimumIntensity();
  mapper->SetInputConnection(clip->GetOutputPort());

  vtkVolume* volume = vtkVolume::New();
  volume->SetMapper(mapper);
  volume->SetProperty(property);

  ren->AddViewProp(volume);

  renWin->Render();
  int retVal = vtkRegressionTestImageThreshold(renWin, 0.05);

  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    iren->Start();
  }

  volume->Delete();
  mapper->Delete();
  property->Delete();
  clip->Delete();
  cTFun->Delete();
  oTFun->Delete();
  reader->Delete();
  renWin->Delete();
  iren->Delete();
  ren->Delete();

  return !retVal;
}
