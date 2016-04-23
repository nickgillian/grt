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
#define GRT_DLL_EXPORTS
#include "SVD.h"

GRT_BEGIN_NAMESPACE
	
bool SVD::solve(MatrixFloat &a){
	
	//Setup the memory
	m = a.getNumRows();
	n = a.getNumCols();
	u = a;
	v.resize(n,n);
	w.resize(n);
	
	eps = std::numeric_limits< Float >::epsilon();
	if( !decompose() ) return false;
	if( !reorder() ) return false;
	
	tsh = 0.5*grt_sqrt(m+n+1.)*w[0]*eps;

	return true;
}

bool SVD::solveVector(VectorFloat &b, VectorFloat &x, Float thresh) {
	UINT i,j,jj;
	Float s;
	if(b.size() != m || x.size() != n){
		return false;
	}
	VectorFloat tmp(n);
	tsh = (thresh >= 0. ? thresh : 0.5*grt_sqrt(m+n+1.)*w[0]*eps);
	for (j=0;j<n;j++) {
		s=0.0;
		if (w[j] > tsh) {
			for (i=0;i<m;i++) s += u[i][j]*b[i];
			s /= w[j];
		}
		tmp[j]=s;
	}
	for (j=0;j<n;j++) {
		s=0.0;
		for (jj=0;jj<n;jj++) s += v[j][jj]*tmp[jj];
		x[j]=s;
	}
	return true;
}

bool SVD::solve(MatrixFloat &b, MatrixFloat &x, Float thresh){
	UINT i,j,m=b.getNumCols();
	if (b.getNumRows() != n || x.getNumRows() != n || b.getNumCols() != x.getNumCols()){
		return false;
	}
	VectorFloat xx(n);
	for (j=0;j<m;j++) {
		for (i=0;i<n;i++) xx[i] = b[i][j];
		solveVector(xx,xx,thresh);
		for (i=0;i<n;i++) x[i][j] = xx[i];
	}
	return true;
}
UINT SVD::rank(Float thresh) {
	UINT j,nr=0;
	tsh = (thresh >= 0. ? thresh : 0.5*grt_sqrt(m+n+1.)*w[0]*eps);
	for (j=0;j<n;j++) if (w[j] > tsh) nr++;
	return nr;
}

UINT SVD::nullity(Float thresh) {
	UINT j,nn=0;
	tsh = (thresh >= 0. ? thresh : 0.5*grt_sqrt(m+n+1.)*w[0]*eps);
	for (j=0;j<n;j++) if (w[j] <= tsh) nn++;
	return nn;
}

MatrixFloat SVD::range(Float thresh){
	UINT i,j,nr=0;
	MatrixFloat rnge(m,rank(thresh));
	for (j=0;j<n;j++) {
		if (w[j] > tsh) {
			for (i=0;i<m;i++) rnge[i][nr] = u[i][j];
			nr++;
		}
	}
	return rnge;
}

MatrixFloat SVD::nullspace(Float thresh){
	UINT j,jj,nn=0;
	MatrixFloat nullsp(n,nullity(thresh));
	for (j=0;j<n;j++) {
		if (w[j] <= tsh) {
			for (jj=0;jj<n;jj++) nullsp[jj][nn] = v[jj][j];
			nn++;
		}
	}
	return nullsp;
}

Float SVD::inv_condition() {
	return (w[0] <= 0. || w[n-1] <= 0.) ? 0. : w[n-1]/w[0];
}

bool SVD::decompose() {
	bool flag;
	int i,its,j,jj,k,l,nm,N,M;
	Float anorm,c,f,g,h,s,scale,x,y,z;
	VectorFloat rv1(n);
	g = scale = anorm = 0.0;
	N = int(n);
	M = int(m);
	l = 0;
	for (i=0;i<N;i++) {
		l=i+2;
		rv1[i]=scale*g;
		g=s=scale=0.0;
		if (i < M) {
			for (k=i;k<M;k++) scale += fabs(u[k][i]);
			if (scale != 0.0) {
				for (k=i;k<M;k++) {
					u[k][i] /= scale;
				    s+= u[k][i]*u[k][i];
				}
				f=u[i][i];
				g = -SIGN(grt_sqrt(s),f);
				h=f*g-s;
				u[i][i]=f-g;
				for (j=l-1;j<N;j++) {
					for (s=0.0,k=i;k<M;k++) s += u[k][i] * u[k][j];
					f=s/h;
					for (k=i;k<M;k++) u[k][j] += f * u[k][i];
				}
				for (k=i;k<M;k++) u[k][i] *= scale; 
			}
		}
		w[i]=scale *g;
		g=s=scale=0.0;
		if (i+1 <= M && i+1 != N) {
			for (k=l-1;k<N;k++) scale += fabs(u[i][k]);
			if (scale != 0.0) {
				for (k=l-1;k<N;k++) {
					u[i][k] /= scale;
					s += u[i][k]*u[i][k];
				}
				f=u[i][l-1];
				g = -SIGN(grt_sqrt(s),f);
				h=f*g-s;
				u[i][l-1]=f-g;
				for (k=l-1;k<N;k++) rv1[k]=u[i][k]/h;
				for (j=l-1;j<M;j++) {
					for (s=0.0,k=l-1;k<N;k++) s += u[j][k]*u[i][k];
					for (k=l-1;k<N;k++) u[j][k] += s*rv1[k];
				}
				for (k=l-1;k<N;k++) u[i][k]*= scale;
			}
		}
		anorm=grt_max(anorm,(fabs(w[i])+fabs(rv1[i])));
	}
	for (i=N-1;i>=0;i--) {
		if (i < N-1) {
			if (g != 0.0) {
				for (j=l;j<N;j++)
					v[j][i]=u[i][j]/u[i][l]/g;
				for (j=l;j<N;j++) {
					for (s=0.0,k=l;k<N;k++) s += u[i][k]*v[k][j];
					for (k=l;k<N;k++) v[k][j] += s*v[k][i];
				}
			}
			for (j=l;j<N;j++) v[i][j]=v[j][i]=0.0;
		}
		v[i][i]=1.0;
		g=rv1[i];
		l=i;
	}
	for (i=grt_min(M,N)-1;i>=0;i--) {
		l=i+1;
		g=w[i];
		for (j=l;j<N;j++) u[i][j]=0.0;
		if (g != 0.0) {
			g=1.0/g;
			for (j=l;j<N;j++) {
				for (s=0.0,k=l;k<M;k++) s += u[k][i]*u[k][j];
				f=(s/u[i][i])*g;
				for (k=i;k<M;k++) u[k][j] += f*u[k][i];
			}
			for (j=i;j<M;j++) u[j][i] *= g;
		} else for (j=i;j<M;j++) u[j][i] =0.0;
		++u[i][i];
	}
	for (k=N-1;k>=0;k--) {
		for (its=0;its<MAX_NUM_SVD_ITER;its++) {
			flag=true;
			for (l=k;l>=0;l--) {
				nm=l-1;
				if (l == 0 || fabs(rv1[l]) <= eps*anorm) {
					flag=false;
					break;
				}
				if (fabs(w[nm]) <= eps*anorm) break;
			}
			if (flag) {
				c=0.0;
				s=1.0;
				for (i=l;i<k+1;i++) {
					f=s*rv1[i];
					rv1[i]=c*rv1[i];
					if (fabs(f) <= eps*anorm) break;
					g=w[i];
					h=pythag(f,g);
					w[i]=h;
					h=1.0/h;
					c=g*h;
					s = -f*h;
					for (j=0;j<M;j++) {
						y=u[j][nm];
						z=u[j][i];
						u[j][nm]=y*c+z*s;
						u[j][i]=z*c-y*s;
					}
				}
			}
			z=w[k];
			if (l == k) {
				if (z < 0.0) {
					w[k] = -z;
					for (j=0;j<N;j++) v[j][k] = -v[j][k];
				}
				break;
			}
			if (its == MAX_NUM_SVD_ITER-1){
				return false;
			}
			x=w[l];
			nm=k-1;
			y=w[nm];
			g=rv1[nm];
			h=rv1[k];
			f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
			g=pythag(f,1.0);
			f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
			c=s=1.0;
			for (j=l;j<=nm;j++) {
				i=j+1;
				g=rv1[i];
				y=w[i];
				h=s*g;
				g=c*g;
				z=pythag(f,h);
				rv1[j]=z;
				c=f/z;
				s=h/z;
				f=x*c+g*s;
				g=g*c-x*s;
				h=y*s;
				y *= c;
				for (jj=0;jj<N;jj++) {
					x=v[jj][j];
					z=v[jj][i];
					v[jj][j]=x*c+z*s;
					v[jj][i]=z*c-x*s;
				}
				z=pythag(f,h);
				w[j]=z;
				if (z) {
					z=1.0/z;
					c=f*z;
					s=h*z;
				}
				f=c*g+s*y;
				x=c*y-s*g;
				for (jj=0;jj<M;jj++) {
					y=u[jj][j];
					z=u[jj][i];
					u[jj][j]=y*c+z*s;
					u[jj][i]=z*c-y*s;
				}
			}
			rv1[l]=0.0;
			rv1[k]=f;
			w[k]=x;
		}
	}
	
	return true;
}

bool SVD::reorder() {
	UINT i,j,k,s,inc=1;
	Float sw;
	VectorFloat su(m);
	VectorFloat sv(n);
	do { inc *= 3; inc++; } while (inc <= n);
	do {
		inc /= 3;
		for (i=inc;i<n;i++) {
			sw = w[i];
			for (k=0;k<m;k++) su[k] = u[k][i];
			for (k=0;k<n;k++) sv[k] = v[k][i];
			j = i;
			while (w[j-inc] < sw) {
				w[j] = w[j-inc];
				for (k=0;k<m;k++) u[k][j] = u[k][j-inc];
				for (k=0;k<n;k++) v[k][j] = v[k][j-inc];
				j -= inc;
				if (j < inc) break;
			}
			w[j] = sw;
			for (k=0;k<m;k++) u[k][j] = su[k];
			for (k=0;k<n;k++) v[k][j] = sv[k];
		}
	} while (inc > 1);
	for (k=0;k<n;k++) {
		s=0;
		for (i=0;i<m;i++) if (u[i][k] < 0.) s++;
		for (j=0;j<n;j++) if (v[j][k] < 0.) s++;
		if (s > (m+n)/2) {
			for (i=0;i<m;i++) u[i][k] = -u[i][k];
			for (j=0;j<n;j++) v[j][k] = -v[j][k];
		}
	}
	return true;
}

Float SVD::pythag(const Float a, const Float b) {
	Float absa=fabs(a);
	Float absb=fabs(b);
	return (absa > absb ? absa*grt_sqrt(1.0+grt_sqr(absb/absa)) : (absb == 0.0 ? 0.0 : absb*grt_sqrt(1.0+grt_sqr(absa/absb))));
}
  
GRT_END_NAMESPACE
