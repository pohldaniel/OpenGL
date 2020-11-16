#ifndef _CCOLLISION_H_
#define _CCOLLISION_H_

#define NOMINMAX

#include <windows.h>	
#include <vector>
#include <algorithm>
#include <iostream>
#include "Vector.h"

class Collision {
public:
	
	struct CollTriangle{
		unsigned long indices[3];        
		Vector3f normal;
	};

	struct CollIntersect{
		Vector3f        newCenter;       // the new sphere/ellipsoid centre point
		Vector3f        intersectPoint;  // the point on the exterior of the sphere/ellipsoid where contact happened
		Vector3f        intersectNormal; // the intersection normal (sliding plane)
		float           interval;        // the Time of intersection (Centre + Velocity * Interval)
		unsigned long   triangleIndex;   // the index of the triangle we are intersecting
	};

	Collision();
	virtual ~Collision();

	// public Static Functions for this Class.
	static bool  PointInTriangle(const Vector3f& Point, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3);
	static bool  PointInTriangle(const Vector3f& Point, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, const Vector3f& TriNormal);
	static bool  PointInAABB(const Vector3f& Point, const Vector3f& Min, const Vector3f& Max);

	static bool  SphereIntersectPlane(const Vector3f& Center, float Radius, const Vector3f& Velocity, const Vector3f& PlaneNormal, const Vector3f& PlanePoint, float& tMax);
	static bool  SphereIntersectLineSegment(const Vector3f& Center, float Radius, const Vector3f& Velocity, const Vector3f& v1, const Vector3f& v2, float& tMax, Vector3f& CollisionNormal);
	static bool  SphereIntersectPoint(const Vector3f& Center, float Radius, const Vector3f& Velocity, const Vector3f& Point, float& tMax, Vector3f& CollisionNormal);
	static bool  SphereIntersectTriangle(const Vector3f& Center, float Radius, const Vector3f& Velocity, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, const Vector3f& TriNormal, float& tMax, Vector3f& CollisionNormal);
	static bool  SphereIntersectTriangleBidirectional(const Vector3f& Center, float Radius, const Vector3f& Velocity, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, const Vector3f& TriNormal, float& tMax, Vector3f& CollisionNormal);

	
	// public Functions for this Class.
	bool         addIndexedPrimitive(std::vector<Vector3f>& vertices, std::vector<unsigned int>& indices);

	void         setWorldTransform(const Matrix4f& mtxWorld);
	void         setTriBufferGrow(unsigned short growCount);
	void         setVertBufferGrow(unsigned short growCount);
	void         setMaxIntersections(unsigned short maxIntersections);
	void         setMaxIterations(unsigned short maxIterations);

	bool         collideEllipsoid(const Vector3f& center, const Vector3f& radius, const Vector3f& velocity, Vector3f& newCenter, Vector3f& newIntegrationVelocity, Vector3f& collExtentsMin, Vector3f& collExtentsMax);
	bool         ellipsoidIntersectScene(const Vector3f& center, const Vector3f& radius, const Vector3f& velocity, CollIntersect intersections[], unsigned long& intersectionCount, bool inputEllipsoidSpace = false, bool returnEllipsoidSpace = false);

private:

	// private Static Functions for this Class.
	static bool SolveCollision(float a, float b, float c, float& t);
	inline static Vector3f Vec3VecScale(const Vector3f& v1, const Vector3f& v2) { return Vector3f(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]); }

	// private Functions for this Class.
	bool		  addBufferData(std::vector<Vector3f>& vertBuffer, std::vector<CollTriangle>& triBuffer, std::vector<Vector3f>& vertices, std::vector<unsigned int>& indices, const Matrix4f& mtxWorld);
	unsigned long ellipsoidIntersectBuffers(std::vector<Vector3f>& vertBuffer, std::vector<CollTriangle>& triBuffer, const Vector3f& eCenter, const Vector3f& radius, const Vector3f& invRadius, const Vector3f& eVelocity, float& eInterval, CollIntersect intersections[], unsigned long& intersectionCount, Matrix4f* pTransform = NULL);


	

	// private Variables for this Class.
	std::vector<CollTriangle> m_collTriangles;        // the triangle data referencing the vertices below.
	std::vector<Vector3f>	  m_collVertices;         // the vertices defining the level geometry.
	unsigned short			  m_triGrowCount;         // number of elements to grow the triangle buffer when full
	unsigned short			  m_vertGrowCount;        // number of elements to grow the vertex buffer when full
	unsigned short			  m_maxIntersections;     // the total number of intersections which we should record
	unsigned short			  m_maxIterations;        // the maximum number of collision test iterations we should try before failing
	CollIntersect			 *m_pIntersections;       // internal buffer for storing intersection information
	Matrix4f				  m_mtxWorldTransform;    // when adding primitives, this transform will be applied.
};

#endif // _COLLISION_H_
