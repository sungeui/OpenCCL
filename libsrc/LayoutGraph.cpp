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


#include "LayoutGraph.h"
#include "LayoutMetrics.h"
#include "metis/Metis.h"
#include <stack>
#include <queue>



#ifdef WIN32
// to link metis library
//#pragma comment(lib,"../../Lib/libmetisWinVC.lib")
#pragma comment(lib,"../../Lib/metisWinVC2013.lib")
#endif

namespace OpenCCL
{

CMetrics g_COMetric;


#ifndef USE_STL_Hashmap
SimpleIDHasher g_Hasher;
IDEqualCmp g_EqComp;
#endif


CLayoutVertex::CLayoutVertex (void)
#ifndef USE_STL_Hashmap
: m_Neighbor (2, g_Hasher, g_EqComp)
#endif
{




//	m_c = NULL_IDX;

	m_VWeight = 1;		//	watch out that it is not zero
	m_pNext = m_pPrev = NULL;

	m_ID = 0;

	m_NumChild = 0;
	m_bRefined = 0;
	m_Depth = 0;
	m_pParent = NULL;
	m_Order = 0;
}



CLayoutGraph::CLayoutGraph (int NumVertex)
{
	m_Vertex = NULL;

	CLayoutVertex * pUStartHead = new CLayoutVertex;
	CLayoutVertex * pUEndHead = new CLayoutVertex;

	m_VList.InitList (pUStartHead, pUEndHead);

	m_NumUnit = 0;
	m_MapOID2CID = NULL;
	m_Vertex = NULL;
	m_Unit = NULL;

	m_Vertex = new CLayoutVertex [NumVertex];

	m_NumVertex = NumVertex;
}

CLayoutGraph::~CLayoutGraph (void)
{

	if (m_Vertex != NULL)
		delete [] m_Vertex;

	if (m_Unit != NULL)
		delete [] m_Unit;

	if (m_MapOID2CID != NULL)
		delete [] m_MapOID2CID;

}


// this is used during ordering, not specifying input graph
bool CLayoutVertex::InsertNeighbor (int Neighbor)
{
	int Weight = 1;	// dummy

	CIntHashMap::iterator Iter = m_Neighbor.find (Neighbor);


	if (Iter == m_Neighbor.end ()) {
#ifdef USE_STL_Hashmap
		CIntHashMap::value_type NewEdge (Neighbor, Weight);	// use dummy map
		m_Neighbor.insert (NewEdge);
#else
		m_Neighbor.insert (Neighbor, Weight);		
#endif
	} 
	else {
		Iter->second += Weight;
	}

	return true;
}


bool CLayoutVertex::AddNeighbor (int Neighbor, int Weight)
{
	CIntHashMap::iterator Iter = m_Neighbor.find (Neighbor);
	
	if (Iter == m_Neighbor.end ()) {
#ifdef USE_STL_Hashmap
		CIntHashMap::value_type NewEdge (Neighbor, Weight);	// use dummy map
		m_Neighbor.insert (NewEdge);
#else
		m_Neighbor.insert (Neighbor, Weight);		
#endif
	} 
	else {
		Iter->second += Weight;
	}

	return true;
}


// Note: permit duplicate
void CLayoutGraph::AddEdge (int v1, int v2)
{
	//int Weight;

	if (v1 == v2)
		return;

	m_Vertex [v1].AddNeighbor (v2, 1);
	m_Vertex [v2].AddNeighbor (v1, 1);


}


void CLayoutGraph::ComputeOrdering (int * OrderArray)
{
	int i;
	int NumTotalUnit = m_NumVertex * 2; 
	m_MapOID2CID = new int [NumTotalUnit];
	m_Unit = new CLayoutVertex [NumTotalUnit];
	m_NumUnit = 1;
	m_pRootUnit = & m_Unit [0];

	assert (OrderArray != NULL);

	// initialize for root node
	for (i = 0;i < m_NumVertex;i++)
		m_pRootUnit->m_Vertices.Append (i);

	m_pRootUnit->m_ID = 0;
	m_pRootUnit->m_Depth = 1;
	m_pRootUnit->m_VWeight = m_NumVertex;


	// refine order level by level
	//Progression prog ("Partition", nrBoxes * g_NumSubdivision / (g_NumSubdivision - 1) - 1 , 20);
	queue <CLayoutVertex *> Queue;
	Queue.push (m_pRootUnit);

	/*
	   Stopwatch T_P ("Partition");
	   T_P.Start ();
	 */


	while (!Queue.empty ()) {       
		CLayoutVertex * pNode = Queue.front ();
		Queue.pop ();
		//			prog.step();

		PartitionUnits (pNode, Queue);

		// delete pNode;
		// Note: needs to clean this but it was crease by array.
	}
	/*
	   T_P.Stop ();
	   cout << T_P << endl;
	 */

	assert (m_NumUnit < NumTotalUnit);


	m_VList.InitIteration ();
	while (!m_VList.IsEnd ()) {
		CLayoutVertex * pVertex = m_VList.GetCurrent ();
		assert (pVertex != NULL);

		m_VList.Advance ();
	}	




	/*
	   Stopwatch T_N ("Neighboring");
	   T_N.Start ();
	 */

	ConstructNeighbor (m_pRootUnit);

	/*
	   T_N.Stop ();
	   cout << T_N << endl;
	 */

	/*
	   Stopwatch T_C ("Ordering");
	   T_C.Start ();
	 */


	m_VList.Add (m_pRootUnit);
	Queue.push (m_pRootUnit);
	while (!Queue.empty ()) {  
		CLayoutVertex * pNode = Queue.front ();
		Queue.pop ();

		ComputeOrdering (pNode, Queue);
	}

	/*
	   T_C.Stop ();
	   cout << T_C << endl;
	 */







	assert (m_VList.Size () == m_NumVertex);


	m_VList.InitIteration ();
	while (!m_VList.IsEnd ()) {
		CLayoutVertex * pVertex = m_VList.GetCurrent ();
		assert (pVertex != NULL);

		m_VList.Advance ();
	}	



	m_VList.InitIteration ();
	int Order = 0;
	while (!m_VList.IsEnd ()) {
		CLayoutVertex * pVertex = m_VList.GetCurrent ();


		assert (pVertex != NULL);
		assert (pVertex->m_Vertices.Size () == 1);
		assert (pVertex->m_NumChild == 0);

		int V_ID = pVertex->m_Vertices [0];

		OrderArray [Order] = V_ID;




		m_VList.Advance ();
		Order++;
	}

	assert (Order == m_NumVertex);


	// memory clean
	if (m_Vertex != NULL) {
		delete [] m_Vertex;
		m_Vertex = NULL;
	}

	if (m_Unit != NULL) {
		delete [] m_Unit;
		m_Unit = NULL;
	}

	if (m_MapOID2CID != NULL) {
		delete [] m_MapOID2CID;
		m_MapOID2CID = NULL;
	}



}

//NOTE: during interation of hash_map, please do not insert or delete a node from the map

void CLayoutGraph::ConstructNeighbor (CLayoutVertex * pRoot)
{
	//int NumLeft, NumRight, i;
	int i;
	int MaxDepth, _MaxDepth;
	MaxDepth = 1;

	// access level-by-level from leaf clusters.
	// put all the leaf clusters in the queue, then, access them by decrasing depth
	// priority queue may be more expensive in this case.

	queue <CLayoutVertex *> Queue, LeafQ;
	Queue.push (pRoot);

	while (!Queue.empty ()) 
	{
		CLayoutVertex * pBV = Queue.front ();
		Queue.pop ();

		pBV->m_bRefined = 0;	// to skip duplicate process
		pBV->m_pNext = NULL;
		pBV->m_pPrev = NULL;

		if (pBV->m_NumChild != 0) {
			for (i = 0;i < pBV->m_NumChild;i++)
				Queue.push (pBV->m_pChild [i]);
			continue;
		}

		LeafQ.push (pBV);

		if (MaxDepth < pBV->m_Depth)
			MaxDepth = pBV->m_Depth;

		// LevelNeighborCluster has OriginalID.
		// modify it into ClusterID to easily access cluster hierarchy
		VArray <int> CIDArray;

		assert (pBV->m_Vertices.Size () == 1);
		int VID = pBV->m_Vertices [0];

		CIntHashMap::iterator Iter = m_Vertex [VID].m_Neighbor.begin();
		for(;Iter != m_Vertex [VID].m_Neighbor.end ();Iter++)
		{
			//int OID = * Iter;
			int OID = Iter->first;
			int CID = m_MapOID2CID [OID];
			assert (CID >= 0);

			CIDArray.Append (CID);

		}

		for (i = 0;i < CIDArray.Size ();i++)
			pBV->InsertNeighbor (CIDArray [i]);
		//pBV->m_Neighbor.insert (CIDArray [i]);

		//pBV->m_NumSubClusters = 1;

	}

	_MaxDepth = MaxDepth;	// _MaxDepth is fixed real max depth


	// make sure first element of LeafQ is node of max depth
	while (!LeafQ.empty ()) 
	{
		CLayoutVertex * pBV = LeafQ.front ();

		if (pBV->m_Depth == MaxDepth)
			break;

		LeafQ.pop ();
		LeafQ.push (pBV);
	}

	// make level neighbors
	while (!LeafQ.empty ()) 
	{
		int MaxIter = LeafQ.size ();

		// process only maximum clusters.
		// to stop, we use MaxIter as a threshold. after processing clusters as threshold,
		// we should be done.
		while (!LeafQ.empty () && MaxIter-- > 0) 
		{
			CLayoutVertex * pBV = LeafQ.front ();
			LeafQ.pop ();

			assert (pBV->m_Depth <= MaxDepth);

			if (pBV->m_Depth == MaxDepth && pBV->m_bRefined == 0) {
				// the node is current max and isn't refined
				if (pBV->m_NumChild == 0) {
					if (pBV->m_Depth == _MaxDepth) {		
						// this is real leaf node
						if (pBV->m_pParent != NULL) {
							if (pBV->m_Depth == MaxDepth)
								LeafQ.push (pBV->m_pParent);
							else
								LeafQ.push (pBV);		// process level-by-level
						}

						continue;
					}
					else {
						// this is node on current depth level that we need to process.
						//assert (pBV->m_NumChild != 0);

						// it refers other clusters whose depth may be much higher than its depth
						VArray <int> DeleteID, AddID;
						CIntHashMap::iterator Iter = pBV->m_Neighbor.begin();


						for(;Iter != pBV->m_Neighbor.end ();Iter++)
						{
							//int NeighborID = * Iter;
							int NeighborID = Iter->first;
							CLayoutVertex * pNeighbor = & m_Unit [NeighborID];

							// compute a neighbor whose depth is lower or eq. than current node
							while (pNeighbor->m_Depth > pBV->m_Depth) {
								pNeighbor = pNeighbor->m_pParent;
								assert (pNeighbor != NULL);
							}

							//assert (pNeighbor->m_Depth == pBV->m_Depth);
							if (pNeighbor->m_Depth == pBV->m_Depth) {
								DeleteID.Append (NeighborID);

								AddID.Append (pNeighbor->m_ID);				
								//pNeighbor->m_Neighbor.insert (pBV->m_ID);
								//pNeighbor->InsertNeighbor (pBV->m_ID);
							}
							else 
								assert (pNeighbor->m_NumChild == 0);
							//assert (pNeighbor->m_pLeft == NULL);
						}


						for (i = 0;i < DeleteID.Size ();i++) 
						{
#ifdef USE_STL_Hashmap
							pBV->m_Neighbor.erase (DeleteID [i]);
#else
							pBV->m_Neighbor.Delete (DeleteID [i]);
#endif
						}	

						for (i = 0;i < AddID.Size ();i++)
						{
							int NeighborID = AddID [i];
							pBV->InsertNeighbor (NeighborID);
						//pBV->m_Neighbor.insert (AddID [i]);

							CLayoutVertex * pNeighbor = & m_Unit [NeighborID];
							pNeighbor->InsertNeighbor (pBV->m_ID);
						}

#ifdef USE_STL_Hashmap
						pBV->m_Neighbor.erase (pBV->m_ID); // delete self.
#else
						pBV->m_Neighbor.Delete (pBV->m_ID); // delete self.

#endif
						/*
						// check reciprocity
						Iter = pCluster->m_Neighbor.begin();
						for(;Iter != pCluster->m_Neighbor.end ();Iter++)
						{
						int Neighbor = * Iter;
						CCluster * pNeighbor = & m_pEMesh->m_pArrCluster [Neighbor];

						// check reciprocal relationship
						if (pNeighbor->m_Neighbor.find (pCluster->m_ClusterID) == 
						pNeighbor->m_Neighbor.end ()) {
						printf ("Something is wrong in Updating Level neighbor\n");
						exit (-1);
						}			
						}
						 */
						if (pBV->m_pParent != NULL) {
							if (pBV->m_Depth == MaxDepth)
								LeafQ.push (pBV->m_pParent);
							else
								LeafQ.push (pBV);		// process level-by-level
						}

						continue;
					}
				}



				/*
				   CIntHashMap::iterator Iter = pBV->m_pLeft->m_Neighbor.begin();
				   for(;Iter != pBV->m_pLeft->m_Neighbor.end ();Iter++)
				   pBV->m_Neighbor.insert (* Iter);

				// copy right neighbor into parent
				Iter = pBV->m_pRight->m_Neighbor.begin();
				for(;Iter != pBV->m_pRight->m_Neighbor.end ();Iter++) 
				pBV->m_Neighbor.insert (* Iter);


				 */
				for (i = 0;i < pBV->m_NumChild;i++)
				{
					CIntHashMap::iterator Iter = pBV->m_pChild [i]->m_Neighbor.begin();
					for(;Iter != pBV->m_pChild [i]->m_Neighbor.end ();Iter++)
						pBV->InsertNeighbor (Iter->first);
					//pBV->m_Neighbor.insert (* Iter);
				}




				// modify neighbor's id to indicate same level.

				VArray <int> DeleteID, AddID;
				CIntHashMap::iterator Iter = pBV->m_Neighbor.begin();



				//printf ("Size Neighbor = %d\n", pBV->m_Neighbor.size ());

				for(;Iter != pBV->m_Neighbor.end ();Iter++)
				{
					//int NeighborID = * Iter;
					int NeighborID = Iter->first;
					CLayoutVertex * pNeighbor = & m_Unit [NeighborID];

					if (pNeighbor->m_Depth == pBV->m_Depth) {
						//pNeighbor->m_Neighbor.insert (pBV->m_ID);
						if (pNeighbor != pBV)	
							pNeighbor->InsertNeighbor (pBV->m_ID);
						continue;
					}
					else if (pNeighbor->m_Depth == pBV->m_Depth + 1) {
						DeleteID.Append (NeighborID);
						assert (pNeighbor->m_pParent != NULL);

						AddID.Append (pNeighbor->m_pParent->m_ID);


						// update reciprocity of neighbors
						//CCluster * pNeighbor = & m_pEMesh->m_pArrCluster [NeighborID];
						//pNeighbor->m_Neighbor.erase (pCluster->m_ClusterID);

						//pNeighbor = & m_Unit [pNeighbor->m_pParent->m_ID];
						//pNeighbor->InsertNeighbor (pBV->m_ID);


					}
					else if (pNeighbor->m_Depth < pBV->m_Depth) {
						// just keep it.
						// continue;
					}
					else {
						// pNeighbor->m_Depth > pCluster->m_Depth + 1
						printf ("Error! Wrong level neighbor%d %d\n", pNeighbor->m_Depth, pBV->m_Depth);
						printf ("please report this bug with input data. (sungeui@cs.unc.edu)\n");
						exit (-1);
					}

				}

				for (i = 0;i < DeleteID.Size ();i++)
				{
#ifdef USE_STL_Hashmap
					pBV->m_Neighbor.erase (DeleteID [i]);
#else
					pBV->m_Neighbor.Delete (DeleteID [i]);
#endif
				}

				for (i = 0;i < AddID.Size ();i++) 
				{
					int NeighborID = AddID [i];

					pBV->InsertNeighbor (NeighborID);

					CLayoutVertex * pNeighbor = & m_Unit [NeighborID];
					pNeighbor->InsertNeighbor (pBV->m_ID);
				}



#ifdef USE_STL_Hashmap
				pBV->m_Neighbor.erase (pBV->m_ID); // delete self.
#else
				pBV->m_Neighbor.Delete (pBV->m_ID); // delete self.
#endif



				// update 
				/*
				   pBV->m_NumSubClusters = pBV->m_pLeft->m_NumSubClusters + 
				   pBV->m_pRight->m_NumSubClusters + 1;
				 */

				pBV->m_bRefined = 1;
			}

			if (pBV->m_pParent != NULL) {
				if (pBV->m_Depth == MaxDepth)
					LeafQ.push (pBV->m_pParent);
				else
					LeafQ.push (pBV);		// process level-by-level
			}
		}


		MaxDepth--;



	}

	//return pRoot->m_NumSubClusters;

}


void CLayoutGraph::ComputeOrdering (CLayoutVertex * pNode, 
		queue <CLayoutVertex *> & Queue)
{



	int i;
	// compute optimal order based on a metric
	float Cost = 0, MinCost = float (1e15);
	//int BestOrder = -1, BestOrderWithJump = -1;
	VArray <VArray <int> > Combination;
	bool IsJump;

	if (pNode->m_NumChild == 0)
		return;

	g_COMetric.InitOrdering ();


	//printf ("Compute Ordering of %d clusters-------\n", FinalSubdivision);
	GetCombination (pNode->m_NumChild, Combination);


	for (i = 0;i < Combination.Size ();i++)
		Cost = GetCost (Combination [i], pNode, IsJump);


	int BestOrdering = g_COMetric.GetBestOrdering ();


	VArray <int> & ChosenOrder = Combination [BestOrdering - 1];
	CLayoutVertex * pPivot = pNode->m_pNext;
	int PreviousID = pNode->m_Order;
	for (i = 0;i < ChosenOrder.Size ();i++)
	{
		m_VList.AddBefore (pPivot, pNode->m_pChild [ChosenOrder [i]]);
		pNode->m_pChild [ChosenOrder [i]]->m_Order = PreviousID;
		PreviousID += pNode->m_pChild [ChosenOrder [i]]->m_VWeight;
	}
	m_VList.Delete (pNode);
	assert (PreviousID == pNode->m_Order + pNode->m_VWeight);



	// update temporary list for easy cost computation
	// refine level by level
	for (i = 0;i < ChosenOrder.Size ();i++)
		Queue.push (pNode->m_pChild [ChosenOrder [i]]);
}




const int MIN_EDGE_WGT  = 1;
int nr;
int * vweights;
int * WgtEdge;
int *xadj, *adjncy;
int g_NumEdge = 0;
int *metisBoxes;

void CLayoutGraph::PartitionUnits (CLayoutVertex * pNode, queue <CLayoutVertex *> & Queue)
{
	int ContainBox = -1;

	// compute graph
	int i, MaxSize = 0, NumVertex = pNode->m_Vertices.Size ();

#ifdef USE_STL_Hashmap
	CIntHashMap Hash;
#else
	CIntHashMap Hash (NumVertex, g_Hasher, g_EqComp);
#endif


	//printf ("Before Part. %d are in the Node\n", pNode->m_Vertices.Size ());

	if (pNode->m_Vertices.Size () == 1) {    // it becomes leaf cluster
		m_MapOID2CID [pNode->m_Vertices [0]] = pNode->m_ID;
		return;
	}

	if (pNode->m_Vertices.Size () == 0) {   // it becomes leaf cluster
		printf ("No cluster in this Unit\n");
		return;
	}


	for (i = 0;i < NumVertex;i++)
	{
		int UnitID = pNode->m_Vertices [i];

#ifdef USE_STL_Hashmap
		CIntHashMap::value_type NewMap (UnitID, i);   // mapping between real id and array id
		Hash.insert (NewMap);
#else
		Hash.insert (UnitID, i);		// mapping between real id and array id
#endif

		MaxSize += m_Vertex [UnitID].m_Neighbor.size ();
	}


	vweights = new int [NumVertex];
	xadj = new int [NumVertex + 1];
	adjncy = new int [MaxSize];
	WgtEdge = new int [MaxSize];
	metisBoxes = new int [NumVertex];
	int EdgeIndex = 0;
	int AvgClusterSize = 0;


	for (i = 0;i < NumVertex;i++)
	{
		int UnitID = pNode->m_Vertices [i];
		CLayoutVertex & Unit = m_Vertex [UnitID];

		vweights [i] = Unit.m_VWeight;
		xadj [i] = EdgeIndex;
		AvgClusterSize += 1;

		//printf ("New cluster = %d\n", UnitID);
		CIntHashMap::iterator Iter = Unit.m_Neighbor.begin ();
		while (Iter != Unit.m_Neighbor.end ())
		{
			int DestUnit = Iter->first;

			// check if neighbor cluster are in the our interest
			// Note:: we don't consider edges between other clusters when we partition.
			// But, we consider when we order.
			CIntHashMap::iterator DomainIter = Hash.find (DestUnit);
			if (DomainIter == Hash.end ()) {
				Iter++;
				continue;
			}

			adjncy [EdgeIndex] = DomainIter->second;        // id in the array
			WgtEdge [EdgeIndex++] = Iter->second ;
			Iter++;
		}
	}

	xadj [i] = EdgeIndex;
	assert (EdgeIndex <= MaxSize);
	AvgClusterSize /= g_NumSubdivision;

	//printf ("Avg Cluster Size = %d\n", AvgClusterSize);

	// call metis
	static int options[5] = { 0, 3, 1, 3, 0 };
	int edgeCut = 0;
	int wgtflag = 3;
	int numflag = 0;


	if (NumVertex > g_NumSubdivision) {
		//printf ("Run metis\n");
		//printf ("Input cluster = %d\n", NumVertex);

		METIS_PartGraphRecursive (&NumVertex, xadj, adjncy, vweights, WgtEdge, &wgtflag,
				&numflag, (int *) &g_NumSubdivision , options, &edgeCut, metisBoxes);
		//METIS_PartGraphVKway (&NumVertex, xadj, adjncy, vweights, WgtEdge, &wgtflag,
		//		&numflag, &g_NumSubdivision , options, &edgeCut, metisBoxes);
		//printf ("Total Weight of Edge = %d, Num of edge cut = %d, possible tri = %d (per cluster)\n", Sum, edgeCut, (edgeCut/2/ 2)  / g_NumSubdivision);
	}
	else {
		for (i = 0;i < NumVertex;i++)
			metisBoxes [i] = i;
	}


	// postprocessing to improve connectedness
	int FinalSubdivision = PostprocessingMetis (NumVertex, g_NumSubdivision, AvgClusterSize);


	if (FinalSubdivision < 2) {
		printf ("After postprocessing of Metis, the number of clusters is less than 2\n");
		exit (-1);
	}


	//printf ("Partitioned %d Units into %d clusters\n", NumCluster, FinalSubdivision);

	// update partitioniong

	int PivotUnit = m_NumUnit;
	pNode->m_NumChild = FinalSubdivision;
	for (i = 0;i < FinalSubdivision;i++)
	{
		pNode->m_pChild [i] = & m_Unit [m_NumUnit];
		pNode->m_pChild [i]->m_ID = m_NumUnit;
		pNode->m_pChild [i]->m_Depth = pNode->m_Depth + 1;
		pNode->m_pChild [i]->m_pParent = pNode;
		pNode->m_pChild [i]->m_VWeight = 0;

		m_NumUnit++;
	}

	//CLayoutVertex * pUnits = new CLayoutVertex [FinalSubdivision];
	for (i = 0;i < NumVertex;i++)
	{
		assert (metisBoxes [i] < FinalSubdivision);
		int VID = pNode->m_Vertices [i];
		m_Unit [PivotUnit + metisBoxes [i]].m_Vertices.Append (VID);
		m_Unit [PivotUnit + metisBoxes [i]].m_VWeight += m_Vertex [VID].m_VWeight;

	}


	//printf ("Pivot cluster - %d\n", PivotUnit);



	pNode->m_Vertices.Clear (true);


	delete [] vweights;
	delete [] xadj;
	delete [] adjncy;
	delete [] WgtEdge;
	delete [] metisBoxes;



	for (i = 0;i < FinalSubdivision;i++) {
		Queue.push (& m_Unit [PivotUnit + i]);
	}	



}


// Note:leave at least two component
//bool g_2Com = true;
int g_CurNumClusters = 0; //
int CLayoutGraph::PostprocessingMetis (int SrcNumCluster, int DestNumCluster, int AvgClusterSize)
{
	int i, j, k;
	int NumDeletedCluster = 0;
	VArray <VArray <int> > Clusters;        // partitioned clusters

	g_CurNumClusters = DestNumCluster;		// the number of current clusters

	for (i = 0;i < DestNumCluster;i++)
	{
		VArray <int> Ele;
		Clusters.Append (Ele);
	}

	for (i = 0;i < SrcNumCluster;i++)
		Clusters [metisBoxes [i]].Append (i);

	// Note: we do not create new cluster now.
	// just merge sub-component into given cluster
	VArray <VArray <int> > SubCom;
	for (i = 0;i < DestNumCluster;i++)
	{
		//printf ("Original cells = %d, Num of Components = %d\n", Clusters [i].Size (), SubCom.Size ());
		// compute connected component
		if (GetSubComponent (Clusters [i], SubCom)) {
			//printf ("Original cells = %d, Num of Components = %d\n", Clusters [i].Size (), SubCom.Size ());

			int NumCom = SubCom.Size ();
			for (j = 0;j < NumCom;j++)
			{
				// try to assign neighboring clusters
				if (AssignSubComponent (Clusters [i], SubCom [j],
							Clusters, AvgClusterSize, true)) {
					VArray <int> & DelCom = SubCom [j];
					for (k = 0;k < DelCom.Size ();k++)
					{
						int Cluster = Clusters [i].IndexOf (DelCom [k]);
						assert (Cluster != -1);
						Clusters [i].Remove (Cluster);
					}
				}

			}
			if (Clusters [i].Size () == 0) {
				// remove this cluster.
				//printf ("Remove current cluster\n");
				for (k = 0;k < SrcNumCluster;k++)
					if (metisBoxes [k] >= i)
						metisBoxes [k]--;

				Clusters.Remove (i);
				DestNumCluster--;
				i--;
				NumDeletedCluster++;
				g_CurNumClusters--;
			}
		}
	}

	return DestNumCluster;
}

bool CLayoutGraph::GetSubComponent (VArray <int> & Cluster, VArray <VArray <int> > & Components)
{
	Components.Clear (false);
	int NumCom = 0;
	VArray <int> Comp;
	int MaxCluster = Cluster.Size ();
	bool * Flag = new bool [MaxCluster];
	int i, j;
	stack <int> Stack;

	for (i = 0;i < MaxCluster;i++)
                Flag [i] = false;


        for (i = 0;i < MaxCluster;i++)
        {
                if (Flag [i] == true)
                        continue;


                Comp.Clear (false);

                Flag [i] = true;
                Stack.push (Cluster [i]);
                Comp.Append (Cluster [i]);

                while (!Stack.empty ()) {
                        int C = Stack.top ();
                        Stack.pop ();

                        for (j = xadj [C];j < xadj [C + 1];j++)
                        {
                                int AdjCluster = adjncy [j];

                                if (WgtEdge [j] > MIN_EDGE_WGT) {
                                        int WhichCluster = Cluster.IndexOf (AdjCluster);
                                        if (WhichCluster == -1)
                                                continue;

                                        if (Flag [WhichCluster] == false) {
                                                Flag [WhichCluster] = true;
                                                Stack.push (AdjCluster);
                                                Comp.Append (AdjCluster);
                                        }
                                }
                        }
                }

                Components.Append (Comp);
        }       // end of for , find another component

        delete [] Flag;

        if (Components.Size () == 1)
                return false;

        return true;
}
bool CLayoutGraph::AssignSubComponent (VArray <int> & ParentSubCom, VArray <int> & SubCom, 
				   VArray <VArray <int> > & Clusters, int AvgClusterSize, bool ClusterGranul)
{
        // if there is neighboring cluster of this component, assign it to the cluster.
        int i, j, AssignCluster = -1, MaxEdge = 0;

#ifdef USE_STL_Hashmap
        CIntHashMap Hash;
#else
		CIntHashMap Hash (6, g_Hasher, g_EqComp);
#endif

        int TotalVertices = 0;

        if (ClusterGranul == true)
                TotalVertices = SubCom.Size ();
        else {
                for (i = 0;i < SubCom.Size ();i++)
                        TotalVertices += vweights [SubCom [i]];
        }

        if (TotalVertices >= AvgClusterSize *0.5)
                return false;

        //printf ("TotalVertices %d, Avg %d\n", TotalVertices, AvgClusterSize);

        for (i = 0;i < SubCom.Size ();i++)
        {

                int C = SubCom [i];

                for (j = xadj [C];j < xadj [C + 1];j++)
                {
                        int AdjCluster = adjncy [j];
                        int FinalCluster = metisBoxes [AdjCluster];

                        if (WgtEdge [j] <= MIN_EDGE_WGT ||
                            SubCom.IndexOf (AdjCluster) != -1)
                               continue;        // not in our interest.

                        CIntHashMap::iterator Iter = Hash.find (FinalCluster);
                        if (Iter == Hash.end ()) {
#ifdef USE_STL_Hashmap
                                CIntHashMap::value_type NewMap (FinalCluster, WgtEdge [j]);
                                Hash.insert (NewMap);
#else
				Hash.insert (FinalCluster, WgtEdge [j]);
#endif

                                if (WgtEdge [j] > MaxEdge) {
                                        MaxEdge = WgtEdge [j];
                                        AssignCluster = FinalCluster;
                                }
                        }
						else {
                                Iter->second += WgtEdge [j];
                                if (Iter->second > MaxEdge) {
                                        MaxEdge = Iter->second;
                                        AssignCluster = FinalCluster;
                                }
                        }
                }
        }       // end of for

        if (MaxEdge > 0) {

				// avoid resulting cluster is less than 2
				if (g_CurNumClusters == 2 &&
					ParentSubCom.Size () == SubCom.Size ())
					return false;

                // assign
                for (i = 0;i < SubCom.Size ();i++)
                {
                        Clusters [AssignCluster].Append (SubCom [i]);
                        metisBoxes [SubCom [i]] = AssignCluster;
                }

                //printf ("Assign SubCom into %d (Sharing triangles = %d)\n", AssignCluster, MaxEdge/2/2);
                return true;
        }

        return false;
}


void CLayoutGraph::GetCombination (int NumUnit, VArray <VArray <int> > & Combination, int Which)
{
        static VArray <int> Order;
        int i, j;

        if (Which == NumUnit) {
                Combination.Append (Order);

                /*
                printf ("Order: ");
                for (i = 0;i < Order.Size ();i++)
                        printf ("%d, ", Order [i]);
                printf ("\n");
                */
                return;
        }

        if (Which == 0) {               // initialize
                Order.Clear (false);
                Order.SetCount (NumUnit);
        }

        bool Flag;
        for (i = 0;i < NumUnit;i++)
        {
                Flag = true;
                for (j = 0;j < Which;j++)
                        if (Order [j] == i) {
                               Flag = false;
                               break;
                        }

                if (Flag == true) {
                        Order [Which] = i;
                        GetCombination (NumUnit, Combination, Which + 1);
                }
        }
}

float CLayoutGraph::GetCost (VArray <int> & Order, CLayoutVertex * pParent, 
		bool & Jump)
{
	float Cost = 0;
	int Delta;
	int i;
	CLayoutVertex * pPivot;
	CLayoutVertex ** pChilds = & pParent->m_pChild [0];

	// -- start --- check CMetrics class
	VArray <int> EdgeLengths;
	// --  end  --- check CMetrics class


	// update temporary list for easy cost computation
	int PreviousID = pParent->m_Order;
	pPivot = pParent->m_pNext;
	for (i = 0;i < Order.Size ();i++)
	{
		m_VList.AddBefore (pPivot, pChilds [Order [i]]);
		pChilds [Order [i]]->m_Order = PreviousID;
		PreviousID += pChilds [Order [i]]->m_VWeight;
	}
	pPivot = pParent->m_pPrev;
	m_VList.Delete (pParent);

	assert (PreviousID == pParent->m_Order + pParent->m_VWeight);



	//printf ("Jump = %d\n", Jump);
	// compute cost: each edge is counted two times for simple coding.
	for (i = 0;i < Order.Size ();i++)
	{
		CLayoutVertex & Unit = * pChilds [Order [i]];
		CIntHashMap::iterator Iter = Unit.m_Neighbor.begin ();
		while (Iter != Unit.m_Neighbor.end ())
		{
			int DestUnit = Iter->first;

			// NOTE: there is some bug in computing neighboring	
			int Weight = Iter->second;
			Weight = 1;

			//bool IsOutside = IsOut (DestUnit, pUnits, Order.Size ());
			bool IsOutside = IsOut (DestUnit, pChilds, Order.Size ());
			Delta = IsOutside ? (2) : (1);  // if outside, we have to consider the reverse dir. now.


			// get number of vertices between this edge
			float EdgeLength = GetCost (pChilds [Order [i]], DestUnit);
			//float SubCost = (Iter->second * EdgeLength);



			// -- start --- check CMetrics class
			for (int k = 0;k < Delta * Weight;k++)
				EdgeLengths.Append (int (EdgeLength));

			Iter++;
		}


	}       // end of for , each order

	// restore list
	for (i = 0;i < Order.Size ();i++)
		m_VList.Delete (pChilds [i]);
	m_VList.AddNext (pPivot, pParent);




	// -- start --- check CMetrics class
	g_COMetric.AddOrdering (EdgeLengths.Size (), EdgeLengths.GetArray ());
	// --  end  --- check CMetrics class

	return 0;
}

bool CLayoutGraph::IsOut (int SrcID, CLayoutVertex ** pChild, int NumChild)
{
	int i;
	for (i = 0;i < NumChild;i++)
		if (SrcID == pChild [i]->m_ID)
			return false;

	return true;
}


// get number of vertices between them
// NOTE: currently, it's very in-efficient
float CLayoutGraph::GetCost (CLayoutVertex * pPivot, int DestUnit)
{

	float NumVertices = 0;
	CLayoutVertex * pNode = pPivot;

	CLayoutVertex * pDest = & m_Unit [DestUnit];

	assert (pDest->m_pNext != NULL ||
			pDest->m_pParent->m_pNext != NULL);

	int DestOrder = pDest->m_Order;
	if (pDest->m_pNext == NULL)
		DestOrder = pDest->m_pParent->m_Order;

	int IdxGap = pPivot->m_Order - DestOrder;
	IdxGap = abs (IdxGap);
	return CostFunc (IdxGap, true); 

}
// this is kind of sqrt (IdxGap) function
// It is better to avoid tight fitting given a machine
// Note:: currently, 256vertices require 4K, which is page table.
float CLayoutGraph::CostFunc (int IdxGap, bool ClusterLevel)
{
	assert (IdxGap >= 0);

	//if (m_bCacheOblivious) // return just an edge length
	return float (IdxGap);

}








} // end of name space
