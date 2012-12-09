/***************************************************************************
 *   Copyright (C) 2011 by Pere Ràfols Soler                               *
 *   sapista2@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/***************************************************************************
This file contains the filter definitions
****************************************************************************/

#ifndef  FILTER_H
  #define FILTER_H

#include <math.h>

//Constants definitions
#define PI 3.1416
#define  F_NOT_SET 0
#define  F_LPF_ORDER_1 1
#define  F_LPF_ORDER_2 2
#define  F_LPF_ORDER_3 3
#define  F_LPF_ORDER_4 4
#define  F_HPF_ORDER_1 5
#define  F_HPF_ORDER_2 6
#define  F_HPF_ORDER_3 7
#define  F_HPF_ORDER_4 8
#define  F_LOW_SHELF 9
#define  F_HIGH_SHELF 10
#define  F_PEAK 11
#define  F_NOTCH 12


typedef struct
{
  float b0, b1, b2, a1, a2; //Second Order coeficients
  float b1_0, b1_1, b1_2, a1_1, a1_2; //Second Order extra coeficients 
  int filter_order;  //filter order
  float fs; //sample rate
}Filter;

typedef struct
{
  float buf_0;
  float buf_1;
  float buf_2;
  float buf_e0;
  float buf_e1;
  float buf_e2; 
}Buffers;

//Initialize filter instance
Filter *FilterInit(double rate);

//Destroy a filter instance
void FilterClean(Filter *f);

//Clean buffers
void flushBuffers(Buffers *buf);

//Compute filter coeficients
static inline void calcCoefs(Filter *filter, float fGain, float fFreq, float fQ, int iType, float iEnabled) //p2 = GAIN p3 = Q
{  
  if(iEnabled)
  {  
    //Get filter order
    switch(iType)
    {   
      case F_LPF_ORDER_1: case F_HPF_ORDER_1:
      case F_LPF_ORDER_2: case F_HPF_ORDER_2:
      case F_LOW_SHELF: case F_HIGH_SHELF: 
      case F_PEAK: case F_NOTCH:
	filter->filter_order = 0;
      break;
    
      case F_LPF_ORDER_3: case F_HPF_ORDER_3:   
      case F_LPF_ORDER_4: case F_HPF_ORDER_4:
	filter->filter_order = 1;
      break;
    }
    
    
    float w0=2*PI*(fFreq/filter->fs);
    float alpha, A, b0, b1, b2, a0, a1, a2, b1_0, b1_1, b1_2, a1_0, a1_1, a1_2;
    alpha = A = b0 = b1 = b2 = a0 = a1 = a2 = b1_0 = b1_1 = b1_2 = a1_0 = a1_1 = a1_2 = 1.0;

    switch(iType){

      case F_HPF_ORDER_1:
	b0 = 2; //b0
	b1 = -2; //b1
	b2 = 0.0; //b2
	a0 = w0+2; //a0
	a1 = w0-2; //a1
	a2 = 0.0; //a2
      break;

      case F_HPF_ORDER_2:  case F_HPF_ORDER_4:
	alpha = sinf(w0)/(2*fQ);
	b1_0 = b0 = (1 + cosf(w0))/2; //b0
	b1_1 = b1 = -(1 + cosf(w0)); //b1
	b1_2 = b2 = (1 + cosf(w0))/2; //b2
	a1_0 = a0 = 1 + alpha; //a0
	a1_1 = a1 = -2*cosf(w0); //a1
	a1_2 = a2 = 1 - alpha; //a2
      break;

      case F_HPF_ORDER_3: 
	alpha = sinf(w0)/(2*fQ);
	b1_0 = 2; //b0
	b1_1 = -2; //b1
	b1_2 = 0.0; //b2
	a1_0 = w0+2; //a0
	a1_1 = w0-2; //a1
	a1_2 = 0.0; //a2
	b0 = (1 + cosf(w0))/2; //b0
	b1 = -(1 + cosf(w0)); //b1
	b2 = (1 + cosf(w0))/2; //b2
	a0 = 1 + alpha; //a0
	a1 = -2*cosf(w0); //a1
	a2 = 1 - alpha; //a2
      break;

      case F_LPF_ORDER_1: 
	b0 = w0; //b0
	b1 = w0; //b1
	b2 = 0.0; //b2
	a0 = w0+2; //a0
	a1 = w0-2; //a1
	a2 = 0.0; //a2
      break;

      case F_LPF_ORDER_2: case F_LPF_ORDER_4:
	alpha = sinf(w0)/(2*fQ);
	b1_0 = b0 = (1 - cosf(w0))/2; //b0
	b1_1 = b1 = 1 - cosf(w0); //b1
	b1_2 = b2 = (1 - cosf(w0))/2; //b2
	a1_0 = a0 = 1 + alpha; //a0
	a1_1 = a1 = -2*cosf(w0); //a1
	a1_2 = a2 = 1 - alpha; //a2
      break;

      case F_LPF_ORDER_3:
	alpha = sinf(w0)/(2*fQ);
	b1_0 = w0; //b0
	b1_1 = w0; //b1
	b1_2 = 0.0; //b2
	a1_0 = w0+2; //a0
	a1_1 = w0-2; //a1
	a1_2 = 0.0; //a2
	b0 = (1 - cosf(w0))/2; //b0
	b1 = 1 - cosf(w0); //b1
	b2 = (1 - cosf(w0))/2; //b2
	a0 = 1 + alpha; //a0
	a1 = -2*cosf(w0); //a1
	a2 = 1 - alpha; //a2
      break;

      case F_LOW_SHELF:
	A = sqrtf((fGain));
	alpha =sinf(w0)/2 * (1/fQ);
	b0 = A*((A+1)-(A-1)*cosf(w0)+2*sqrtf(A)*alpha); //b0
	b1 = 2*A*((A-1)-(A+1)*cosf(w0)); //b1
	b2 = A*((A+1)-(A-1)*cosf(w0)-2*sqrtf(A)*alpha); //b2
	a0 = (A+1) + (A-1)*cosf(w0) + 2*sqrtf(A)*alpha; //a0
	a1 = -2*((A-1) + (A+1)*cosf(w0)); //a1
	a2 = (A+1) + (A-1)*cosf(w0) - 2*sqrtf(A)*alpha; //a2
      break;

      case F_HIGH_SHELF:
	A = sqrtf((fGain));
	alpha =sinf(w0)/2 * (1/fQ);
	b0 = A*( (A+1) + (A-1)*cosf(w0) + 2*sqrtf(A)*alpha ); //b0
	b1 = -2*A*( (A-1) + (A+1)*cosf(w0)); //b1
	b2 = A*( (A+1) + (A-1)*cosf(w0) - 2*sqrtf(A)*alpha ); //b2
	a0 = (A+1) - (A-1)*cosf(w0) + 2*sqrtf(A)*alpha; //a0
	a1 = 2*( (A-1) - (A+1)*cosf(w0)); //a1
	a2 = (A+1) - (A-1)*cosf(w0) - 2*sqrtf(A)*alpha; //a2
      break;

      case F_PEAK:
	A = sqrtf((fGain));
	//A=powf(10,(fGain/40));
	alpha = sinf(w0)/(2*fQ);

	b0 = 1 + alpha*A; //b0
	b1 =  -2*cosf(w0); //b1
	b2 =   1 - alpha*A; //b2
	a0 =   1 + alpha/A; //a0
	a1 =  -2*cosf(w0); //a1
	a2 =   1 - alpha/A; //a2
      break;

      case F_NOTCH:
	alpha = sinf(w0)/(2*fQ);

	b0 =  1; //b0
	b1 = -2*cosf(w0); //b1
	b2 =  1; //b2
	a0 =  1 + alpha; //a0
	a1 = -2*cosf(w0); //a1
	a2 =  1 - alpha; //a2
      break;
  } //End of switch

    //Normalice coeficients to a0=1
    filter->b0 = b0/a0; //b0
    filter->b1 =b1/a0; //b1
    filter->b2 =b2/a0; //b2
    filter->a1 =a1/a0; //a1
    filter->a2 =a2/a0; //a2
    filter->b1_0 = b1_0/a1_0;
    filter->b1_1 = b1_1/a1_0;
    filter->b1_2 = b1_2/a1_0;
    filter->a1_1 = a1_1/a1_0;
    filter->a1_2 = a1_2/a1_0;
  }
  else
  {
    //Filter is disabled
    filter->b0 = 1.0f;
    filter->b1 = 0.0f;
    filter->b2 = 0.0f;
    filter->a1 = 0.0f;
    filter->a2 = 0.0f;
    filter->b1_0 = 0.0f;
    filter->b1_1 = 0.0f;
    filter->b1_2 = 0.0f;
    filter->a1_1 = 0.0f; 
    filter->a1_2 = 0.0f;
    filter->filter_order = 0;
  }
}


/***************************************************************************
///TODO I ara com faig aixo pq els HPF no fagis soroll en variar freq????
static inline void adjustBuffers(Filter *filter, Buffers *buf)
{
  //Re-calc buffer values only for LPF Filters this is a improved smooth method that only works for LPF
  if( (int)iType == LPF_ORDER_2 || (int)iType == LPF_ORDER_4)
  {
      //Using B
      buf->buf_1 *= (filter->b1 / b1);
      buf->buf_2 *= (filter->b2 / b2);
      buf->buf_e1 *= (filter->b1_1 / b1_1);
      buf->buf_e2 *= (filter->b1_2 / b1_2);
  }
}
*********************************************************************************/

#define DENORMAL_TO_ZERO(x) if (fabs(x) < (10e-40)) x = 0.f;

//Compute filter
static inline  void computeFilter(Filter *filter, Buffers *buf, float *inputSample)
{
  //Process 1, 2 orders
  //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
  buf->buf_0 = *inputSample-filter->a1*buf->buf_1-filter->a2*buf->buf_2;
  //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
  DENORMAL_TO_ZERO(buf->buf_0);
  *inputSample = filter->b0*buf->buf_0 + filter->b1*buf->buf_1+ filter->b2*buf->buf_2;

  buf->buf_2 = buf->buf_1;
  buf->buf_1 = buf->buf_0;
  
  //Process 3,4 orders if applis
  if(filter->filter_order)
  {
      //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
      buf->buf_e0 = *inputSample-filter->a1_1*buf->buf_e1-filter->a1_2*buf->buf_e2;
      //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
      DENORMAL_TO_ZERO(buf->buf_e0);
      *inputSample = filter->b1_0*buf->buf_e0 + filter->b1_1*buf->buf_e1+ filter->b1_2*buf->buf_e2;

      buf->buf_e2 = buf->buf_e1;
      buf->buf_e1 = buf->buf_e0;
  }
}
#endif