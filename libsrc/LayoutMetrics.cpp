/******************************************************************************\

Copyright (c) <2015>, <UNC-CH>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


---------------------------------
|Please send all BUG REPORTS to:  |
|                                 |
|   sungeui@cs.kaist.ac.kr        |
|   sungeui@gmail.com             |
|                                 |
---------------------------------


The authors may be contacted via:

Mail:         Sung-Eui Yoon
              Dept. of Computer Science, E3-1
              KAIST
              291 Daehak-ro(373-1 Guseong-dong), Yuseong-gu
              DaeJeon, 305-701
              Republic of Korea

\*****************************************************************************/


#include "stdio.h"
#include "LayoutMetrics.h"
#include "math.h"
#include "VArray.h"

using namespace OpenCCL;




CMetrics::CMetrics (void)
{
	m_bMetricType = true;
	InitOrdering ();

}



void CMetrics::InitOrdering (void)
{
	//printf ("Init\n");
	//srand (time (NULL));

	m_hpBestOrdering.clear ();
	m_NumAddedOrdering = 0;
	m_BestOrdering = 0;
}

// Add an ordering
// Return value: positive value --> return ordering.
//		 negative value --> error.
int CMetrics::AddOrdering (int NumEdgeLengths, int * EdgeLengths)
{
	int i;

	m_NumAddedOrdering++;

	// check if we need to set the best ordering with an initial ordering.
	if (m_hpBestOrdering.empty ()) {	
		InitOrdering (NumEdgeLengths, EdgeLengths);

		m_BestOrdering = m_NumAddedOrdering;
		return m_NumAddedOrdering;
	}


	// check if added one is better than current better one.
	// If so, consider it as the best one.
	CHyperPlane NewOne = m_hpBestOrdering;	// not to affect the best one
	
	for (i = 0;i < NumEdgeLengths;i++)
	{
		int EL = EdgeLengths [i];

		assert (EL >= 0);

		CHyperPlane::iterator Iter = NewOne.find (EL);
		if (Iter == NewOne.end ()) {
			CHyperPlane::value_type NewMap (EL, 1);		
			NewOne.insert (NewMap);	// add one edge length
		}
		else 
			Iter->second += 1;
	}


	float ExpPF = PerformCOMetric (NewOne);

	if (ExpPF < 0) {
		m_hpBestOrdering.clear ();
		InitOrdering (NumEdgeLengths, EdgeLengths);

		m_BestOrdering = m_NumAddedOrdering;

	}


	return m_NumAddedOrdering;
}

//set a best ordering with the initial one.
void CMetrics::InitOrdering (int NumEdgeLengths, int * EdgeLengths)
{
	int i;

	assert (m_hpBestOrdering.empty ());
	
	for (i = 0;i < NumEdgeLengths;i++)
	{
		int EL = EdgeLengths [i];

		assert (EL >= 0);

		CHyperPlane::iterator Iter = m_hpBestOrdering.find (EL);
		if (Iter == m_hpBestOrdering.end ()) {
			CHyperPlane::value_type NewMap (EL, -1);			
			m_hpBestOrdering.insert (NewMap);	// add one edge length
		}
		else 
			Iter->second += (-1);
	}
}


float CMetrics::PerformCOMetric (CHyperPlane & HyperPlane)
{
	// HyperPlane is already sorted
	float Sum = 0, tempSum = 0;
	int i, Dim = HyperPlane.size ();
	int CoeffSum = 0;

	//printf ("Dim = %d\n", Dim);
	// centroid is (1/n, ... n/n)
	CHyperPlane::iterator Iter = HyperPlane.begin ();
	for (i = 0;i < Dim;i++)
	{
		CoeffSum += Iter->second;
		Sum += (float (Iter->second) * log (float (Iter->first)));	// cache-oblivious metric
		
		Iter++;
	}

	assert (CoeffSum == 0);




	return Sum;
}


int CMetrics::GetBestOrdering (void)
{
	return m_BestOrdering;
}
