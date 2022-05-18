/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHyperTreeGridLocator.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkHyperTreeGridLocator.h"

#include "vtkHyperTreeGrid.h"

//------------------------------------------------------------------------------
vtkHyperTreeGridLocator::vtkHyperTreeGridLocator()
{
  this->HTG = nullptr;
}

//------------------------------------------------------------------------------
vtkHyperTreeGridLocator::~vtkHyperTreeGridLocator()
{
  this->SetHyperTreeGrid(nullptr);
}

//------------------------------------------------------------------------------
void vtkHyperTreeGridLocator::Initialize()
{
  // Do nothing
}

//------------------------------------------------------------------------------
void vtkHyperTreeGridLocator::Update()
{
  if (!this->HTG)
  {
    vtkErrorMacro("HyperTreeGrid not set befor updating.");
    return;
  }
}

//------------------------------------------------------------------------------
void vtkHyperTreeGridLocator::SetHyperTreeGrid(vtkHyperTreeGrid* candHTG)
{
  this->HTG = candHTG;
}

//------------------------------------------------------------------------------
void vtkHyperTreeGridLocator::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << this->GetObjectName() << " acting on:\n";
  if (this->HTG)
  {
    HTG->PrintSelf(os, indent);
  }
  else
  {
    os << indent << "HyperTreeGrid: none\n";
  }
}
