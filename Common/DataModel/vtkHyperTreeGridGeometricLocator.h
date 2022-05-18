/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHyperTreeGridGeoemtricLocator.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkHyperTreeGridGeometricLocator
 * @brief class that implements accelerated searches through HyperTree Grids (HTGs) using geometric
 * information
 *
 * The goal of this class is to implement a geometric locator search through the HTG structure. Its
 * main feature should be to expose a generic interface to finding the HTG cells that contain a
 * given geometric object. The search through the HTG is implemented using a
 * vtkHyperTreeGridNonOrientedGeometricCursor. The arborescent structure of the HTG should be
 * sufficient to accelerate the search and achieve good performance in general.
 *
 * All methods in this class should be thread safe since it is meantto be used in a multithreaded
 * environment out of the box.
 *
 * @sa
 * vtkHyperTreeGridLocator, vtkHyperTreeGrid, vtkHyperTree, vtkHyperTreeGridOrientedCursor,
 * vtkHyperTreeGridNonOrientedCursor
 */

#ifndef vtkHyperTreeGridGeometricLocator_h
#define vtkHyperTreeGridGeometricLocator_h

#include "vtkCommonDataModelModule.h" //For export macro
#include "vtkHyperTreeGridLocator.h"

class vtkGenericCell;

class vtkHyperTreeGridGeometricLocator : public vtkHyperTreeGridLocator
{
public:
  ///@{
  /**
   * Standard type methods.
   */
  vtkTypeMacro(vtkHyperTreeGridGeometricLocator, vtkHyperTreeGridLocator);
  ///@}

  ///@{
  /**
   * Construct a new default locator
   */
  static vtkHyperTreeGridGeometricLocator* New();
  ///@}

  ///@{
  /**
   * Basic search for cell holding a given point
   * @param point coordinated of sought point
   * @return the global index of the cell holding the point (-1 if cell not found)
   */
  vtkIdType Search(const double point[3]) override;

  /**
   * Find the cell where a given point lies
   * @param[in] point an array holding the coordinates of the point to search for
   * @param[in] tol tolerance level
   * @param[out] cell pointer to a cell configured with information from return value cell index
   * @param[out] subId
   * @param[out] pcoords parametric coordinates of the point in the cell
   * @param[out] weights interpolation weights of the sought point in the cell
   * @return the global index of the cell holding the point (-1 if no cell is found)
   */
  vtkIdType FindCell(const double point[3], const double tol, vtkGenericCell* cell, int& subId,
    double pcoords[3], double* weights) override;

  /**
   * Find first intersection of the line defined by (p0, p1) with the HTG
   * @param[in] p0 first point of the line
   * @param[in] p1 second point of the line
   * @param[in] tol tolerance level
   * @param[out] t pseudo-time along line path at intersection
   * @param[out] x intersection point
   * @param[out] pcoords parametric coordinatesof intersection
   * @param[out] subId
   * @param[out] cellId the global index of the intersected cell
   * @param[out] cell pointer to a vtkCell object corresponding to cellId
   * @return an integer with 0 if no intersection could be found
   */
  int IntersectWithLine(const double p0[3], const double p[2], const double tol, double& t,
    double x[3], double pcoords[3], int& subId, vtkIdType& cellId, vtkGenericCell* cell) override;
  ///@}

protected:
  vtkHyperTreeGridGeometricLocator() = default;
  ~vtkHyperTreeGridGeometricLocator() = default;

private:
  vtkHyperTreeGridGeometricLocator(const vtkHyperTreeGridGeometricLocator&) = delete;
  void operator=(const vtkHyperTreeGridGeometricLocator&) = delete;

  /**
   * Helper method for determining if a point is held within an extent
   */
  bool isInExtent(const double pt[3], const double extent[6]);

}; // vtkHyperTreeGridGeometricLocator

#endif // vtkHyperTreeGridGeometricLocator_h
