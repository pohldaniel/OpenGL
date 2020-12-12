#include "Collision.h"

Collision::Collision(){
	m_triGrowCount = 100;
	m_vertGrowCount = 100;
	m_maxIntersections = 100;
	m_maxIterations = 30;

	// reset the internal transformation matrix
	m_mtxWorldTransform = Matrix4f::IDENTITY;

	// allocate memory for intersections
	m_pIntersections = new CollIntersect[m_maxIntersections];
}

Collision::~Collision(){
	if (m_pIntersections) delete[] m_pIntersections;
	m_pIntersections = NULL;
}


// static utility functions
bool Collision::PointInTriangle(const Vector3f& point, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, const Vector3f& triNormal){

	Vector3f edge, edgeNormal, direction;

	// first edge
	edge = v2 - v1;
	direction = v1 - point;
	edgeNormal = Vector3f::Cross(edge, triNormal);

	
	// in front of edge?
	if (Vector3f::Dot(direction, edgeNormal) < 0.0f) return false;

	// second edge
	edge = v3 - v2;
	direction = v2 - point;
	edgeNormal = Vector3f::Cross(edge, triNormal);

	// in front of edge?
	if (Vector3f::Dot(direction, edgeNormal) < 0.0f) return false;

	// third edge
	edge = v1 - v3;
	direction = v3 - point;
	edgeNormal = Vector3f::Cross(edge, triNormal);

	// in front of edge?
	if (Vector3f::Dot(direction, edgeNormal) < 0.0f) return false;

	// we are behind all planes
	return true;
}

bool Collision::PointInTriangle(const Vector3f& point, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3){

	// generate the triangle normal
	Vector3f normal = Vector3f::Cross(v2 - v1, v3 - v1);
	Vector3f::Normalize(normal);
	return Collision::PointInTriangle(point, v1, v2, v3, normal);
}

bool Collision::PointInAABB(const Vector3f& point, const Vector3f& min, const Vector3f& max){

	// does the point fall outside any of the AABB planes?
	if (point[0] < min[0] || point[0] > max[0]) return false;
	if (point[1] < min[1] || point[1] > max[1]) return false;
	if (point[2] < min[2] || point[2] > max[2]) return false;

	// we are behind all planes
	return true;
}

bool Collision::SphereIntersectTriangle(const Vector3f& center, float radius, const Vector3f& velocity, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, const Vector3f& triNormal, float& tmax, Vector3f& collisionNormal) {

	float       t = tmax;
	Vector3f    collisionCenter;
	bool        collided = false;

	// find the time of collision with the triangle's plane.
	if (!SphereIntersectPlane(center, radius, velocity, triNormal, v1, t)) return false;

	// calculate the sphere's center at the point of collision with the plane
	if (t < 0)
		collisionCenter = center + (triNormal * -t);
	else
		collisionCenter = center + (velocity * t);


	// if this point is within the bounds of the triangle, we have found the collision
	if (PointInTriangle(collisionCenter, v1, v2, v3, triNormal)) {
		// collision normal is just the triangle normal
		collisionNormal = triNormal;
		tmax = t;

		// Intersecting!
		return true;

	} // end if point within triangle interior

	  // otherwise we need to test each edge
	collided |= SphereIntersectLineSegment(center, radius, velocity, v1, v2, tmax, collisionNormal);
	collided |= SphereIntersectLineSegment(center, radius, velocity, v2, v3, tmax, collisionNormal);
	collided |= SphereIntersectLineSegment(center, radius, velocity, v3, v1, tmax, collisionNormal);
	return collided;
}

bool Collision::SphereIntersectTriangleBidirectional(const Vector3f& center, float radius, const Vector3f& velocity, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, const Vector3f& triNormal, float& tmax, Vector3f& collisionNormal){

	float       tccw = tmax, tcw = tmax, t;
	Vector3f    collisionCenter;
	bool        collided = false;

	// find the time of collision with the triangle's plane.
	bool ccw = SphereIntersectPlane(center, radius, velocity, triNormal, v1, tccw);
	bool cw  = SphereIntersectPlane(center, radius, velocity, -triNormal, v1, tcw);
		
	if (!(ccw || cw)) return false;
	t = ccw ? tccw : tcw;

	// calculate the sphere's center at the point of collision with the plane
	if (ccw && t < 0)
		collisionCenter = center - (triNormal * t);
	else if(cw && t < 0)
		collisionCenter = center + (triNormal * t);
	else
		collisionCenter = center + (velocity * t);
	
	// if this point is within the bounds of the triangle, we have found the collision
	if (ccw && PointInTriangle(collisionCenter, v1, v2, v3, triNormal)){
		// collision normal is just the triangle normal
		collisionNormal = triNormal;
		tmax = tccw;
		return true;
	} 
	
	if (cw && PointInTriangle(collisionCenter, v2, v1, v3, -triNormal)) {
		// collision normal is just the triangle normal
		collisionNormal = -triNormal;
		tmax = tcw;
		return true;
	} 

	// otherwise we need to test each edge
	collided |= SphereIntersectLineSegment(center, radius, velocity, v1, v2, tmax, collisionNormal);
	collided |= SphereIntersectLineSegment(center, radius, velocity, v2, v3, tmax, collisionNormal);
	collided |= SphereIntersectLineSegment(center, radius, velocity, v3, v1, tmax, collisionNormal);
	
	return collided;
}

bool Collision::SphereIntersectPlane(const Vector3f& center, float radius, const Vector3f& velocity, const Vector3f& planeNormal, const Vector3f& planePoint, float& tmax){

	float numer, denom, t;
	
	// setup equation
	numer = Vector3f::Dot(center - planePoint, planeNormal) - radius;
	denom = Vector3f::Dot(velocity, planeNormal);
	
	// are we already overlapping?
	if (numer < 0.0f || denom > -0.0000001f ){
		
		// the sphere is moving away from the plane
		if (denom > -1e-5f ) return false;

		// sphere is too far away from the plane
		if (numer < -radius ) return false;

		// calculate the penetration depth
		tmax = numer;
		
		// intersecting!
		return true;

	} // end if overlapping*/

	t = -(numer / denom);
	if (t < 0.0f || t > tmax) return false;

	// store interval
	tmax = t;
	
	// intersecting!
	return true;
}

bool Collision::SphereIntersectLineSegment(const Vector3f& center, float radius, const Vector3f& velocity, const Vector3f& v1, const Vector3f& v2, float& tmax, Vector3f& collisionNormal){
	Vector3f E, L, X, Y, pointOnEdge, collisionCenter;
	float a, b, c, d, e, t, n;

	// setup the equation values
	E = v2 - v1;
	L = center - v1;

	// re-normalise the cylinder radius with segment length (((P - C) x E)² = r²)
	e = E.length();

	// if e == 0 we can't possibly succeed, the edge is degenerate and we'll
	// get a divide by 0 error in the normalization and 2nd order solving.
	if (e < 1e-5f) return false;

	// normalize the line vector
	E = E/e;
	
	// generate cross values
	X = Vector3f::Cross(L, E);
	Y = Vector3f::Cross(velocity, E);
	
	// setup the input values for the quadratic equation
	a = Y.lengthSq();
	b = 2.0f * Vector3f::Dot(X, Y);
	c = X.lengthSq() - (radius * radius);

	// if the sphere centre is already inside the cylinder, we need an overlap test
	if (c < 0.0f){

		// find the distance along the line where our sphere center is positioned.
		// (i.e. sphere center projected down onto line)
		d = Vector3f::Dot(L, E);

		// is this before or after line start?
		if (d < 0.0f){

			// the point is before the beginning of the line, test against the first vertex
			return SphereIntersectPoint(center, radius, velocity, v1, tmax, collisionNormal);

		} // end if before line start
		else if (d > e){

			// the point is after the end of the line, test against the second vertex
			return SphereIntersectPoint(center, radius, velocity, v2, tmax, collisionNormal);

		} // end if after line end
		else{

			// point within the line segment
			pointOnEdge = v1 + E * d;

			// generate collision normal
			collisionNormal = center - pointOnEdge;
			n = collisionNormal.length();
			collisionNormal = collisionNormal/ n;

			// calculate t value (remember we only enter here if we're already overlapping)
			// remember, when we're overlapping we have no choice but to return a physical distance (the penetration depth)
			t = n - radius;
			if (tmax < t) return false;

			// store t and return
			tmax = t;

			// edge overlap
			return true;

		} // end if inside line segment

	} // end if sphere inside cylinder

	  // if we are already checking for overlaps, return
	if (tmax < 0.0f) return false;

	// solve the quadratic for t
	if (!SolveCollision(a, b, c, t)) return false;

	// is the segment too far away?
	if (t > tmax) return false;

	// calculate the new sphere center at the time of collision
	collisionCenter = center + velocity * t;

	// project this down onto the edge
	d = Vector3f::Dot((collisionCenter - v1), E);

	// simply check whether we need to test the end points as before
	if (d < 0.0f)
		return SphereIntersectPoint(center, radius, velocity, v1, tmax, collisionNormal);
	else if (d > e)
		return SphereIntersectPoint(center, radius, velocity, v2, tmax, collisionNormal);

	// caclulate the Point of contact on the line segment
	pointOnEdge = v1 + E * d;

	// we can now generate our normal, store the interval and return
	collisionNormal = (collisionCenter - pointOnEdge).normalize();
	tmax = t;

	// intersecting
	return true;
}

bool Collision::SphereIntersectPoint(const Vector3f& center, float radius, const Vector3f& velocity, const Vector3f& point, float& tmax, Vector3f& collisionNormal){
	Vector3f L, collisionCenter;
	float a, b, c, l, l2, t;

	// setup the equation values
	L = center - point;
	l2 = L.lengthSq();
	
	// setup the input values for the quadratic equation
	a = velocity.lengthSq();
	b = 2.0f * Vector3f::Dot(velocity, L);
	c = l2 - (radius * radius);

	// if c < 0 then we are overlapping, return the overlap
	if (c < 0.0f){

		// remember, when we're overlapping we have no choice 
		// but to return a physical distance (the penetration depth)
		l = sqrtf(l2);
		t = l - radius;

		// outside our range?
		if (tmax < t) return false;

		// generate the collision normal
		collisionNormal = L / l;

		// store t and return
		tmax = t;

		// vertex Overlap
		return true;

	} // end if overlapping 

	  // if we are already checking for overlaps, return
	if (tmax < 0.0f) return false;

	// solve the quadratic for t
	if (!SolveCollision(a, b, c, t)) return false;

	// is the vertex too far away?
	if (t > tmax) return false;

	// calculate the new sphere position at the time of contact
	collisionCenter = center + velocity * t;

	// we can now generate our normal, store the interval and return
	collisionNormal = (collisionCenter - point).normalize();
	tmax = t;

	// intersecting!
	return true;
}

bool Collision::SolveCollision(float a, float b, float c, float& t){

	float d, one_over_two_a, t0, t1, temp;

	// Basic equation solving
	d = b*b - 4 * a*c;

	// No root if d < 0
	if (d < 0.0f) return false;

	// Setup for calculation
	d = sqrtf(d);
	one_over_two_a = 1.0f / (2.0f * a);

	// Calculate the two possible roots
	t0 = (-b - d) * one_over_two_a;
	t1 = (-b + d) * one_over_two_a;

	// Order the results
	if (t1 < t0) { temp = t0; t0 = t1; t1 = temp; }

	// Fail if both results are negative
	if (t1 < 0.0f) return false;

	// Return the first positive root
	if (t0 < 0.0f) t = t1; else t = t0;

	// Solution found
	return true;

}

bool Collision::addBufferData(std::vector<Vector3f>& vertBuffer, std::vector<CollTriangle>& triBuffer, std::vector<Vector3f>& vertices, std::vector<unsigned int>& indices, const Matrix4f& mtxWorld) {

	unsigned long i, index1, index2, index3, baseVertex;
	CollTriangle triangle;
	Vector3f  edge1, edge2;

	try{
		// grow the vertex buffer if required
		while (vertBuffer.capacity() < vertBuffer.size() + vertices.size()){
			// reserve extra space
			vertBuffer.reserve(vertBuffer.capacity() + m_vertGrowCount);
		} 

		// grow the triangle buffer if required
		while (triBuffer.capacity() < triBuffer.size() + (indices.size() / 3)){
			// reserve extra space
			triBuffer.reserve(triBuffer.capacity() + m_triGrowCount);

		}

		// store the original vertex count before we copy
		baseVertex = vertBuffer.size();

		// for each vertex passed
		for (i = 0; i < vertices.size(); i++){
			
			// transform the vertex
			Vector3f vertex = vertices[i];
			vertex = mtxWorld * Vector4f(vertex[0], vertex[1], vertex[2], 1.0f);
			vertBuffer.push_back(vertex);
		}

		// build the triangle data
		for (i = 0; i <  indices.size(); i = i +3){
			// retrieve the three indices
			index1 = indices[i];
			index2 = indices[i + 1];
			index3 = indices[i + 2];

			// store the details
			triangle.indices[0] = index1 + baseVertex;
			triangle.indices[1] = index2 + baseVertex;
			triangle.indices[2] = index3 + baseVertex;

			// retrieve the vertices themselves
			Vector3f &v1 = vertBuffer[triangle.indices[0]];
			Vector3f &v2 = vertBuffer[triangle.indices[1]];
			Vector3f &v3 = vertBuffer[triangle.indices[2]];

			// calculate and store edge values
			edge1 = (v2 - v1).normalize();
			edge2 = (v3 - v1).normalize();
			
			// skip if this is a degenerate triangle, we don't want it in our set
			float fDot = Vector3f::Dot(edge1, edge2);
			if (fabsf(fDot) >= (1.0f - 1e-5f)) continue;

			// generate the triangle normal
			triangle.normal = Vector3f::Cross(edge1, edge2);
			Vector3f::Normalize(triangle.normal);
			
			// store the triangle in our database
			triBuffer.push_back(triangle);
		}
	}catch (...){
		return false;
	} 
	return true;
}

bool Collision::addIndexedPrimitive(std::vector<Vector3f>& vertices, std::vector<unsigned int>& indices) {
	return addBufferData(m_collVertices, m_collTriangles, vertices, indices, m_mtxWorldTransform);
}

void Collision::setTriBufferGrow(unsigned short nGrowCount){
	m_triGrowCount = std::max((unsigned short)1, nGrowCount);
}

void Collision::setVertBufferGrow(unsigned short nGrowCount){
	m_triGrowCount = std::max((unsigned short)1, nGrowCount);
}

void Collision::setMaxIntersections(unsigned short nMaxIntersections){
	m_maxIntersections = std::max((unsigned short)1, nMaxIntersections);

	if (m_pIntersections) delete[]m_pIntersections;
	m_pIntersections = NULL;

	m_pIntersections = new CollIntersect[m_maxIntersections];
}

void Collision::setMaxIterations(unsigned short nMaxIterations){
	m_maxIterations = std::max((unsigned short)1, nMaxIterations);
}

void Collision::setWorldTransform(const Matrix4f& mtxWorld){
	m_mtxWorldTransform = mtxWorld;
}


//-----------------------------------------------------------------------------
// Name : collideEllipsoid ()
// Desc : this is the main collision detection AND response function for
//        dealing with the interaction of scene and ellipse.
// Note : for ease of understanding, all variables used in this function which
//        begin with a lower case 'e' (i.e. eNormal) denote that the values
//        contained within it are described in ellipsoid space.
bool Collision::collideEllipsoid(const Vector3f& center, const Vector3f& radius, const Vector3f& velocity, Vector3f& newCenter, Vector3f& newIntegrationVelocity, Vector3f &slideNormal, Vector3f& collExtentsMin, Vector3f& collExtentsMax) {
	
	Vector3f	  vecOutputPos, vecOutputVelocity, invRadius, vecNormal, vecExtents;
	Vector3f	  eVelocity, eInputVelocity, eFrom, eTo, vecNewCenter, vecIntersectPoint;
	Vector3f	  vecFrom, vecEndPoint, vecAdjust;
	unsigned long intersectionCount, i;
	float         fDistance, fDot;
	bool          bHit = false;
	
	// Default the output to move in clear space, regardless of whether we 
	// return true or not (we're going to alter this if we register a hit)
	// Note : We store this in a separate variable in case the user passed
	//        a reference to the same vector to both vecPos and vecNewPos
	vecOutputPos = center + velocity;
	vecOutputVelocity = velocity;

	// default our intersection extents
	collExtentsMin[0] = radius[0];
	collExtentsMin[1] = radius[1];
	collExtentsMin[2] = radius[2];
	collExtentsMax[0] = -radius[0];
	collExtentsMax[1] = -radius[1];
	collExtentsMax[2] = -radius[2];

	// store ellipsoid transformation values
	invRadius = Vector3f(1.0f / radius[0], 1.0f / radius[1], 1.0f / radius[2]);

	// calculate values in ellipsode space
	eVelocity = Vec3VecScale(velocity, invRadius);
	eFrom = Vec3VecScale(center, invRadius);
	eTo = eFrom + eVelocity;

	// store the input velocity for jump testing
	eInputVelocity = eVelocity;

	// Keep testing until we hit our max iteration limit
	for (i = 0; i < m_maxIterations; ++i){

		// break out if our velocity is too small (optional but sometimes beneficial)
		if (eVelocity.length() < 1e-5f) break;

		// attempt scene intersection
		// note : we are working totally in ellipsoid space at this point, so we specify that both
		// our input values are in ellipsoid space, and we would like our output values in ellipsoid space also.
		if (ellipsoidIntersectScene(eFrom, radius, eVelocity, m_pIntersections, intersectionCount, true, true)){
			// Retrieve the first collision intersections
			CollIntersect & FirstIntersect = m_pIntersections[0];

			// calculate the WORLD space sliding normal
			vecNormal = Vec3VecScale(FirstIntersect.intersectNormal, invRadius).normalize();	

			// generate slide velocity
			fDistance = Vector3f::Dot(vecOutputVelocity, vecNormal);
			vecOutputVelocity -= vecNormal * fDistance;

			// set the sphere position to the collision position for the next iteration of testing
			eFrom = FirstIntersect.newCenter;

			// project the end of the velocity vector onto the collision plane (at the sphere centre)
			fDistance = Vector3f::Dot((eTo - FirstIntersect.newCenter), FirstIntersect.intersectNormal);
			eTo -= FirstIntersect.intersectNormal * fDistance;

			// transform the sphere position back into world space, and recalculate the intersect point
			// (we recalculate because we want our collision extents to be based on the slope of intersections
			//  given in world space, rather than ellipsoid space. This gives us a better quality slope test later).
			vecNewCenter = Vec3VecScale(FirstIntersect.newCenter, radius);
			vecIntersectPoint = vecNewCenter - Vec3VecScale(vecNormal, radius);

			slideNormal = center - vecIntersectPoint;
			Vector3f::Normalize(slideNormal);
			// calculate the min / max collision extents around the ellipsoid center
			vecExtents = vecIntersectPoint - vecNewCenter;
			if (vecExtents[0] > collExtentsMax[0]) collExtentsMax[0] = vecExtents[0];
			if (vecExtents[1] > collExtentsMax[1]) collExtentsMax[1] = vecExtents[1];
			if (vecExtents[2] > collExtentsMax[2]) collExtentsMax[2] = vecExtents[2];
			if (vecExtents[0] < collExtentsMin[0]) collExtentsMin[0] = vecExtents[0];
			if (vecExtents[1] < collExtentsMin[1]) collExtentsMin[1] = vecExtents[1];
			if (vecExtents[2] < collExtentsMin[2]) collExtentsMin[2] = vecExtents[2];

			// update the velocity value
			eVelocity = eTo - eFrom;

			// we hit something
			bHit = true;

			// filter 'Impulse' jumps
			if (Vector3f::Dot(eVelocity, eInputVelocity) < 0) { eTo = eFrom; break; }

		} else{
			// no collisions, so break out of the loop
			break;

		} 
	}

	// did we register any intersection at all?
	if (bHit){
		
		// did we finish neatly or not?
		if (i < m_maxIterations){

			// return our final position in world space
			vecOutputPos = Vec3VecScale(eTo, radius);

		}else{

			// just find the closest intersection
			eFrom = Vec3VecScale(center, invRadius);

			// attempt to intersect the scene
			intersectionCount = 0;
			if (ellipsoidIntersectScene(eFrom, radius, eInputVelocity, m_pIntersections, intersectionCount, true, true)){

				// retrieve the intersection point in clear space, but ensure that we undo the epsilon
				// shift we apply during the above call. This ensures that when we are stuck between two
				// planes, we don't slowly push our way through.
				vecOutputPos = m_pIntersections[0].newCenter - (m_pIntersections[0].intersectNormal * 1e-3f);

				// Scale back into world space
				vecOutputPos = Vec3VecScale(vecOutputPos, radius);

			}else {
				// don't move at all, stay where we were
				vecOutputPos = center;

			} // end if no collision

		} // end if bad situation

	} // end if intersection found

	// store the resulting output values
	newCenter = vecOutputPos;
	newIntegrationVelocity = vecOutputVelocity;

	return bHit;
}

//-----------------------------------------------------------------------------
// Name : EllipsoidIntersectScene ()
// Desc : Test for collision against the database using the ellipsoid specified
// Note : For ease of understanding, all variables used in this function which
//        begin with a lower case 'e' (i.e. eNormal) denote that the values
//        contained within it are described in ellipsoid space.
//-----------------------------------------------------------------------------
bool Collision::ellipsoidIntersectScene(const Vector3f& center, const Vector3f& radius, const Vector3f& velocity, CollIntersect intersections[], unsigned long& intersectionCount, bool inputEllipsoidSpace /* = false */, bool returnEllipsoidSpace /* = false */){
	Vector3f	  eCenter, eVelocity, eAdjust, vecEndPoint, invRadius;
	float         eInterval;
	unsigned long i;

	// calculate the reciprocal radius to prevent the many divides we would need otherwise
	invRadius = Vector3f(1.0f / radius[0], 1.0f / radius[1], 1.0f / radius[2]);

	// convert the values specified into ellipsoid space if required
	if (!inputEllipsoidSpace){
		eCenter = Vec3VecScale(center, invRadius);
		eVelocity = Vec3VecScale(velocity, invRadius);

	} // End if the input values were not in ellipsoid space
	else{
		eCenter = center;
		eVelocity = velocity;

	} // End if the input values are already in ellipsoid space
	
	  // reset ellipsoid space interval to maximum
	eInterval = 1.0f;

	// reset initial intersection count to 0 to save the caller having to do this.
	intersectionCount = 0;

	// perform the ellipsoid intersect test against our static scene
	ellipsoidIntersectBuffers(m_collVertices, m_collTriangles, eCenter, radius, invRadius, eVelocity, eInterval, intersections, intersectionCount);

	// if we were requested to return the values in normal space
	// then we must take the values back out of ellipsoid space here
	if (!returnEllipsoidSpace)
	{
		// for each intersection found
		for (i = 0; i < intersectionCount; ++i)
		{
			// transform the new center position and intersection point
			intersections[i].newCenter = Vec3VecScale(intersections[i].newCenter, radius);
			intersections[i].intersectPoint = Vec3VecScale(intersections[i].intersectPoint, radius);

			// transform the normal (again we do this in the opposite way to a coordinate)
			Vector3f normal = Vec3VecScale(intersections[i].intersectNormal, invRadius);
			Vector3f::Normalize(normal);

			// store the transformed normal
			intersections[i].intersectNormal = normal;
		} 
	} 

	// return hit.
	return (intersectionCount > 0);
}

unsigned long Collision::ellipsoidIntersectBuffers(std::vector<Vector3f>& vertBuffer, std::vector<CollTriangle>& triBuffer, const Vector3f& eCenter, const Vector3f& radius, const Vector3f& invRadius, const Vector3f& eVelocity, float& eInterval, CollIntersect intersections[], unsigned long& intersectionCount, Matrix4f* pTransform) {

	Vector3f	  ePoints[3], eNormal;
	Vector3f	  eIntersectNormal, eNewCenter;
	unsigned long counter, newIndex, firstIndex;
	bool		  addToList;
	
	// firstIndex tracks the first item to be added the intersection list.
	firstIndex = intersectionCount;

	// iterate through our triangle database
	std::vector<CollTriangle>::iterator Iterator = triBuffer.begin();
	for (counter = 0; Iterator != triBuffer.end(); ++Iterator, ++counter){

		// get the triangle descriptor
		CollTriangle * pTriangle = &(*Iterator);
		if (!pTriangle) continue;

		// requires transformation?
		if (pTransform){

			// get transformed points
			ePoints[0] = *pTransform * Vector4f(vertBuffer[pTriangle->indices[0]]);
			ePoints[1] = *pTransform * Vector4f(vertBuffer[pTriangle->indices[1]]);
			ePoints[2] = *pTransform * Vector4f(vertBuffer[pTriangle->indices[2]]);

			// get points and transform into ellipsoid space
			ePoints[0] = Vec3VecScale(ePoints[0], invRadius);
			ePoints[1] = Vec3VecScale(ePoints[1], invRadius);
			ePoints[2] = Vec3VecScale(ePoints[2], invRadius);

			// transform normal and normalize (Note how we do not use InvRadius for the normal)
			Vector4f normal = Vector4f(pTriangle->normal); normal[3] = 0.0f;
			eNormal = *pTransform * normal;
			eNormal = Vec3VecScale(eNormal, radius);
			Vector3f::Normalize(eNormal);

		} // end if requires transformation
		else{
			// get points and transform into ellipsoid space
			ePoints[0] = Vec3VecScale(vertBuffer[pTriangle->indices[0]], invRadius);
			ePoints[1] = Vec3VecScale(vertBuffer[pTriangle->indices[1]], invRadius);
			ePoints[2] = Vec3VecScale(vertBuffer[pTriangle->indices[2]], invRadius);

			// transform normal and normalize (Note how we do not use InvRadius for the normal)
			eNormal = Vec3VecScale(pTriangle->normal, radius);
			Vector3f::Normalize(eNormal);

		} // end if no transformation

		 // test for intersection with a unit sphere and the ellipsoid space triangle
		//if (SphereIntersectTriangle(eCenter, 1.0f, eVelocity, ePoints[0], ePoints[1], ePoints[2], eNormal, eInterval, eIntersectNormal)) {
		if (SphereIntersectTriangleBidirectional(eCenter, 1.0f, eVelocity, ePoints[0], ePoints[1], ePoints[2], eNormal, eInterval, eIntersectNormal)){
			// calculate our new sphere center at the point of intersection
			if (eInterval > 0)
				eNewCenter = eCenter + (eVelocity * eInterval);
			else
				eNewCenter = eCenter - (eIntersectNormal * eInterval);

			// where in the array should it go?
			addToList = false;
			if (intersectionCount == 0 || eInterval < intersections[0].interval){
				// we either have nothing in the array yet, or the new intersection is closer to us
				addToList = true;
				newIndex = 0;
				intersectionCount = 1;

				// reset, we've cleared the list
				firstIndex = 0;

			} // end if overwrite existing intersections
			else if (fabsf(eInterval - intersections[0].interval) < 1e-5f){
				// it has the same interval as those in our list already, append to 
				// the end unless we've already reached our limit
				if (intersectionCount < m_maxIntersections){
					addToList = true;
					newIndex = intersectionCount;
					intersectionCount++;

				} // end if we have room to store more

			} // end if the same interval

			  // add to the list?
			if (addToList){
				intersections[newIndex].interval = eInterval;
				intersections[newIndex].newCenter = eNewCenter + (eIntersectNormal * 1e-3f); // push back from the plane slightly to improve accuracy
				intersections[newIndex].intersectPoint = eNewCenter - eIntersectNormal;      // the intersection point on the surface of the sphere (and triangle)
				intersections[newIndex].intersectNormal = eIntersectNormal;
				intersections[newIndex].triangleIndex = counter;
			} // end if we are inserting in our list

		} // end if collided
	} // next triangle

	 // return hit.
	return firstIndex;
}


