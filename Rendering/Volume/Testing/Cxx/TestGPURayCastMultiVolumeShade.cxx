// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
/**
 * Mostly based on the TestGPURayCastMultiVolumeCellData, load few volumes for testing the
 * vtkMultiVolume.
 *
 * The purpose of this test is to check the result when we activate the shade property and set
 * some lighting property when the renderer use the default lighting.
 */
#include "vtkCamera.h"
#include "vtkColorTransferFunction.h"
#include "vtkCommand.h"
#include "vtkConeSource.h"
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkImageResample.h"
#include "vtkImageResize.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkMultiVolume.h"
#include "vtkNew.h"
#include "vtkNrrdReader.h"
#include "vtkPNGReader.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPointData.h"
#include "vtkPointDataToCellData.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkTestUtilities.h"
#include "vtkVolume16Reader.h"
#include "vtkVolumeProperty.h"
#include "vtkXMLImageDataReader.h"

#include "vtkAbstractMapper.h"
#include "vtkImageData.h"
#include "vtkOutlineFilter.h"
#include "vtkPolyDataMapper.h"

#include "vtkMath.h"

namespace
{

//------------------------------------------------------------------------------
vtkSmartPointer<vtkImageData> ConvertImageToFloat(vtkDataObject* image)
{
  auto imageIn = vtkImageData::SafeDownCast(image);

  vtkSmartPointer<vtkImageData> imageOut = vtkSmartPointer<vtkImageData>::New();
  imageOut->SetDimensions(imageIn->GetDimensions());
  imageOut->AllocateScalars(VTK_FLOAT, 4);

  auto arrayIn = imageIn->GetPointData()->GetScalars();
  auto arrayOut = imageOut->GetPointData()->GetScalars();
  const auto numTuples = arrayOut->GetNumberOfTuples();

  for (vtkIdType i = 0; i < numTuples; i++)
  {
    double* value = arrayIn->GetTuple(i);

    double valuef[4];
    valuef[0] = value[0] / 255.;
    valuef[1] = value[1] / 255.;
    valuef[2] = value[2] / 255.;
    valuef[3] = value[3] / 255.;
    arrayOut->SetTuple(i, valuef);
  }

  return imageOut;
}

}

//------------------------------------------------------------------------------
int TestGPURayCastMultiVolumeShade(int argc, char* argv[])
{
  // Load data
  vtkNew<vtkVolume16Reader> headReader;
  headReader->SetDataDimensions(64, 64);
  headReader->SetImageRange(1, 93);
  headReader->SetDataByteOrderToLittleEndian();
  char* fname = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/headsq/quarter");
  headReader->SetFilePrefix(fname);
  headReader->SetDataSpacing(3.2, 3.2, 1.5);
  delete[] fname;

  fname = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/tooth.nhdr");
  vtkNew<vtkNrrdReader> toothReader;
  toothReader->SetFileName(fname);
  delete[] fname;

  vtkNew<vtkPNGReader> reader2dtf;
  fname = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/tooth_2dtransf.png");
  reader2dtf->SetFileName(fname);
  reader2dtf->Update();
  delete[] fname;

  // Create and configure some volumes mapper, mostly a copy of the
  // TestGPURayCastMultiVolumeCellData
  vtkNew<vtkImageResize> headmrSource;
  headmrSource->SetInputConnection(headReader->GetOutputPort());
  headmrSource->SetResizeMethodToOutputDimensions();
  headmrSource->SetOutputDimensions(128, 128, 128);

  vtkNew<vtkPointDataToCellData> pointsToCells;
  pointsToCells->SetInputConnection(headmrSource->GetOutputPort());
  pointsToCells->Update();

  vtkNew<vtkColorTransferFunction> ctf;
  ctf->AddRGBPoint(0, 0.0, 0.0, 0.0);
  ctf->AddRGBPoint(500, 0.1, 0.6, 0.3);
  ctf->AddRGBPoint(1000, 0.1, 0.6, 0.3);
  ctf->AddRGBPoint(1150, 1.0, 1.0, 0.9);

  vtkNew<vtkPiecewiseFunction> pf;
  pf->AddPoint(0, 0.00);
  pf->AddPoint(500, 0.15);
  pf->AddPoint(1000, 0.15);
  pf->AddPoint(1150, 0.85);

  vtkNew<vtkPiecewiseFunction> gf;
  gf->AddPoint(0, 0.0);
  gf->AddPoint(90, 0.07);
  gf->AddPoint(100, 0.7);

  vtkNew<vtkVolume> vol;
  vol->GetProperty()->SetScalarOpacity(pf);
  vol->GetProperty()->SetColor(ctf);
  vol->GetProperty()->SetGradientOpacity(gf);
  vol->GetProperty()->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

  vtkNew<vtkVolume> vol1;
  auto tf2d = ::ConvertImageToFloat(reader2dtf->GetOutputDataObject(0));
  vol1->GetProperty()->SetTransferFunctionModeTo2D();
  vol1->GetProperty()->SetTransferFunction2D(tf2d);
  vol1->GetProperty()->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

  vol1->RotateX(180.);
  vol1->RotateZ(90.);
  vol1->SetScale(1.8, 1.8, 1.8);
  vol1->SetPosition(175., 190., 210.);

  // ---------------------------------------------------------
  // Here is what we want to test, activate the shading and
  // set some properties especially the ambient property.
  vol->GetProperty()->ShadeOn();
  vol->GetProperty()->SetAmbient(0.5);
  vol->GetProperty()->SetDiffuse(1.0);
  vol->GetProperty()->SetSpecular(0.9);

  // Same as above it's mostly a copy of the TestGPURayCastMultiVolumeCellData
  vtkNew<vtkMultiVolume> overlappingVol;
  vtkNew<vtkGPUVolumeRayCastMapper> mapper;
  overlappingVol->SetMapper(mapper);

  mapper->SetInputConnection(0, pointsToCells->GetOutputPort());
  overlappingVol->SetVolume(vol, 0);

  mapper->SetInputConnection(3, toothReader->GetOutputPort());
  overlappingVol->SetVolume(vol1, 3);

  mapper->SetUseJittering(1);

  vtkNew<vtkRenderWindow> renWin;
  renWin->SetSize(400, 400);
  renWin->SetMultiSamples(0);

  vtkNew<vtkRenderer> ren;
  renWin->AddRenderer(ren);
  ren->SetBackground(1.0, 1.0, 1.0);

  ren->AddVolume(overlappingVol);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  iren->SetInteractorStyle(style);

  auto cam = ren->GetActiveCamera();
  cam->SetFocalPoint(85.7721, 88.4044, 33.8576);
  cam->SetPosition(-173.392, 611.09, -102.892);
  cam->SetViewUp(0.130638, -0.194997, -0.972065);

  renWin->Render();

  int retVal = vtkTesting::Test(argc, argv, renWin, 90);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    iren->Start();
  }

  return !((retVal == vtkTesting::PASSED) || (retVal == vtkTesting::DO_INTERACTOR));
}
