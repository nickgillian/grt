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

#include "MatrixDouble.h"

namespace GRT{

class EigenvalueDecomposition {
public:
    EigenvalueDecomposition();
    ~EigenvalueDecomposition();
    
    bool decompose(const MatrixDouble &a);
    
    /**
     Returns the eigenvector matrix
     
     @return a MatrixDouble containing the eigenvector matrix
     */
    MatrixDouble getEigenvectors(){ return eigenvectors; }
    
    /**
     Returns the block diagonal eigenvalue matrix
     
     @return a MatrixDouble containing the diagonal eigenvalues
     */
    MatrixDouble getDiagonalEigenvalueMatrix();
    
    /**
     Return the real parts of the eigenvalues
     
     @return a VectorDouble containing the real parts of the eigenvalues
     */
    VectorDouble getRealEigenvalues();
    
    /**
     Return the complex parts of the eigenvalues
     
     @return a VectorDouble containing the complex parts of the eigenvalues
     */
    VectorDouble getComplexEigenvalues();
    
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
    void cdiv(double xr, double xi, double yr, double yi);
    
    inline double findMax(const double &a,const double &b){
        return (a > b ? a : b);
    }
    inline double findMin(const double &a,const double &b){
        return (a < b ? a : b);
    }
    inline double hypot(const double &a,const double &b){
        return sqrt( (a*a)+(b*b) );
    }
    
    int n;
    bool issymmetric;
    double cdivr;
    double cdivi;
    MatrixDouble eigenvectors;
    MatrixDouble h;
    VectorDouble realEigenvalues;
    VectorDouble complexEigenvalues;
    VectorDouble ort;
    
    WarningLog warningLog;
    
};
    
} //End of namespace GRT

#endif //GRT_EIGENVALUE_DECOMPOSITION_HEADER
