/*
 This code is based on EigenvalueDecomposition.java from http://math.nist.gov/javanumerics/jama/
 The license below is from that file.
 */

/*
 * This software is a cooperative product of The MathWorks and the National
 * Institute of Standards and Technology (NIST) which has been released to the
 * public domain. Neither The MathWorks nor NIST assumes any responsibility
 * whatsoever for its use by other parties, and makes no guarantees, expressed
 * or implied, about its quality, reliability, or any other characteristic.

 * EigenvalueDecomposition.java
 * Copyright (C) 1999 The Mathworks and NIST
 *
 */

#define GRT_DLL_EXPORTS
#include "EigenvalueDecomposition.h"

GRT_BEGIN_NAMESPACE
   
EigenvalueDecomposition::EigenvalueDecomposition(){
    warningLog.setKey("[WARNING EigenvalueDecomposition]");
}

EigenvalueDecomposition::~EigenvalueDecomposition(){

}
    
bool EigenvalueDecomposition::decompose(const MatrixFloat &a){
    
    n = a.getNumCols();
    eigenvectors.resize(n,n);
    realEigenvalues.resize(n);
    complexEigenvalues.resize(n);
    
    issymmetric = true;
    for(int j = 0; (j < n) & issymmetric; j++) {
        for(int i = 0; (i < n) & issymmetric; i++) {
            issymmetric = (a[i][j] == a[j][i]);
        }
    }
    
    if (issymmetric) {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                eigenvectors[i][j] = a[i][j];
            }
        }
        
        // Tridiagonalize.
        tred2();
        
        // Diagonalize.
        tql2();
        
    } else {
        h.resize(n,n);
        ort.resize(n);
        
        for(int j = 0; j < n; j++) {
            for(int i = 0; i < n; i++) {
                h[i][j] = a[i][j];
            }
        }
        
        // Reduce to Hessenberg form.
        orthes();
        
        // Reduce Hessenberg to real Schur form.
        hqr2();
    }
    
    return true;
}
    
void EigenvalueDecomposition::tred2(){
    
    for(int j = 0; j < n; j++) {
        realEigenvalues[j] = eigenvectors[n-1][j];
    }
    
    // Householder reduction to tridiagonal form.
    for(int i = n-1; i > 0; i--) {
        
        // Scale to avoid under/overflow.
        Float scale = 0.0;
        Float h = 0.0;
        for (int k = 0; k < i; k++) {
            scale = scale + fabs(realEigenvalues[k]);
        }
        if (scale == 0.0) {
            complexEigenvalues[i] = realEigenvalues[i-1];
            for (int j = 0; j < i; j++) {
                realEigenvalues[j] = eigenvectors[i-1][j];
                eigenvectors[i][j] = 0.0;
                eigenvectors[j][i] = 0.0;
            }
        } else {
            
            // Generate Householder vector.
            for (int k = 0; k < i; k++) {
                realEigenvalues[k] /= scale;
                h += realEigenvalues[k] * realEigenvalues[k];
            }
            Float f = realEigenvalues[i-1];
            Float g = sqrt(h);
            if (f > 0) {
                g = -g;
            }
            complexEigenvalues[i] = scale * g;
            h = h - f * g;
            realEigenvalues[i-1] = f - g;
            for (int j = 0; j < i; j++) {
                complexEigenvalues[j] = 0.0;
            }
            
            // Apply similarity transformation to remaining columns.
            for (int j = 0; j < i; j++) {
                f = realEigenvalues[j];
                eigenvectors[j][i] = f;
                g = complexEigenvalues[j] + eigenvectors[j][j] * f;
                for (int k = j+1; k <= i-1; k++) {
                    g += eigenvectors[k][j] * realEigenvalues[k];
                    complexEigenvalues[k] += eigenvectors[k][j] * f;
                }
                complexEigenvalues[j] = g;
            }
            f = 0.0;
            for (int j = 0; j < i; j++) {
                complexEigenvalues[j] /= h;
                f += complexEigenvalues[j] * realEigenvalues[j];
            }
            Float hh = f / (h + h);
            for (int j = 0; j < i; j++) {
                complexEigenvalues[j] -= hh * realEigenvalues[j];
            }
            for (int j = 0; j < i; j++) {
                f = realEigenvalues[j];
                g = complexEigenvalues[j];
                for (int k = j; k <= i-1; k++) {
                    eigenvectors[k][j] -= (f * complexEigenvalues[k] + g * realEigenvalues[k]);
                }
                realEigenvalues[j] = eigenvectors[i-1][j];
                eigenvectors[i][j] = 0.0;
            }
        }
        realEigenvalues[i] = h;
    }
    
    // Accumulate transformations.
    for(int i = 0; i < n-1; i++) {
        eigenvectors[n-1][i] = eigenvectors[i][i];
        eigenvectors[i][i] = 1.0;
        Float h = realEigenvalues[i+1];
        if (h != 0.0) {
            for (int k = 0; k <= i; k++) {
                realEigenvalues[k] = eigenvectors[k][i+1] / h;
            }
            for (int j = 0; j <= i; j++) {
                Float g = 0.0;
                for (int k = 0; k <= i; k++) {
                    g += eigenvectors[k][i+1] * eigenvectors[k][j];
                }
                for (int k = 0; k <= i; k++) {
                    eigenvectors[k][j] -= g * realEigenvalues[k];
                }
            }
        }
        for(int k = 0; k <= i; k++) {
            eigenvectors[k][i+1] = 0.0;
        }
    }
    for(int j = 0; j < n; j++) {
        realEigenvalues[j] = eigenvectors[n-1][j];
        eigenvectors[n-1][j] = 0.0;
    }
    eigenvectors[n-1][n-1] = 1.0;
    complexEigenvalues[0] = 0.0;
    
    return;
}

void EigenvalueDecomposition::tql2(){
    
    for(int i = 1; i < n; i++) {
        complexEigenvalues[i-1] = complexEigenvalues[i];
    }
    complexEigenvalues[n-1] = 0.0;
    
    Float f = 0.0;
    Float tst1 = 0.0;
    Float eps = pow(2.0,-52.0);
    for (int l = 0; l < n; l++) {
        
        // Find small subdiagonal element
        tst1 = findMax(tst1,fabs(realEigenvalues[l]) + fabs(complexEigenvalues[l]));
        int m = l;
        while (m < n) {
            if(fabs(complexEigenvalues[m]) <= eps*tst1) {
                break;
            }
            m++;
        }
        
        // If m == l, d[l] is an eigenvalue, otherwise, iterate.
        if (m > l) {
            int iter = 0;
            do {
                iter = iter + 1;  // (Could check iteration count here.)
                
                // Compute implicit shift
                Float g = realEigenvalues[l];
                Float p = (realEigenvalues[l+1] - g) / (2.0 * complexEigenvalues[l]);
                Float r = hypot(p,1.0);
                if (p < 0) {
                    r = -r;
                }
                realEigenvalues[l] = complexEigenvalues[l] / (p + r);
                realEigenvalues[l+1] = complexEigenvalues[l] * (p + r);
                Float dl1 = realEigenvalues[l+1];
                Float h = g - realEigenvalues[l];
                for (int i = l+2; i < n; i++) {
                    realEigenvalues[i] -= h;
                }
                f = f + h;
                
                // Implicit QL transformation.
                p = realEigenvalues[m];
                Float c = 1.0;
                Float c2 = c;
                Float c3 = c;
                Float el1 = complexEigenvalues[l+1];
                Float s = 0.0;
                Float s2 = 0.0;
                for (int i = m-1; i >= l; i--) {
                    c3 = c2;
                    c2 = c;
                    s2 = s;
                    g = c * complexEigenvalues[i];
                    h = c * p;
                    r = hypot(p,complexEigenvalues[i]);
                    complexEigenvalues[i+1] = s * r;
                    s = complexEigenvalues[i] / r;
                    c = p / r;
                    p = c * realEigenvalues[i] - s * g;
                    realEigenvalues[i+1] = h + s * (c * g + s * realEigenvalues[i]);
                    
                    // Accumulate transformation.
                    for(int k = 0; k < n; k++) {
                        h = eigenvectors[k][i+1];
                        eigenvectors[k][i+1] = s * eigenvectors[k][i] + c * h;
                        eigenvectors[k][i] = c * eigenvectors[k][i] - s * h;
                    }
                }
                p = -s * s2 * c3 * el1 * complexEigenvalues[l] / dl1;
                complexEigenvalues[l] = s * p;
                realEigenvalues[l] = c * p;
                
                // Check for convergence.
            } while (fabs(complexEigenvalues[l]) > eps*tst1);
        }
        realEigenvalues[l] = realEigenvalues[l] + f;
        complexEigenvalues[l] = 0.0;
    }
    
    // Sort eigenvalues and corresponding vectors.
    for(int i = 0; i < n-1; i++) {
        int k = i;
        Float p = realEigenvalues[i];
        for (int j = i+1; j < n; j++) {
            if(realEigenvalues[j] < p) {
                k = j;
                p = realEigenvalues[j];
            }
        }
        if (k != i) {
            realEigenvalues[k] = realEigenvalues[i];
            realEigenvalues[i] = p;
            for (int j = 0; j < n; j++) {
                p = eigenvectors[j][i];
                eigenvectors[j][i] = eigenvectors[j][k];
                eigenvectors[j][k] = p;
            }
        }
    }
    return;
}

void EigenvalueDecomposition::orthes(){
    
    int low = 0;
    int high = n-1;
    
    for(int m = low+1; m <= high-1; m++) {
        
        // Scale column.
        Float scale = 0.0;
        for (int i = m; i <= high; i++) {
            scale = scale + fabs(h[i][m-1]);
        }
        if (scale != 0.0) {
            
            // Compute Householder transformation.
            Float ht = 0.0;
            for(int i = high; i >= m; i--) {
                ort[i] = h[i][m-1]/scale;
                ht += ort[i] * ort[i];
            }
            Float g = sqrt( ht );
            if (ort[m] > 0) {
                g = -g;
            }
            ht = ht - ort[m] * g;
            ort[m] = ort[m] - g;
            
            // Apply Householder similarity transformation
            // H = (I-u*u'/h)*H*(I-u*u')/h)
            for (int j = m; j < n; j++) {
                Float f = 0.0;
                for (int i = high; i >= m; i--) {
                    f += ort[i]*h[i][j];
                }
                f = f/ht;
                for (int i = m; i <= high; i++) {
                    h[i][j] -= f*ort[i];
                }
            }
            
            for(int i = 0; i <= high; i++) {
                Float f = 0.0;
                for(int j = high; j >= m; j--) {
                    f += ort[j]*h[i][j];
                }
                f = f/ht;
                for (int j = m; j <= high; j++) {
                    h[i][j] -= f*ort[j];
                }
            }
            ort[m] = scale*ort[m];
            h[m][m-1] = scale*g;
        }
    }
    
    // Accumulate transformations (Algol's ortran).
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            eigenvectors[i][j] = (i == j ? 1.0 : 0.0);
        }
    }
    
    for (int m = high-1; m >= low+1; m--) {
        if (h[m][m-1] != 0.0) {
            for (int i = m+1; i <= high; i++) {
                ort[i] = h[i][m-1];
            }
            for (int j = m; j <= high; j++) {
                Float g = 0.0;
                for (int i = m; i <= high; i++) {
                    g += ort[i] * eigenvectors[i][j];
                }
                // Double division avoids possible underflow
                g = (g / ort[m]) / h[m][m-1];
                for (int i = m; i <= high; i++) {
                    eigenvectors[i][j] += g * ort[i];
                }
            }
        }
    }
    return;
}

void EigenvalueDecomposition::hqr2(){
    
    // Initialize
    int nn = this->n;
    int n = nn-1;
    int low = 0;
    int high = nn-1;
    Float eps = pow(2.0,-52.0);
    Float exshift = 0.0;
    Float p=0,q=0,r=0,s=0,z=0,t,w,x,y;
    
    // Store roots isolated by balanc and compute matrix norm
    Float norm = 0.0;
    for(int i = 0; i < nn; i++) {
        if( (i < low) | (i > high) ){
            realEigenvalues[i] = h[i][i];
            complexEigenvalues[i] = 0.0;
        }
        for (int j = findMax(i-1,0); j < nn; j++) {
            norm = norm + fabs(h[i][j]);
        }
    }
    
    // Outer loop over eigenvalue index
    int iter = 0;
    while (n >= low) {
        
        // Look for single small sub-diagonal element
        int l = n;
        while (l > low) {
            s = fabs(h[l-1][l-1]) + fabs(h[l][l]);
            if (s == 0.0) {
                s = norm;
            }
            if(fabs(h[l][l-1]) < eps * s) {
                break;
            }
            l--;
        }
        
        // Check for convergence
        // One root found
        if(l == n) {
            h[n][n] = h[n][n] + exshift;
            realEigenvalues[n] = h[n][n];
            complexEigenvalues[n] = 0.0;
            n--;
            iter = 0;
            
            // Two roots found
        } else if (l == n-1) {
            w = h[n][n-1] * h[n-1][n];
            p = (h[n-1][n-1] - h[n][n]) / 2.0;
            q = p * p + w;
            z = sqrt(fabs(q));
            h[n][n] = h[n][n] + exshift;
            h[n-1][n-1] = h[n-1][n-1] + exshift;
            x = h[n][n];
            
            // Real pair
            if (q >= 0) {
                if (p >= 0) {
                    z = p + z;
                } else {
                    z = p - z;
                }
                realEigenvalues[n-1] = x + z;
                realEigenvalues[n] = realEigenvalues[n-1];
                if (z != 0.0) {
                    realEigenvalues[n] = x - w / z;
                }
                complexEigenvalues[n-1] = 0.0;
                complexEigenvalues[n] = 0.0;
                x = h[n][n-1];
                s = fabs(x) + fabs(z);
                p = x / s;
                q = z / s;
                r = sqrt(p * p+q * q);
                p = p / r;
                q = q / r;
                
                // Row modification
                for(int j = n-1; j < nn; j++) {
                    z = h[n-1][j];
                    h[n-1][j] = q * z + p * h[n][j];
                    h[n][j] = q * h[n][j] - p * z;
                }
                
                // Column modification
                for(int i = 0; i <= n; i++) {
                    z = h[i][n-1];
                    h[i][n-1] = q * z + p * h[i][n];
                    h[i][n] = q * h[i][n] - p * z;
                }
                
                // Accumulate transformations
                for(int i = low; i <= high; i++) {
                    z = eigenvectors[i][n-1];
                    eigenvectors[i][n-1] = q * z + p * eigenvectors[i][n];
                    eigenvectors[i][n] = q * eigenvectors[i][n] - p * z;
                }
                
                // Complex pair
            } else {
                realEigenvalues[n-1] = x + p;
                realEigenvalues[n] = x + p;
                complexEigenvalues[n-1] = z;
                complexEigenvalues[n] = -z;
            }
            n = n - 2;
            iter = 0;
            
            // No convergence yet
        } else {
            
            // Form shift
            x = h[n][n];
            y = 0.0;
            w = 0.0;
            if (l < n) {
                y = h[n-1][n-1];
                w = h[n][n-1] * h[n-1][n];
            }
            
            // Wilkinson's original ad hoc shift
            if (iter == 10) {
                exshift += x;
                for (int i = low; i <= n; i++) {
                    h[i][i] -= x;
                }
                s = fabs(h[n][n-1]) + fabs(h[n-1][n-2]);
                x = y = 0.75 * s;
                w = -0.4375 * s * s;
            }
            
            // MATLAB's new ad hoc shift
            if (iter == 30) {
                s = (y - x) / 2.0;
                s = s * s + w;
                if (s > 0) {
                    s = sqrt(s);
                    if (y < x) {
                        s = -s;
                    }
                    s = x - w / ((y - x) / 2.0 + s);
                    for(int i = low; i <= n; i++) {
                        h[i][i] -= s;
                    }
                    exshift += s;
                    x = y = w = 0.964;
                }
            }
            
            iter = iter + 1;   // (Could check iteration count here.)
            
            // Look for two consecutive small sub-diagonal elements
            int m = n-2;
            while (m >= l) {
                z = h[m][m];
                r = x - z;
                s = y - z;
                p = (r * s - w) / h[m+1][m] + h[m][m+1];
                q = h[m+1][m+1] - z - r - s;
                r = h[m+2][m+1];
                s = fabs(p) + fabs(q) + fabs(r);
                p = p / s;
                q = q / s;
                r = r / s;
                if(m == l){
                    break;
                }
                if(fabs(h[m][m-1]) * (fabs(q) +fabs(r)) <
                    eps * (fabs(p) * (fabs(h[m-1][m-1]) + fabs(z) +
                                          fabs(h[m+1][m+1])))) {
                    break;
                }
                m--;
            }
            
            for(int i = m+2; i <= n; i++) {
                h[i][i-2] = 0.0;
                if (i > m+2) {
                    h[i][i-3] = 0.0;
                }
            }
            
            // Double QR step involving rows l:n and columns m:n
            for(int k = m; k <= n-1; k++) {
                bool notlast = (k != n-1);
                if(k != m) {
                    p = h[k][k-1];
                    q = h[k+1][k-1];
                    r = (notlast ? h[k+2][k-1] : 0.0);
                    x = fabs(p) + fabs(q) + fabs(r);
                    if (x != 0.0) {
                        p = p / x;
                        q = q / x;
                        r = r / x;
                    }
                }
                if(x == 0.0){
                    break;
                }
                s = sqrt(p * p + q * q + r * r);
                if(p < 0){
                    s = -s;
                }
                if(s != 0){
                    if(k != m){
                        h[k][k-1] = -s * x;
                    }else if(l != m){
                        h[k][k-1] = -h[k][k-1];
                    }
                    p = p + s;
                    x = p / s;
                    y = q / s;
                    z = r / s;
                    q = q / p;
                    r = r / p;
                    
                    // Row modification
                    for(int j = k; j < nn; j++) {
                        p = h[k][j] + q * h[k+1][j];
                        if(notlast){
                            p = p + r * h[k+2][j];
                            h[k+2][j] = h[k+2][j] - p * z;
                        }
                        h[k][j] = h[k][j] - p * x;
                        h[k+1][j] = h[k+1][j] - p * y;
                    }
                    
                    // Column modification
                    for (int i = 0; i <= findMin(n,k+3); i++) {
                        p = x * h[i][k] + y * h[i][k+1];
                        if(notlast){
                            p = p + z * h[i][k+2];
                            h[i][k+2] = h[i][k+2] - p * r;
                        }
                        h[i][k] = h[i][k] - p;
                        h[i][k+1] = h[i][k+1] - p * q;
                    }
                    
                    // Accumulate transformations
                    for(int i = low; i <= high; i++) {
                        p = x * eigenvectors[i][k] + y * eigenvectors[i][k+1];
                        if(notlast){
                            p = p + z * eigenvectors[i][k+2];
                            eigenvectors[i][k+2] = eigenvectors[i][k+2] - p * r;
                        }
                        eigenvectors[i][k] = eigenvectors[i][k] - p;
                        eigenvectors[i][k+1] = eigenvectors[i][k+1] - p * q;
                    }
                }  // (s != 0)
            }  // k loop
        }  // check convergence
    }  // while (n >= low)
    
    // Backsubstitute to find vectors of upper triangular form
    if(norm == 0.0){
        return;
    }
    
    for(n = nn-1; n >= 0; n--) {
        p = realEigenvalues[n];
        q = complexEigenvalues[n];
        
        // Real vector
        if (q == 0) {
            int l = n;
            h[n][n] = 1.0;
            for(int i = n-1; i >= 0; i--) {
                w = h[i][i] - p;
                r = 0.0;
                for(int j = l; j <= n; j++) {
                    r = r + h[i][j] * h[j][n];
                }
                if(complexEigenvalues[i] < 0.0) {
                    z = w;
                    s = r;
                } else {
                    l = i;
                    if (complexEigenvalues[i] == 0.0) {
                        if (w != 0.0) {
                            h[i][n] = -r / w;
                        } else {
                            h[i][n] = -r / (eps * norm);
                        }
                        
                        // Solve real equations
                    } else {
                        x = h[i][i+1];
                        y = h[i+1][i];
                        q = (realEigenvalues[i] - p) * (realEigenvalues[i] - p) + complexEigenvalues[i] * complexEigenvalues[i];
                        t = (x * s - z * r) / q;
                        h[i][n] = t;
                        if(fabs(x) > fabs(z)) {
                            h[i+1][n] = (-r - w * t) / x;
                        } else {
                            h[i+1][n] = (-s - y * t) / z;
                        }
                    }
                    
                    // Overflow control
                    t = fabs(h[i][n]);
                    if ((eps * t) * t > 1) {
                        for(int j = i; j <= n; j++) {
                            h[j][n] = h[j][n] / t;
                        }
                    }
                }
            }
            
            // Complex vector
        } else if (q < 0) {
            int l = n-1;
            
            // Last vector component imaginary so matrix is triangular
            if (fabs(h[n][n-1]) > fabs(h[n-1][n])) {
                h[n-1][n-1] = q / h[n][n-1];
                h[n-1][n] = -(h[n][n] - p) / h[n][n-1];
            } else {
                cdiv(0.0,-h[n-1][n],h[n-1][n-1]-p,q);
                h[n-1][n-1] = cdivr;
                h[n-1][n] = cdivi;
            }
            h[n][n-1] = 0.0;
            h[n][n] = 1.0;
            for(int i = n-2; i >= 0; i--) {
                Float ra,sa,vr,vi;
                ra = 0.0;
                sa = 0.0;
                for (int j = l; j <= n; j++) {
                    ra = ra + h[i][j] * h[j][n-1];
                    sa = sa + h[i][j] * h[j][n];
                }
                w = h[i][i] - p;
                
                if(complexEigenvalues[i] < 0.0) {
                    z = w;
                    r = ra;
                    s = sa;
                } else {
                    l = i;
                    if(complexEigenvalues[i] == 0) {
                        cdiv(-ra,-sa,w,q);
                        h[i][n-1] = cdivr;
                        h[i][n] = cdivi;
                    } else {
                        
                        // Solve complex equations
                        x = h[i][i+1];
                        y = h[i+1][i];
                        vr = (realEigenvalues[i] - p) * (realEigenvalues[i] - p) + complexEigenvalues[i] * complexEigenvalues[i] - q * q;
                        vi = (realEigenvalues[i] - p) * 2.0 * q;
                        if((vr == 0.0) & (vi == 0.0)){
                            vr = eps * norm * (fabs(w) + fabs(q) + fabs(x) + fabs(y) + fabs(z));
                        }
                        cdiv(x*r-z*ra+q*sa,x*s-z*sa-q*ra,vr,vi);
                        h[i][n-1] = cdivr;
                        h[i][n] = cdivi;
                        if (fabs(x) > (fabs(z) + fabs(q))) {
                            h[i+1][n-1] = (-ra - w * h[i][n-1] + q * h[i][n]) / x;
                            h[i+1][n] = (-sa - w * h[i][n] - q * h[i][n-1]) / x;
                        } else {
                            cdiv(-r-y*h[i][n-1],-s-y*h[i][n],z,q);
                            h[i+1][n-1] = cdivr;
                            h[i+1][n] = cdivi;
                        }
                    }
                    
                    // Overflow control
                    t = findMax(fabs(h[i][n-1]),fabs(h[i][n]));
                    if ((eps * t) * t > 1) {
                        for(int j = i; j <= n; j++) {
                            h[j][n-1] = h[j][n-1] / t;
                            h[j][n] = h[j][n] / t;
                        }
                    }
                }
            }
        }
    }
    
    // Vectors of isolated roots
    for (int i = 0; i < nn; i++) {
        if((i < low) | (i > high)){
            for (int j = i; j < nn; j++) {
                eigenvectors[i][j] = h[i][j];
            }
        }
    }
    
    // Back transformation to get eigenvectors of original matrix
    for (int j = nn-1; j >= low; j--) {
        for (int i = low; i <= high; i++) {
            z = 0.0;
            for (int k = low; k <= findMin(j,high); k++) {
                z = z + eigenvectors[i][k] * h[k][j];
            }
            eigenvectors[i][j] = z;
        }
    }
    
    return;
}
    
void EigenvalueDecomposition::cdiv(Float xr, Float xi, Float yr, Float yi){
    Float r,d;
    if(fabs(yr) > fabs(yi)){
        r = yi/yr;
        d = yr + r*yi;
        cdivr = (xr + r*xi)/d;
        cdivi = (xi - r*xr)/d;
    } else {
        r = yr/yi;
        d = yi + r*yr;
        cdivr = (r*xr + xi)/d;
        cdivi = (r*xi - xr)/d;
    }
    return;
}
    
MatrixFloat EigenvalueDecomposition::getDiagonalEigenvalueMatrix(){
    
    MatrixFloat x(n,n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            x[i][j] = 0.0;
        }
        x[i][i] = realEigenvalues[i];
        if(complexEigenvalues[i] > 0) {
            x[i][i+1] = complexEigenvalues[i];
        } else if(complexEigenvalues[i] < 0) {
            x[i][i-1] = complexEigenvalues[i];
        }
    }
    return x;
}

VectorFloat EigenvalueDecomposition::getRealEigenvalues(){
    return realEigenvalues;
}
    
VectorFloat EigenvalueDecomposition::getComplexEigenvalues(){
    return complexEigenvalues;
}
    
GRT_END_NAMESPACE
