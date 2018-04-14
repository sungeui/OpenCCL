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

// include header
#include "OpenCCL.h"

// enable name space
using namespace OpenCCL;


int main (int argc, char * argv [])
{
	printf (" A sample mesh: 	\n");
	printf (" 0	--	1 	\n");
	printf (" |	\\	|	\n");	
	printf (" 2	__	3	\n");	

	// Specify the number of vertex
	const int NumVertex = 5;	
	int Order [NumVertex];

	CLayoutGraph Graph (NumVertex);

	// make edges between vertices
	Graph.AddEdge (0, 1);
	Graph.AddEdge (0, 2);
	Graph.AddEdge (0, 3);
	
	Graph.AddEdge (1, 3);
	Graph.AddEdge (2, 3);

	Graph.AddEdge (1, 4);
	Graph.AddEdge (3, 4);

	// Get ordering of the vertices
	Graph.ComputeOrdering (Order);


	printf ("Computed vertex order:\n");
	int i;
	for (i = 0;i < NumVertex;i++)
		printf ("ID of %dth position in the computed order is %d\n", i, Order [i]);	

	return 1;
}



