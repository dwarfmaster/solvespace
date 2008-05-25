
#ifndef __POLYGON_H
#define __POLYGON_H

class SPolygon;
class SMesh;
class SBsp3;

template <class T>
class SList {
public:
    T   *elem;
    int  n;
    int  elemsAllocated;

    void Add(T *t) {
        if(n >= elemsAllocated) {
            elemsAllocated = (elemsAllocated + 32)*2;
            elem = (T *)MemRealloc(elem, elemsAllocated*sizeof(elem[0]));
        }
        elem[n++] = *t;
    }

    void ClearTags(void) {
        int i;
        for(i = 0; i < n; i++) {
            elem[i].tag = 0;
        }
    }

    void Clear(void) {
        if(elem) MemFree(elem);
        elem = NULL;
        n = elemsAllocated = 0;
    }

    void RemoveTagged(void) {
        int src, dest;
        dest = 0;
        for(src = 0; src < n; src++) {
            if(elem[src].tag) {
                // this item should be deleted
            } else {
                if(src != dest) {
                    elem[dest] = elem[src];
                }
                dest++;
            }
        }
        n = dest;
        // and elemsAllocated is untouched, because we didn't resize
    }
};

class SEdge {
public:
    int    tag;
    Vector a, b;
};

class SEdgeList {
public:
    SList<SEdge>    l;

    void Clear(void);
    void AddEdge(Vector a, Vector b);
    bool AssemblePolygon(SPolygon *dest, SEdge *errorAt);
};

class SPoint {
public:
    int     tag;
    Vector  p;
};

class SContour {
public:
    SList<SPoint>   l;

    void MakeEdgesInto(SEdgeList *el);
    void Reverse(void);
    Vector ComputeNormal(void);
    bool IsClockwiseProjdToNormal(Vector n);
    bool ContainsPointProjdToNormal(Vector n, Vector p);
};

class SPolygon {
public:
    SList<SContour> l;
    Vector          normal;

    Vector ComputeNormal(void);
    void AddEmptyContour(void);
    void AddPoint(Vector p);
    bool ContainsPoint(Vector p);
    void MakeEdgesInto(SEdgeList *el);
    void FixContourDirections(void);
    void TriangulateInto(SMesh *m);
    void Clear(void);
};

class STriangle {
public:
    int     tag;
    Vector  a, b, c;

    Vector Normal(void);
    bool ContainsPoint(Vector p);
};

class SBsp2 {
public:
    Vector      np;     // normal to the plane

    Vector      no;     // outer normal to the edge
    double      d;
    SEdge       edge;

    SBsp2       *pos;
    SBsp2       *neg;

    SBsp2       *more;

    static const int POS = 100, NEG = 101, COPLANAR = 200;
    void InsertTriangleHow(int how, STriangle *tr, SMesh *m, SBsp3 *bsp3);
    void InsertTriangle(STriangle *tr, SMesh *m, SBsp3 *bsp3);
    Vector IntersectionWith(Vector a, Vector b);
    SBsp2 *InsertEdge(SEdge *nedge, Vector nnp, Vector out);
    static SBsp2 *Alloc(void);

    void DebugDraw(Vector n, double d);
};

class SBsp3 {
public:
    Vector      n;
    double      d;

    STriangle   tri;
    SBsp3       *pos;
    SBsp3       *neg;

    SBsp3       *more;

    SBsp2       *edges;

    static SBsp3 *Alloc(void);
    static SBsp3 *FromMesh(SMesh *m);

    Vector IntersectionWith(Vector a, Vector b);

    static const int POS = 100, NEG = 101, COPLANAR = 200;
    void InsertHow(int how, STriangle *str, SMesh *instead);
    SBsp3 *Insert(STriangle *str, SMesh *instead);

    void InsertConvexHow(int how, Vector *vertex, int n, SMesh *instead);
    SBsp3 *InsertConvex(Vector *vertex, int n, SMesh *instead);

    void InsertInPlane(bool pos2, STriangle *tr, SMesh *m);

    void DebugDraw(void);
};


class SMesh {
public:
    SList<STriangle>    l;

    bool    flipNormal;
    bool    keepCoplanar;
    bool    atLeastOneDiscarded;

    void Clear(void);
    void AddTriangle(STriangle *st);
    void AddTriangle(Vector a, Vector b, Vector c);
    void AddTriangle(Vector n, Vector a, Vector b, Vector c);
    void DoBounding(Vector v, Vector *vmax, Vector *vmin);
    void GetBounding(Vector *vmax, Vector *vmin);

    void AddAgainstBsp(SMesh *srcm, SBsp3 *bsp3);
    void MakeFromUnion(SMesh *a, SMesh *b);
    void MakeFromDifference(SMesh *a, SMesh *b);
};

#endif

