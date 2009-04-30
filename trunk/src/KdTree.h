#pragma once
#include "stdafx.h"
#include <d3dx9math.h>

class CKdTreeNode;
typedef CKdTreeNode KDTREENODE, *LPKDTREENODE;

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif    

enum NODE_DIM
{
    X_DIM,
    Y_DIM,
    Z_DIM
};

struct KdPoint
{
    KdPoint(void){};
    KdPoint(D3DXVECTOR3 pos, unsigned int i) : position(pos), index(i) {}
    D3DXVECTOR3 position;
    int         index;
};

class CKdTreeNode
{
public:
    CKdTreeNode()
    {
        m_pLeftChild = NULL;
        m_pRightChild = NULL;
    }

    virtual ~CKdTreeNode()
    {
        SAFE_DELETE(m_pLeftChild);
        SAFE_DELETE(m_pRightChild);
    }

    LPKDTREENODE m_pLeftChild;
    LPKDTREENODE m_pRightChild;

    KdPoint     Point;
    NODE_DIM    NodeDim;
};

class CKdTree
{
public:
    CKdTree(const D3DXVECTOR3 *pointList, unsigned int size);
    ~CKdTree(void);

    void PrintTree();
    void PrintNPoints();
    void QueryRange(const D3DXVECTOR3 &positon, const float hh, std::vector<int> &npIndices);
protected:
    void PrintTree(LPKDTREENODE, unsigned int Depth);
    void CreateKdTree(LPKDTREENODE pKdNode, std::vector<KdPoint>::iterator it1, 
        std::vector<KdPoint>::iterator it2, unsigned int Depth);
    void QueryRange(LPKDTREENODE pKdTreeNode, std::vector<int> &npIndices);
public:
    std::vector<KdPoint>    m_NPoints;
private:
    LPKDTREENODE            m_pRoot;
    std::vector<KdPoint>    m_Points;
    unsigned int            m_MaxDepth;

    float                   m_hh;
    float                   m_h;
    D3DXVECTOR3             m_QueryPoint;
};
