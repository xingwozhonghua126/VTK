/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOBJWriter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkOBJWriter.h"

#include "vtkDataSet.h"
#include "vtkErrorCode.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkNumberToString.h"
#include "vtkPNGWriter.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkTriangleStrip.h"

#include "vtksys/FStream.hxx"
#include "vtksys/SystemTools.hxx"

namespace
{
//------------------------------------------------------------------------------
void WriteFaces(std::ostream& f, vtkCellArray* faces, bool withNormals, bool withTCoords)
{
  vtkIdType npts;
  const vtkIdType* indx;
  for (faces->InitTraversal(); faces->GetNextCell(npts, indx);)
  {
    f << "f";
    for (vtkIdType i = 0; i < npts; i++)
    {
      f << " " << indx[i] + 1;
      if (withTCoords)
      {
        f << "/" << indx[i] + 1;
        if (withNormals)
        {
          f << "/" << indx[i] + 1;
        }
      }
      else if (withNormals)
      {
        f << "//" << indx[i] + 1;
      }
    }
    f << "\n";
  }
}

//------------------------------------------------------------------------------
void WriteLines(std::ostream& f, vtkCellArray* lines)
{
  vtkIdType npts;
  const vtkIdType* indx;
  for (lines->InitTraversal(); lines->GetNextCell(npts, indx);)
  {
    f << "l";
    for (vtkIdType i = 0; i < npts; i++)
    {
      f << " " << indx[i] + 1;
    }
    f << "\n";
  }
}

//------------------------------------------------------------------------------
void WritePoints(std::ostream& f, vtkPoints* pts, vtkDataArray* normals, vtkDataArray* tcoords)
{
  vtkNumberToString convert;
  vtkIdType nbPts = pts->GetNumberOfPoints();

  // Positions
  for (vtkIdType i = 0; i < nbPts; i++)
  {
    double p[3];
    pts->GetPoint(i, p);
    f << "v " << convert(p[0]) << " " << convert(p[1]) << " " << convert(p[2]) << "\n";
  }

  // Normals
  if (normals)
  {
    for (vtkIdType i = 0; i < nbPts; i++)
    {
      double p[3];
      normals->GetTuple(i, p);
      f << "vn " << convert(p[0]) << " " << convert(p[1]) << " " << convert(p[2]) << "\n";
    }
  }

  // Textures
  if (tcoords)
  {
    for (vtkIdType i = 0; i < nbPts; i++)
    {
      double p[2];
      tcoords->GetTuple(i, p);
      f << "vt " << convert(p[0]) << " " << convert(p[1]) << "\n";
    }
  }
}

//----------------------------------------------------------------------------
bool WriteMtl(std::ostream& f, const std::string& baseName, const char* textureFileName)
{
  std::string mtlFileName = baseName + ".mtl";
  vtksys::ofstream fmtl(mtlFileName.c_str(), vtksys::ofstream::out);
  if (fmtl.fail())
  {
    return false;
  }

  // remove directories
  mtlFileName = vtksys::SystemTools::GetFilenameName(mtlFileName);

  // set material
  std::string mtlName = vtksys::SystemTools::GetFilenameName(baseName);
  fmtl << "newmtl " << mtlName << "\n";
  fmtl << "map_Kd " << textureFileName << "\n";
  return true;
}
}

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkOBJWriter);

//------------------------------------------------------------------------------
vtkOBJWriter::vtkOBJWriter()
{
  this->FileName = nullptr;
  this->TextureFileName = nullptr;
  this->SetNumberOfInputPorts(2);
}

//------------------------------------------------------------------------------
vtkOBJWriter::~vtkOBJWriter()
{
  this->SetFileName(nullptr);
  this->SetTextureFileName(nullptr);
}

//------------------------------------------------------------------------------
void vtkOBJWriter::WriteData()
{
  vtkPolyData* input = this->GetInputGeometry();
  vtkImageData* texture = this->GetInputTexture();

  if (input == nullptr)
  {
    vtkErrorMacro("No geometry to write!");
    this->SetErrorCode(vtkErrorCode::UnknownError);
    return;
  }

  vtkPoints* pts = input->GetPoints();
  vtkCellArray* polys = input->GetPolys();
  vtkCellArray* strips = input->GetStrips();
  vtkCellArray* lines = input->GetLines();
  vtkDataArray* normals = input->GetPointData()->GetNormals();
  vtkDataArray* tcoords = input->GetPointData()->GetTCoords();

  if (pts == nullptr)
  {
    vtkErrorMacro("No data to write!");
    this->SetErrorCode(vtkErrorCode::UnknownError);
    return;
  }

  if (this->FileName == nullptr)
  {
    vtkErrorMacro("Please specify FileName to write");
    this->SetErrorCode(vtkErrorCode::NoFileNameError);
    return;
  }

  vtkIdType npts = 0;

  vtksys::ofstream f(this->FileName, vtksys::ofstream::out);
  if (f.fail())
  {
    vtkErrorMacro("Unable to open file: " << this->FileName);
    this->SetErrorCode(vtkErrorCode::CannotOpenFileError);
    return;
  }

  // Write header
  f << "# Generated by Visualization Toolkit\n";

  if (texture && this->TextureFileName)
  {
    // resolve conflict
    vtkWarningMacro("Both a vtkImageData on port 1 and the TextureFileName are set. "
                    "Using TextureFileName.");
    texture = nullptr;
  }

  std::vector<std::string> comp;
  vtksys::SystemTools::SplitPath(vtksys::SystemTools::GetFilenamePath(this->FileName), comp);
  comp.push_back(vtksys::SystemTools::GetFilenameWithoutLastExtension(this->FileName));
  std::string baseName = vtksys::SystemTools::JoinPath(comp);
  if (texture || this->TextureFileName)
  {
    std::string textureFileName = texture ? baseName + ".png" : this->TextureFileName;
    if (!::WriteMtl(f, baseName, textureFileName.c_str()))
    {
      vtkErrorMacro("Unable to create material file");
    }
    if (texture)
    {
      vtkNew<vtkPNGWriter> pngWriter;
      pngWriter->SetInputData(texture);
      pngWriter->SetFileName(textureFileName.c_str());
      pngWriter->Write();
    }
    // write mtllib line
    std::string mtlFileName = baseName + ".mtl";
    std::string mtlName = vtksys::SystemTools::GetFilenameName(mtlFileName);
    f << "mtllib " + mtlName + "\n";
  }

  // Write points
  ::WritePoints(f, pts, normals, tcoords);

  // Decompose any triangle strips into triangles
  vtkNew<vtkCellArray> polyStrips;
  if (strips->GetNumberOfCells() > 0)
  {
    const vtkIdType* ptIds = nullptr;
    for (strips->InitTraversal(); strips->GetNextCell(npts, ptIds);)
    {
      vtkTriangleStrip::DecomposeStrip(npts, ptIds, polyStrips);
    }
  }

  // Write material if a texture is specified
  if (texture || this->TextureFileName)
  {
    // declare material in obj file
    std::string mtlName = vtksys::SystemTools::GetFilenameName(baseName);
    f << "usemtl " << mtlName << "\n";
  }

  // Write triangle strips
  ::WriteFaces(f, polyStrips, normals != nullptr, tcoords != nullptr);

  // Write polygons.
  if (polys)
  {
    ::WriteFaces(f, polys, normals != nullptr, tcoords != nullptr);
  }

  // Write lines.
  if (lines)
  {
    ::WriteLines(f, lines);
  }

  f.close();
}

//------------------------------------------------------------------------------
void vtkOBJWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "FileName: " << (this->GetFileName() ? this->GetFileName() : "(none)") << endl;
  os << indent << "Input: " << this->GetInputGeometry() << endl;

  vtkImageData* texture = this->GetInputTexture();
  if (texture)
  {
    os << indent << "Texture:" << endl;
    texture->PrintSelf(os, indent.GetNextIndent());
  }
}

//------------------------------------------------------------------------------
vtkPolyData* vtkOBJWriter::GetInputGeometry()
{
  return vtkPolyData::SafeDownCast(this->GetInput(0));
}

//------------------------------------------------------------------------------
vtkImageData* vtkOBJWriter::GetInputTexture()
{
  return vtkImageData::SafeDownCast(this->GetInput(1));
}

//------------------------------------------------------------------------------
vtkDataSet* vtkOBJWriter::GetInput(int port)
{
  return vtkDataSet::SafeDownCast(this->Superclass::GetInput(port));
}

//------------------------------------------------------------------------------
int vtkOBJWriter::FillInputPortInformation(int port, vtkInformation* info)
{
  if (port == 0)
  {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
    return 1;
  }
  if (port == 1)
  {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
    return 1;
  }
  return 0;
}
