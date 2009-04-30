#include "stdafx.h"
#include "KdTree.h"
#include <algorithm>
#include <functional>

using namespace std;


CKdTree::CKdTree(const D3DXVECTOR3 *pointList, unsigned int size)
{
    m_MaxDepth = 0;
    m_Points.clear();

    if (m_Points.capacity() < size)
    {
        m_Points.reserve(size);
    }

    //Copy the points
    for (unsigned int i = 0;i < size;i++)
    {
        KdPoint pt;
        pt.index = i;
        pt.position = pointList[i]; 
        m_Points.push_back(pt);
    }

    m_pRoot = new KDTREENODE;
    if (m_Points.size() > 0)
    {
        CreateKdTree(m_pRoot, m_Points.begin(), m_Points.end(), 0);
    }
}

CKdTree::~CKdTree(void)
{
    SAFE_DELETE(m_pRoot); // Clear up;
}

class Compare
{
public:
    Compare(NODE_DIM nodeDim)
    {
        m_NodeDim = nodeDim;
    }
    bool operator()(const KdPoint& _Left, const KdPoint& _Right) const
    {
        float a = 0.0f;
        FLOAT b = 0.0f;
        if (m_NodeDim == X_DIM)
        {
            a = _Left.position.x;
            b = _Right.position.x;
        }
        else if (m_NodeDim == Y_DIM)
        {
            a = _Left.position.y;
            b = _Right.position.y;
        }
        else if (m_NodeDim == Z_DIM)
        {
            a = _Left.position.z;
            b = _Right.position.z;
        }
        if (a < b)
        {
            return true;
        }
        else
            return false;
    }
private:
    NODE_DIM m_NodeDim;
};

void CKdTree::CreateKdTree(LPKDTREENODE pKdNode, vector<KdPoint>::iterator it1, 
                           vector<KdPoint>::iterator it2, unsigned int Depth)
{
    if (pKdNode == NULL)
    {
        pKdNode = new KDTREENODE;
    }

    NODE_DIM nd = X_DIM;
    int d = Depth % 3;
    switch (d)
    {
    case 0:
        nd = X_DIM;
        break;
    case 1:
        nd = Y_DIM;
        break;
    case 2:
        nd = Z_DIM;
        break;
    }

    if (it1 == it2)
    {
        pKdNode->Point = *it1;
        pKdNode->NodeDim = nd;
        return;
    }

    Compare c(nd);
    sort(it1, it2, c);

    vector<KdPoint>::iterator medianIt = it1 + (it2 - it1) / 2;

    pKdNode->Point = *medianIt;
    pKdNode->NodeDim = nd;

    if (Depth > m_MaxDepth)
    {
        m_MaxDepth = Depth;
    }

    if (it1 != medianIt)
    {
        pKdNode->m_pLeftChild = new KDTREENODE;
        CreateKdTree(pKdNode->m_pLeftChild, it1, medianIt, Depth + 1);
    }

    if (medianIt + 1 != it2)
    {
        pKdNode->m_pRightChild = new KDTREENODE;
        CreateKdTree(pKdNode->m_pRightChild, medianIt + 1, it2, Depth + 1);
    }
}

void CKdTree::PrintTree()
{
    PrintTree(m_pRoot, 0);
}

void CKdTree::PrintTree(LPKDTREENODE pTreeNode, unsigned int Depth)
{
    if (pTreeNode != NULL)
    {
        PrintTree(pTreeNode->m_pLeftChild, Depth + 1);
        for (unsigned int i = 0;i < Depth; i++)
        {
            cout << "----";
        }
        cout << " [" << Depth << "] ";
        cout << "Point(" << 
            pTreeNode->Point.position.x << ", " <<
            pTreeNode->Point.position.y << ", " <<
            pTreeNode->Point.position.z << ") Index(" <<
            pTreeNode->Point.index << ")"
            << endl;
        PrintTree(pTreeNode->m_pRightChild, Depth + 1);
    }
}

float SQRDist(D3DXVECTOR3 a, D3DXVECTOR3 b)
{
    float x = (a.x - b.x);
    float y = (a.y - b.y);
    float z = (a.z - b.z);
    return x * x + y * y + z * z;
}

void CKdTree::QueryRange(const D3DXVECTOR3 &positon, const float hh, vector<int> &npIndices)
{
    m_hh = hh;
    m_QueryPoint = positon;
    m_NPoints.clear();
    if(m_pRoot == NULL) return;
    QueryRange(m_pRoot, npIndices);
}

void CKdTree::QueryRange(LPKDTREENODE pKdTreeNode, vector<int> &npIndices)
{
    if (pKdTreeNode == NULL)
    {
        return;
    }

    if (SQRDist(pKdTreeNode->Point.position, m_QueryPoint) < m_hh)
    {
        npIndices.push_back(pKdTreeNode->Point.index);
    }

    float offset = 0;

    switch(pKdTreeNode->NodeDim)
    {
    case X_DIM:
        offset = pKdTreeNode->Point.position.x - m_QueryPoint.x;
        break;
    case Y_DIM:
        offset = pKdTreeNode->Point.position.y - m_QueryPoint.y;
        break;
    case Z_DIM:
        offset = pKdTreeNode->Point.position.z - m_QueryPoint.z;
        break;
    }

    if (offset < 0)
    {
        QueryRange(pKdTreeNode->m_pRightChild, npIndices);
        if (offset * offset < m_hh)
        {
            QueryRange(pKdTreeNode->m_pLeftChild, npIndices);
        }
    }
    else
    {
        QueryRange(pKdTreeNode->m_pLeftChild, npIndices);
        if (offset * offset < m_hh)
        {
            QueryRange(pKdTreeNode->m_pRightChild, npIndices);
        }
    }
}


void CKdTree::PrintNPoints()
{
    for (unsigned int i = 0; i < m_NPoints.size(); i++)
    {
        cout << "Point(" << m_NPoints[i].position.x << ", " <<
            m_NPoints[i].position.y << ", " <<
            m_NPoints[i].position.z << ") Index(" <<
            m_NPoints[i].index << ")" << endl;
    }
}