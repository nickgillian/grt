/*
GRT MIT License
Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef GRT_EIGENVALUE_DECOMPOSITION_HEADER
#define GRT_EIGENVALUE_DECOMPOSITION_HEADER

#include "GRTTypedefs.h"
#include "../DataStructures/VectorFloat.h"
#include "../DataStructures/MatrixFloat.h"

GRT_BEGIN_NAMESPACE

class GRT_API EigenvalueDecomposition {
public:
    EigenvalueDecomposition();
    ~EigenvalueDecomposition();
    
    bool decompose(const MatrixFloat &a);
    
    /**
     Returns the eigenvector matrix
     
     @return a MatrixDouble containing the eigenvector matrix
     */
    MatrixFloat getEigenvectors(){ return eigenvectors; }
    
    /**
     Returns the block diagonal eigenvalue matrix
     
     @return a MatrixDouble containing the diagonal eigenvalues
     */
    MatrixFloat getDiagonalEigenvalueMatrix();
    
    /**
     Return the real parts of the eigenvalues
     
     @return a VectorFloat containing the real parts of the eigenvalues
     */
    VectorFloat getRealEigenvalues();
    
    /**
     Return the complex parts of the eigenvalues
     
     @return a VectorFloat containing the complex parts of the eigenvalues
     */
    VectorFloat getComplexEigenvalues();
    
protected:
    /**
     Symmetric Householder reduction to tridiagonal form.

      This is derived from the Algol procedures tred2 by Bowdler, Martin,
       Reinsch, and Wilkinson, Handbook for Auto. Comp., Vol.ii-Linear Algebra,
       and the corresponding Fortran subroutine in EISPACK.
     */
    void tred2();
    
    /**
      Symmetric tridiagonal QL algorithm.
     
      This is derived from the Algol procedures tql2, by Bowdler, Martin,
      Reinsch, and Wilkinson, Handbook for Auto. Comp., Vol.ii-Linear Algebra,
      and the corresponding Fortran subroutine in EISPACK.
     */
    void tql2();
    
    /**
     Nonsymmetric reduction to Hessenberg form.

     This is derived from the Algol procedures orthes and ortran, by Martin
     and Wilkinson, Handbook for Auto. Comp., Vol.ii-Linear Algebra, and the
     corresponding Fortran subroutines in EISPACK.
     */
    void orthes();
    
    /**
     Nonsymmetric reduction from Hessenberg to real Schur form.

     This is derived from the Algol procedure hqr2, by Martin and Wilkinson,
     Handbook for Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
     Fortran subroutine in EISPACK.
     */
    void hqr2();
    
    /**
      Complex scalar division.
     */
    void cdiv(Float xr, Float xi, Float yr, Float yi);

    template< class T >
    inline T findMax(const T &a,const T &b){
        return (a > b ? a : b);
    }
    template< class T >
    inline T findMin(const T &a,const T &b){
        return (a < b ? a : b);
    }
    template< class T >
    inline T hypot(const T &a,const T &b){
        return sqrt( (a*a)+(b*b) );
    }
    
    int n;
    bool issymmetric;
    Float cdivr;
    Float cdivi;
    MatrixFloat eigenvectors;
    MatrixFloat h;
    VectorFloat realEigenvalues;
    VectorFloat complexEigenvalues;
    VectorFloat ort;
    
    WarningLog warningLog;
    
};
    
GRT_END_NAMESPACE

#endif //GRT_EIGENVALUE_DECOMPOSITION_HEADER
