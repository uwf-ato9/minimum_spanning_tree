#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define PushArray(Count, type) ((type *)calloc((Count), sizeof(type)))

static uint32_t FindSet(uint32_t *Parents, uint32_t VertexIndex)
{
    uint32_t Result = VertexIndex;
    while(Result != Parents[VertexIndex])
    {
        Result = Parents[VertexIndex];
    }
    return Result;
}

static void Union(uint32_t *Parents, uint32_t UIndex, uint32_t VIndex)
{
    uint32_t UParentIndex = FindSet(Parents, UIndex);
    uint32_t VParentIndex = FindSet(Parents, VIndex);
    if(UParentIndex != VParentIndex)
    {
        Parents[VParentIndex] = UParentIndex;
    }
}

struct edge
{
    uint32_t UIndex;
    uint32_t VIndex;
    uint64_t Weight;
};

static void InsertionSort(uintptr_t EdgeCount, edge *Edges)
{
    for(uintptr_t EdgeIndex = 1;
        EdgeIndex < EdgeCount;
        ++EdgeIndex)
    {
        edge Edge = Edges[EdgeIndex];
        intptr_t PrevEdgeIndex = (intptr_t)EdgeIndex - 1;
        while((PrevEdgeIndex >= 0) &&
              (Edges[PrevEdgeIndex].Weight > Edge.Weight))
        {
            Edges[PrevEdgeIndex + 1] = Edges[PrevEdgeIndex];
            --PrevEdgeIndex;
        }

        Edges[PrevEdgeIndex] = Edge;
    }
}

static uintptr_t MST(uintptr_t EdgeCount, edge *Edges, uintptr_t VertexCount)
{
    edge *TempEdges = PushArray(EdgeCount, edge);
    uintptr_t TempEdgeCount = 0;

    // NOTE: Disjoint-Set (Union-Find)
    uint32_t *Parents = PushArray(VertexCount, uint32_t);

    // NOTE: All vertices are initially their own ancestors
    for(uintptr_t VertexIndex = 0;
        VertexIndex < VertexCount;
        ++VertexIndex)
    {
        Parents[VertexIndex] = VertexIndex;
    }

    // NOTE: Sort edges into nondecreasing order by weight
    InsertionSort(EdgeCount, Edges);

    // NOTE: Visit edges in sorted order
    for(uintptr_t EdgeIndex = 0;
        EdgeIndex < EdgeCount;
        ++EdgeIndex)
    {
        edge *Edge = Edges + EdgeIndex;
        if(FindSet(Parents, Edge->UIndex) != FindSet(Parents, Edge->VIndex))
        {
            TempEdges[TempEdgeCount++] = *Edge;
            Union(Parents, Edge->UIndex, Edge->VIndex);
        }
    }

    // NOTE: Copy to result set
    for(uintptr_t DestIndex = 0;
        DestIndex < TempEdgeCount;
        ++DestIndex)
    {
        edge *Dest = Edges + DestIndex;
        edge *Source = TempEdges + DestIndex;
        *Dest = *Source;
    }

    return TempEdgeCount;
}

static bool HasEdge(uintptr_t EdgeCount, edge *Edges, uint32_t UIndex, uint32_t VIndex)
{
    bool Result = false;

    for(uintptr_t EdgeIndex = 0;
        EdgeIndex < EdgeCount;
        ++EdgeIndex)
    {
        edge *Edge = Edges + EdgeIndex;
        if(((Edge->UIndex == UIndex) && (Edge->VIndex == VIndex)) ||
           ((Edge->UIndex == VIndex) && (Edge->VIndex == UIndex)))
        {
            Result = true;
            break;
        }
    }

    return Result;
}

extern "C" int main(void)
{
    srand(time(0));

    uintptr_t VertexCount = 4;
    uintptr_t MaxEdgeCount = (VertexCount * (VertexCount - 1)) / 2;
    uintptr_t EdgeCount = VertexCount + 15;
    if(EdgeCount > MaxEdgeCount)
    {
        EdgeCount = MaxEdgeCount;
    }

    edge *Edges = PushArray(EdgeCount, edge);

    uint32_t *Vertices = PushArray(VertexCount, uint32_t);
    for(uintptr_t VertexIndex = 0;
        VertexIndex < VertexCount;
        ++VertexIndex)
    {
        Vertices[VertexIndex] = VertexIndex;
    }

    for(uintptr_t VertexIndex = VertexCount - 1;
        VertexIndex > 0;
        --VertexIndex)
    {
        uint32_t NextVertexIndex = ((uint32_t)rand() % (VertexIndex + 1));
        uint32_t Temp = Vertices[VertexIndex];
        Vertices[VertexIndex] = Vertices[NextVertexIndex];
        Vertices[NextVertexIndex] = Temp;
    }

    uintptr_t WriteIndex = 0;
    for(uintptr_t VertexIndex = 1;
        VertexIndex < VertexCount;
        ++VertexIndex)
    {
        uint32_t UIndex = Vertices[VertexIndex];
        uint32_t VIndex = Vertices[(uint32_t)rand() % VertexIndex];
        Edges[WriteIndex++] = {UIndex, VIndex, ((uint32_t)rand() % 10) + 1};
    }

    while(WriteIndex < EdgeCount)
    {
        uint32_t UIndex = Vertices[(uint32_t)rand() % VertexCount];
        uint32_t VIndex = Vertices[(uint32_t)rand() % VertexCount];
        if((UIndex != VIndex) &&
           !HasEdge(WriteIndex, Edges, UIndex, VIndex))
        {
            Edges[WriteIndex++] = {UIndex, VIndex, ((uint32_t)rand() % 10) + 1};
        }
    }

    free(Vertices);

    puts("digraph G1 {");
    puts("  label = \"Initial Graph\"");
    puts("  color = red");

    // NOTE: Print graph before running MST
    for(uintptr_t EdgeIndex = 0;
        EdgeIndex < EdgeCount;
        ++EdgeIndex)
    {
        edge *Edge = Edges + EdgeIndex;
        printf("  %u -> %u [label=\"%lu\"]\n", Edge->UIndex, Edge->VIndex, Edge->Weight);
    }
    puts("}");

    puts("digraph MST {");
    puts("  label = \"MST\"");
    puts("  color = blue");

    EdgeCount = MST(EdgeCount, Edges, VertexCount);

    // NOTE: Print graph after running MST
    for(uintptr_t EdgeIndex = 0;
        EdgeIndex < EdgeCount;
        ++EdgeIndex)
    {
        edge *Edge = Edges + EdgeIndex;
        printf("  %u -> %u [label=\"%lu\"]\n", Edge->UIndex, Edge->VIndex, Edge->Weight);
    }

    puts("}");
}
