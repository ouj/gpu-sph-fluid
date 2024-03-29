#include "stdafx.h"
#include "MarchingCubeMeshConstructor.h"
#include "MathHelper.h"
#include "Setting.h"

CMarchingCubeMeshConstructor::CMarchingCubeMeshConstructor(void)
{
    m_pScalarValues = NULL;
    m_IsoLevel = 0.8f;
}

CMarchingCubeMeshConstructor::~CMarchingCubeMeshConstructor(void)
{
    SAFE_DELETE_ARRAY(m_pScalarValues);
}

const unsigned int CMarchingCubeMeshConstructor::m_EdgeTable[256] = 
{
    0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
    0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
    0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
    0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
    0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
    0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
    0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
    0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
    0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
    0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
    0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
    0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
    0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
    0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
    0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
    0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
    0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
    0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
    0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
    0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
    0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
    0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
    0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
    0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
    0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
    0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
    0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
    0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
    0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
    0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
    0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
    0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0
};

const int CMarchingCubeMeshConstructor::m_Faces[256][16] =
{
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
    {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
    {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
    {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
    {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
    {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
    {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
    {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
    {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
    {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
    {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
    {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
    {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
    {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
    {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
    {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
    {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
    {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
    {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
    {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
    {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
    {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
    {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
    {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
    {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
    {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
    {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
    {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
    {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
    {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
    {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
    {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
    {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
    {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
    {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
    {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
    {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
    {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
    {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
    {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
    {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
    {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
    {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
    {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
    {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
    {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
    {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
    {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
    {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
    {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
    {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
    {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
    {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
    {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
    {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
    {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
    {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
    {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
    {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
    {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
    {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
    {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
    {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
    {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
    {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
    {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
    {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
    {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
    {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
    {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
    {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
    {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
    {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
    {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
    {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
    {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
    {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
    {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
    {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
    {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
    {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
    {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
    {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
    {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
    {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
    {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
    {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
    {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
    {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
    {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
    {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
    {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
    {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
    {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
    {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
    {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
    {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};


void CMarchingCubeMeshConstructor::ConstructSurface(const CSmoothedParticles &sParticles)
{
    if (sParticles.GetParticleNum() <= 0)
    {
        return;
    }

    ConstructDistanceField(sParticles);

    m_GridPoints.clear();

    for (unsigned int i = 0;i < sParticles.GetParticleNum(); i++)
    {
        Rasterize(sParticles, i);
    }

    GenerateSurface();
}

void CMarchingCubeMeshConstructor::GenerateSurface(void)
{
    unsigned int nPointsInXDirection = (m_Cx + 1);
    unsigned int nPointsInSlice = nPointsInXDirection * (m_Cy + 1);

    m_Indices.clear();
    m_EdgeID2Indices.clear();
    m_Vertices.clear();
    m_Normals.clear();

    // Generate isoSurface.
    for (int k = 0; k < m_Cz; k++)
    {
        for (int j = 0; j < m_Cy; j++)
        {
            for (int i = 0; i < m_Cx; i++) 
            {
                // Calculate table lookup index from those
                // vertices which are below the isolevel.

                unsigned int tableIndex = 0;
                if (m_pScalarValues[k * nPointsInSlice + j * nPointsInXDirection + i] > m_IsoLevel)
                    tableIndex |= 1;
                if (m_pScalarValues[k * nPointsInSlice + (j + 1) * nPointsInXDirection + i] > m_IsoLevel)
                    tableIndex |= 2;
                if (m_pScalarValues[k * nPointsInSlice + (j + 1) * nPointsInXDirection + (i + 1)] > m_IsoLevel)
                    tableIndex |= 4;
                if (m_pScalarValues[k * nPointsInSlice + j * nPointsInXDirection + (i + 1)] > m_IsoLevel)
                    tableIndex |= 8;
                if (m_pScalarValues[(k + 1) * nPointsInSlice + j * nPointsInXDirection + i] > m_IsoLevel)
                    tableIndex |= 16;
                if (m_pScalarValues[(k + 1) * nPointsInSlice + (j + 1) * nPointsInXDirection + i] > m_IsoLevel)
                    tableIndex |= 32;
                if (m_pScalarValues[(k + 1) * nPointsInSlice + (j + 1) * nPointsInXDirection + (i + 1)] > m_IsoLevel)
                    tableIndex |= 64;
                if (m_pScalarValues[(k + 1) * nPointsInSlice + j * nPointsInXDirection + (i + 1)] > m_IsoLevel)
                    tableIndex |= 128;

                if (m_EdgeTable[tableIndex] != 0)
                {
                    if (m_EdgeTable[tableIndex] & 1)
                    {
                        D3DXVECTOR3 pt = IntersectPoint(i, j ,k, 0);
                        unsigned int id = GetEdgeID(i, j, k, 0);

                        m_Vertices.push_back(pt);
                        pair<map<unsigned int, unsigned int>::iterator, bool> pr;
                        pr = m_EdgeID2Indices.insert(map<unsigned int, unsigned int>::value_type(id, m_Vertices.size() - 1));
                        assert(pr.second);
                        //m_EdgeID2Indices[id] = m_Vertices.size() - 1; 
                    }
                    if (m_EdgeTable[tableIndex] & 8)
                    {
                        D3DXVECTOR3 pt = IntersectPoint(i, j ,k, 3);
                        unsigned int id = GetEdgeID(i, j, k, 3);

                        m_Vertices.push_back(pt);
                        pair<map<unsigned int, unsigned int>::iterator, bool> pr;
                        pr = m_EdgeID2Indices.insert(map<unsigned int, unsigned int>::value_type(id, m_Vertices.size() - 1));
                        assert(pr.second);
                        //m_EdgeID2Indices[id] = m_Vertices.size() - 1;
                    }
                    if (m_EdgeTable[tableIndex] & 256)
                    {
                        D3DXVECTOR3 pt = IntersectPoint(i, j ,k, 8);
                        unsigned int id = GetEdgeID(i, j, k, 8);

                        m_Vertices.push_back(pt);
                        pair<map<unsigned int, unsigned int>::iterator, bool> pr;
                        pr = m_EdgeID2Indices.insert(map<unsigned int, unsigned int>::value_type(id, m_Vertices.size() - 1));
                        assert(pr.second);
                        //m_EdgeID2Indices[id] = m_Vertices.size() - 1;
                    }
                }

                if (i == m_Cx - 1) // last cell in x direction
                {
                    if (m_EdgeTable[tableIndex] & 4)
                    {
                        D3DXVECTOR3 pt = IntersectPoint(i, j, k, 2);
                        unsigned int id = GetEdgeID(i, j, k, 2);

                        m_Vertices.push_back(pt);
                        pair<map<unsigned int, unsigned int>::iterator, bool> pr;
                        pr = m_EdgeID2Indices.insert(map<unsigned int, unsigned int>::value_type(id, m_Vertices.size() - 1));
                        assert(pr.second);
                        //m_EdgeID2Indices[id] = m_Vertices.size() - 1;
                    }
                    if (m_EdgeTable[tableIndex] & 2048)
                    {
                        D3DXVECTOR3 pt = IntersectPoint(i, j, k, 11);
                        unsigned int id = GetEdgeID(i, j, k, 11);

                        m_Vertices.push_back(pt);
                        pair<map<unsigned int, unsigned int>::iterator, bool> pr;
                        pr = m_EdgeID2Indices.insert(map<unsigned int, unsigned int>::value_type(id, m_Vertices.size() - 1));
                        assert(pr.second);
                        //m_EdgeID2Indices[id] = m_Vertices.size() - 1;
                    }
                }

                if (j == m_Cy - 1) // last cell in y direction
                {
                    if (m_EdgeTable[tableIndex] & 2)
                    {
                        D3DXVECTOR3 pt = IntersectPoint(i, j, k, 1);
                        unsigned int id = GetEdgeID(i, j, k, 1);

                        m_Vertices.push_back(pt);
                        pair<map<unsigned int, unsigned int>::iterator, bool> pr;
                        pr = m_EdgeID2Indices.insert(map<unsigned int, unsigned int>::value_type(id, m_Vertices.size() - 1));
                        assert(pr.second);
                        //m_EdgeID2Indices[id] = m_Vertices.size() - 1;
                    }
                    if (m_EdgeTable[tableIndex] & 512)
                    {
                        D3DXVECTOR3 pt = IntersectPoint(i, j, k, 9);
                        unsigned int id = GetEdgeID(i, j, k, 9);

                        m_Vertices.push_back(pt);
                        pair<map<unsigned int, unsigned int>::iterator, bool> pr;
                        pr = m_EdgeID2Indices.insert(map<unsigned int, unsigned int>::value_type(id, m_Vertices.size() - 1));
                        assert(pr.second);
                        //m_EdgeID2Indices[id] = m_Vertices.size() - 1;
                    }
                }

                if (k == m_Cz - 1) // last cell in z direction
                {
                    if (m_EdgeTable[tableIndex] & 16)
                    {
                        D3DXVECTOR3 pt = IntersectPoint(i, j, k, 4);
                        unsigned int id = GetEdgeID(i, j, k, 4);

                        m_Vertices.push_back(pt);
                        pair<map<unsigned int, unsigned int>::iterator, bool> pr;
                        pr = m_EdgeID2Indices.insert(map<unsigned int, unsigned int>::value_type(id, m_Vertices.size() - 1));
                        assert(pr.second);
                        //m_EdgeID2Indices[id] = m_Vertices.size() - 1;
                    }
                    if (m_EdgeTable[tableIndex] & 128)
                    {
                        D3DXVECTOR3 pt = IntersectPoint(i, j, k, 7);
                        unsigned int id = GetEdgeID(i, j, k, 7);

                        m_Vertices.push_back(pt);
                        pair<map<unsigned int, unsigned int>::iterator, bool> pr;
                        pr = m_EdgeID2Indices.insert(map<unsigned int, unsigned int>::value_type(id, m_Vertices.size() - 1));
                        assert(pr.second);
                        //m_EdgeID2Indices[id] = m_Vertices.size() - 1; 
                    }
                }

                if ((i == m_Cx - 1) && (j == m_Cy - 1)) // at the corner
                {
                    if (m_EdgeTable[tableIndex] & 1024)
                    {
                        D3DXVECTOR3 pt = IntersectPoint(i, j, k, 10);
                        unsigned int id = GetEdgeID(i, j, k, 10);

                        m_Vertices.push_back(pt);
                        pair<map<unsigned int, unsigned int>::iterator, bool> pr;
                        pr = m_EdgeID2Indices.insert(map<unsigned int, unsigned int>::value_type(id, m_Vertices.size() - 1));
                        assert(pr.second);
                        //m_EdgeID2Indices[id] = m_Vertices.size() - 1;
                    }
                }
                if ((i == m_Cx - 1) && (k == m_Cz - 1))
                {
                    if (m_EdgeTable[tableIndex] & 64)
                    {
                        D3DXVECTOR3 pt = IntersectPoint(i, j, k, 6);
                        unsigned int id = GetEdgeID(i, j, k, 6);

                        m_Vertices.push_back(pt);
                        pair<map<unsigned int, unsigned int>::iterator, bool> pr;
                        pr = m_EdgeID2Indices.insert(map<unsigned int, unsigned int>::value_type(id, m_Vertices.size() - 1));
                        assert(pr.second);
                        //m_EdgeID2Indices[id] = m_Vertices.size() - 1;
                    }
                }

                if ((j == m_Cy - 1) && (k == m_Cz - 1))
                {
                    if (m_EdgeTable[tableIndex] & 32)
                    {
                        D3DXVECTOR3 pt = IntersectPoint(i, j, k, 5);
                        unsigned int id = GetEdgeID(i, j, k, 5);

                        m_Vertices.push_back(pt);
                        pair<map<unsigned int, unsigned int>::iterator, bool> pr;
                        pr = m_EdgeID2Indices.insert(map<unsigned int, unsigned int>::value_type(id, m_Vertices.size() - 1));
                        assert(pr.second);
                        //m_EdgeID2Indices[id] = m_Vertices.size() - 1;
                    }
                }

                for (unsigned int n = 0; m_Faces[tableIndex][n] != -1; n += 3) 
                {
                    unsigned int edgeID0, edgeID1, edgeID2;

                    edgeID0 = GetEdgeID(i, j, k, m_Faces[tableIndex][n]);
                    edgeID1 = GetEdgeID(i, j, k, m_Faces[tableIndex][n+1]);
                    edgeID2 = GetEdgeID(i, j, k, m_Faces[tableIndex][n+2]);

                    m_Indices.push_back(edgeID0);
                    m_Indices.push_back(edgeID1);
                    m_Indices.push_back(edgeID2);
                }
            }
        }
    }
    UpdateIndices();
    UpdateNormals();
}

void CMarchingCubeMeshConstructor::UpdateNormals()
{
    m_Normals.resize(m_Vertices.size());
    m_Normals.assign(m_Normals.size(), D3DXVECTOR3(0, 0, 0));

    //m_Normals.clear();
    //for (int i = 0; i < m_Vertices.size();i++)
    //{
    //    m_Normals.push_back(D3DXVECTOR3(0, 0, 0));
    //}

    // Calculate normals.
    for (UINT i = 0; i < m_Indices.size(); i += 3) {

        unsigned int id0 = m_Indices[i];
        unsigned int id1 = m_Indices[i + 1];
        unsigned int id2 = m_Indices[i + 2];
        
        D3DXVECTOR3 vec1 = m_Vertices[id1] - m_Vertices[id0];
        D3DXVECTOR3 vec2 = m_Vertices[id2] - m_Vertices[id0];

        D3DXVECTOR3 normal;
        D3DXVec3Cross(&normal, &vec1, &vec2);

        m_Normals[id0] += normal;
        m_Normals[id1] += normal;
        m_Normals[id2] += normal;
    }
}

void CMarchingCubeMeshConstructor::UpdateIndices()
{
    vector<unsigned int>::iterator it = m_Indices.begin();
    while (it != m_Indices.end())
    {
        assert(m_EdgeID2Indices[*it] < 0xffff);
        *it = m_EdgeID2Indices[*it];
        it++;
    }
}

unsigned int CMarchingCubeMeshConstructor::GetVertexID(unsigned int i, unsigned int j, unsigned int k)
{
    return (k * (m_Cy + 1) + j) * (m_Cx + 1) + i;
}

int CMarchingCubeMeshConstructor::GetEdgeID(unsigned int i, unsigned int j, unsigned int k, unsigned int nEdgeNo)
{

    switch(nEdgeNo)
    {
    case 0:
        return 3 * GetVertexID(i, j ,k) + 1;
    case 1:
        return 3 * GetVertexID(i, j + 1, k);
    case 2:
        return 3 * GetVertexID(i + 1, j, k) + 1;
    case 3:
        return 3 * GetVertexID(i, j ,k);
    case 4:
        return 3 * GetVertexID(i, j ,k + 1) + 1;
    case 5:
        return 3 * GetVertexID(i, j + 1, k + 1);
    case 6:
        return 3 * GetVertexID(i + 1, j, k + 1) + 1;
    case 7:
        return 3 * GetVertexID(i, j ,k + 1);
    case 8:
        return 3 * GetVertexID(i, j ,k) + 2;
    case 9:
        return 3 * GetVertexID(i, j + 1, k) + 2;
    case 10:
        return 3 * GetVertexID(i + 1, j + 1, k) + 2;
    case 11:
        return 3 * GetVertexID(i + 1, j ,k) + 2;
    default:
        return -1;
    }
}

D3DXVECTOR3 CMarchingCubeMeshConstructor::IntersectPoint(unsigned int i, unsigned int j, unsigned int k, unsigned int nEdgeNo)
{
    D3DXVECTOR3 vec1, vec2;

    unsigned int v1x = i;
    unsigned int v2x = i;
    unsigned int v1y = j;
    unsigned int v2y = j;
    unsigned int v1z = k;
    unsigned int v2z = k;

    switch (nEdgeNo)
    {
    case 0:
        v2y += 1;
        break;
    case 1:
        v1y += 1;
        v2x += 1;
        v2y += 1;
        break;
    case 2:
        v1x += 1;
        v1y += 1;
        v2x += 1;
        break;
    case 3:
        v1x += 1;
        break;
    case 4:
        v1z += 1;
        v2y += 1;
        v2z += 1;
        break;
    case 5:
        v1y += 1;
        v1z += 1;
        v2x += 1;
        v2y += 1;
        v2z += 1;
        break;
    case 6:
        v1x += 1;
        v1y += 1;
        v1z += 1;
        v2x += 1;
        v2z += 1;
        break;
    case 7:
        v1x += 1;
        v1z += 1;
        v2z += 1;
        break;
    case 8:
        v2z += 1;
        break;
    case 9:
        v1y += 1;
        v2y += 1;
        v2z += 1;
        break;
    case 10:
        v1x += 1;
        v1y += 1;
        v2x += 1;
        v2y += 1;
        v2z += 1;
        break;
    case 11:
        v1x += 1;
        v2x += 1;
        v2z += 1;
        break;
    }

    vec1.x = v1x * m_CellH.x;
    vec1.y = v1y * m_CellH.y;
    vec1.z = v1z * m_CellH.z;
    vec2.x = v2x * m_CellH.x;
    vec2.y = v2y * m_CellH.y;
    vec2.z = v2z * m_CellH.z;

    unsigned int nPointsInXDirection = (m_Cx + 1);
    unsigned int nPointsInSlice = nPointsInXDirection * (m_Cy + 1);
    float val1 = m_pScalarValues[v1z * nPointsInSlice + v1y * nPointsInXDirection + v1x];
    float val2 = m_pScalarValues[v2z * nPointsInSlice + v2y * nPointsInXDirection + v2x];
    D3DXVECTOR3 intersection = Interpolate(vec1, vec2, val1, val2);

    return intersection + m_MinBound;
}

D3DXVECTOR3 CMarchingCubeMeshConstructor::Interpolate(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2, 
                                                      float val1, float val2)
{
    assert(val1 != val2);
    D3DXVECTOR3 interpolation;
    float mu;
    mu = float(( m_IsoLevel - val1)) /( val2 - val1);
    interpolation = vec1 + mu * (vec2 - vec1);
    return interpolation;
}


void CMarchingCubeMeshConstructor::ComputeBoundingBox(const CSmoothedParticles &sParticles)
{
    m_MaxBound = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    m_MinBound = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
    for (UINT i = 0; i < sParticles.GetParticleNum(); i++)
    {
        if (sParticles.positions[i].x > m_MaxBound.x)
        {
            m_MaxBound.x = sParticles.positions[i].x;
        }
        if (sParticles.positions[i].y > m_MaxBound.y)
        {
            m_MaxBound.y = sParticles.positions[i].y;
        }
        if (sParticles.positions[i].z > m_MaxBound.z)
        {
            m_MaxBound.z = sParticles.positions[i].z;
        }
        if (sParticles.positions[i].x < m_MinBound.x)
        {
            m_MinBound.x = sParticles.positions[i].x;
        }
        if (sParticles.positions[i].y < m_MinBound.y)
        {
            m_MinBound.y = sParticles.positions[i].y;
        }
        if (sParticles.positions[i].z < m_MinBound.z)
        {
            m_MinBound.z = sParticles.positions[i].z;
        }
    }

    // take some offset (the surface might go a little outside
    // the box of the particle positions
    const D3DXVECTOR3 offset(2.0f * CSetting::GetSmoothingLength(), 
        2.0f * CSetting::GetSmoothingLength(),
        2.0f * CSetting::GetSmoothingLength());
    m_MinBound -= offset;
    m_MaxBound += offset;

    // make sure that subsequent constructions of the grid
    // have alligned grid cells (to avoid flickering which
    // mc can be prone of)
    m_MinBound /= CSetting::GetSamplingLength();
    CMathHelper::VectorFloor(m_MinBound);
    m_MinBound *= CSetting::GetSamplingLength();

    m_MaxBound /= CSetting::GetSamplingLength();
    CMathHelper::VectorCeil(m_MaxBound);
    m_MaxBound *= CSetting::GetSamplingLength();
}

void CMarchingCubeMeshConstructor::Rasterize(const CSmoothedParticles &sParticles, unsigned int index)
{
    D3DXVECTOR3 p = sParticles.positions[index];
    D3DXVECTOR3 tp = p - m_MinBound;
    float h = CSetting::GetSmoothingLength();
    float rho = sParticles.densities[index];


    unsigned int xstart = max(0, (int)floor((tp.x - h) / m_CellH.x));
    unsigned int xend = min(m_Cx, (int)ceil((tp.x + h) / m_CellH.x));

    unsigned int ystart = max(0, (int)floor((tp.y - h) / m_CellH.y));
    unsigned int yend = min(m_Cy, (int)ceil((tp.y + h) / m_CellH.y));

    unsigned int zstart = max(0, (int)floor((tp.z - h) / m_CellH.z));
    unsigned int zend = min(m_Cz, (int)ceil((tp.z + h) / m_CellH.z));

    float HH = 4 * h * h;

    for (UINT k = zstart;k <= zend; k++) {
        for (UINT j = ystart; j <= yend; j++) {
            for (UINT i = xstart; i <= xend; i++) {

                D3DXVECTOR3 gridPoint(i * m_CellH.x, j * m_CellH.y, k * m_CellH.z);
                gridPoint += m_MinBound;
                const float sqrDist = CMathHelper::SquaredDistance(gridPoint, p);
                if (sqrDist > HH) 
                    continue;

                const int idx = ( k * (m_Cy + 1) + j ) * (m_Cx + 1) + i;

                float weight = 1.0f - sqrDist / HH;
                weight /= rho;
                weight = weight * weight * weight; //w = k(s)=max(0,(1−s^2)^3);
                

                m_pScalarValues[idx] += weight;
            }
        }
    }
}

void CMarchingCubeMeshConstructor::ConstructDistanceField(const CSmoothedParticles &sParticles)
{
    ComputeBoundingBox(sParticles);

    m_Cx = (int)((m_MaxBound.x - m_MinBound.x) / CSetting::GetSamplingLength());
    m_Cy = (int)((m_MaxBound.y - m_MinBound.y) / CSetting::GetSamplingLength());
    m_Cz = (int)((m_MaxBound.z - m_MinBound.z) / CSetting::GetSamplingLength());

    m_CellH = D3DXVECTOR3((m_MaxBound.x - m_MinBound.x) / (float)m_Cx, 
        (m_MaxBound.y - m_MinBound.y) / (float)m_Cy, 
        (m_MaxBound.z - m_MinBound.z) / (float)m_Cz);

    InitializeDistanceField();
}

void CMarchingCubeMeshConstructor::InitializeDistanceField()
{
    DestroyBuffers();

    const int count = (m_Cx + 1) * (m_Cy + 1) * (m_Cz + 1); // vetices number

    m_pScalarValues = new float[count];

    memset(m_pScalarValues, 0, count * sizeof(float));
}

void CMarchingCubeMeshConstructor::DestroyBuffers()
{
    SAFE_DELETE_ARRAY(m_pScalarValues);
}

D3DXVECTOR3 CMarchingCubeMeshConstructor::GetMaxBound()
{
    return m_MaxBound;
}

D3DXVECTOR3 CMarchingCubeMeshConstructor::GetMinBound()
{
    return m_MinBound;
}
