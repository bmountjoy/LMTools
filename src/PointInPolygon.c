

#include "PointInPolygon.h"


int intersects(double e, double n, double e1, double n1, double e2, double n2)
{
	double  y,	//y intersect 
			x;	//x intersect
	
	double m1 = 1.41421;
		
	/**
	 * Handle case where e1 = e2 and n1 = n2.
	 */
	
	if(e1 == e2 && n1 == n2)
		 return 0;
	
	/**
	 * Line throught (e,n) with slope 1.
	 * y  = x + b1
	 * b1 = n - e
	 * m1 = 1
	 */
	 
	double b1 = n - m1*e;
	 
	 
	/**
	 * Handle case where m2 = infinity
	 * y = e1 + b1
	 * Intersection must be x = e1 = e2
	 */
	
	if(e1 == e2)
	{	 
		y = m1*e1 + b1;
		if(e1 < e  || 
		  ((y < n1 || y >= n2) && (y <= n2 || y > n1)) ){
			return 0;
		}
		//printf("e1 == e2 (%.2lf,%.2lf) yint: %.2lf e1: %.2lf  e2: %.2lf n1: %.2lf n2: %.2lf\n", e, n, y, e1, e2, n1, n2);
		return 1;
	}
	
	/**
	 * Handle case where m2 = 0
	 * Intersection must be at y = n1 = n2
	 * n1 = m1x + b1 ==> x = (n1 - b1)/m1
	 */
	if(n1 == n2)
	{
		x = (n1 - b1)/m1;
		
		if( x < e  ||
		  ((x < e1 || x >= e2) && (x <= e2 || x > e1)) ){
			return 0;
		}
		//printf("n1 == n2 (%.2lf,%.2lf) xint: %.2lf e1: %.2lf  e2: %.2lf n1: %.2lf n2: %.2lf\n", e, n, x, e1, e2, n1, n2);
		return 1;
	}
	
	
	/**
	 * Line through (e1,n1) and (e2,n2).
	 * y = m2x + b2
	 */
	
	double m2 = (n2 - n1)/(e2 - e1);
	double b2 = n1 - m2*e1;
	
	
	/**
	 * If b1 = b2 then intersection point is (0, b1).
	 */
	
	if(b1 == b2)
	{
		if((0 < e) ||
		  ((0 < e1 || 0 >= e2) && (0 <= e2 || 0 > e1)) || 
		  ((b1 < n1 || b1 >= n2) && (b1 <= n2 || b2 > n1)) ){
			return 0;
		}
	  return 1;
	}
	
	/**
	 * Solve for x
	 * x + b1 = m2x + b2 ==> (m2 - 1)/(b1 - b2)
	 */
	
	x = (b2 - b1)/(m1 - m2);
	
	/**
	 * Make sure x is between e1 and e2 and x >= e
	 */
	if( x < e ||
	  ((x < e1 || x >= e2) && (x <= e2 || x > e1)) ){
		return 0;
	}
	
	/**
	 * Solve for y using either equation/
	 */
	
	y = x + b1;
	    
	/**
	 * Make sure y is between y1 and y2
	 */
	 
	if( (y < n1 || y >= n2) && (y <= n2 || y > n1) ){
		return 0;
	}
	
	//printf("*** xint %.2lf yint: %.2lf m: %.2lf e1: %.2lf  e2: %.2lf n1: %.2lf n2: %.2lf\n", x, y, m2, e1, e2, n1, n2);
	return 1;
}

int pointInPolygon(SHPObject * pShpObj, double x, double y)
{
	int j, nVerts, intersectionCount = 0;
	double x1, y1, x2, y2, * xVerts, * yVerts;
	
	if(!pShpObj->dfXMin){
		puts("dfxmin is messed up.");
		printf("nvertices: %d\n", pShpObj->nVertices);
	}
	
	if(x < pShpObj->dfXMin || x > pShpObj->dfXMax || y < pShpObj->dfYMin || y > pShpObj->dfYMax)
		return 0;
	
	
	xVerts = pShpObj->padfX;
	yVerts = pShpObj->padfY;
	nVerts = pShpObj->nVertices;
	
	if(nVerts <= 3)
		printf("-------------------------------nVerts:%d\n", nVerts);
		
	for(j = 0; j < nVerts; j++)
	{
		x1 = xVerts[j];
		y1 = yVerts[j];
		
		if(j == nVerts-1)
		{
			x2 = xVerts[0];
			y2 = yVerts[0];
		}
		else
		{
			x2 = xVerts[j+1];
			y2 = yVerts[j+1];
		}
		
		if( intersects(x, y, x1, y1, x2, y2) )
			intersectionCount++;
	}
	
	if(intersectionCount % 2 == 0)
		return 0;
	
	return 1;
	
}
