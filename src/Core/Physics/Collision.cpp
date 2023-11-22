#include "../../include/Core/Physics/Collision.hpp"


bool CollisionCheckBroad(SpatialObject& own, SpatialObject& other)
{
    BoundingBox boxOwn = own.SO_rigidbody.boundbox;
    BoundingBox boxOther = other.SO_rigidbody.boundbox;

    return (boxOwn.min.x <= boxOther.max.x && boxOwn.max.x >= boxOther.min.x && boxOwn.min.y <= boxOther.max.y && boxOwn.max.y >= boxOther.min.y
    && boxOwn.min.z <= boxOther.max.z && boxOwn.max.z >= boxOther.min.z);
}

CollisionPoint::CollisionPoint()
{

}

CollisionPoint::CollisionPoint(glm::vec3 point, float dist)
{
    this->point = point;
    this->dist = dist;
}

CollisionPoint::CollisionPoint(glm::vec3 point, glm::vec3 normal, float dist)
{
    this->point = point;
    this->dist = dist;
	this->normal = normal;
}

CollisionPoint::~CollisionPoint()
{

}


//--------------------------------------------------------------
//--------------------------------------------------------------
//------------------ GJK + EPA(epa dont work) ------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

struct Simplex
{
    glm::vec3 a, b, c, d;
    int count;

    Simplex() 
    {
        a, b, c, d = glm::vec3(0.0f);
        count = 1;
    }
};

glm::vec3 GetSupportPoint(SpatialObject& object, glm::vec3 dir);
bool NextSimplex(Simplex& simplex, glm::vec3& direction);
bool SameLine(glm::vec3 dir, glm::vec3 ao);
bool Simplex2(Simplex& simplex, glm::vec3& direction);
bool Simplex3(Simplex& simplex, glm::vec3& direction);
bool Simplex4(Simplex& simplex, glm::vec3& direction);
CollisionPoint GetCollisionPoint(Simplex& a, SpatialObject& own, SpatialObject& other);

std::pair<bool, CollisionPoint> CollisionCheckNarrowGjk(SpatialObject& own, SpatialObject& other)
{
    glm::vec3 direction = glm::vec3(1.0f,0.0f,0.0f);
    Simplex simplex;
    glm::vec3 support;

    support = GetSupportPoint(own, direction) - GetSupportPoint(other, -direction);
    simplex.a = support;
    direction = -simplex.a;

    for (unsigned int i = 0; i < 64; i++)
    {
        simplex.d = simplex.c;
        simplex.c = simplex.b;
        simplex.b = simplex.a;
        simplex.count++;
        support = GetSupportPoint(own, direction) - GetSupportPoint(other, -direction);

        if(glm::dot(support, direction) < 0)
            return std::make_pair(false, CollisionPoint());

        simplex.a = support;
        if(NextSimplex(simplex, direction))
        {
            return std::make_pair(true, CollisionPoint());
        }
    }
    return std::make_pair(false, CollisionPoint());
}

//fix this giving a empty array
std::pair<std::vector<glm::vec4>, unsigned int> GetTriNormal(std::vector<glm::vec3>& polytope, std::vector<unsigned int>& faces)
{
    std::vector<glm::vec4> normals;
	unsigned int minTriangle = 0;
	float  minDistance = FLT_MAX;
    
	for (unsigned int i = 0; i < faces.size(); i += 3) 
    {
		glm::vec3 a = polytope[faces[i]];
		glm::vec3 b = polytope[faces[i + 1]];
		glm::vec3 c = polytope[faces[i + 2]];

		glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
		float distance = dot(normal, a);

		if (distance < 0) 
        {
			normal *= -1;
			distance *= -1;
		}

		normals.emplace_back(normal, distance);

		if (distance < minDistance) 
        {
			minTriangle = i / 3;
			minDistance = distance;
		}
	}

	return std::make_pair(normals, minDistance);
}

void AddIfUniqueEdge(std::vector<std::pair<unsigned int, unsigned int>>& edges, const std::vector<unsigned int>& faces, unsigned int a, unsigned int b)
{
	auto reverse = std::find(edges.begin(), edges.end(), std::make_pair(faces[b], faces[a]));
 
	if (reverse != edges.end()) 
		edges.erase(reverse);
	else 
		edges.emplace_back(faces[a], faces[b]);
}


CollisionPoint GetCollisionPoint(Simplex& a, SpatialObject& own, SpatialObject& other)
{
    std::vector<glm::vec3> polytope {a.a, a.b, a.c, a.d};
    std::vector<unsigned int> faces = 
	{
		0, 1, 2,
		0, 3, 1,
		0, 2, 3,
		1, 3, 2
	};

    auto [normals, minFace] = GetTriNormal(polytope, faces);

    glm::vec3 minNormal;
	float minDistance = FLT_MAX;
	unsigned int count = 0;

	while(minDistance == FLT_MAX)
	{
		//if(count >= 64)
		//	break;
		minNormal = normals[minFace];
		minDistance = normals[minFace].w;
 
		glm::vec3 support = GetSupportPoint(own, minNormal) - GetSupportPoint(other, -minNormal);
		float sDistance = dot(minNormal, support);
 
		if (abs(sDistance - minDistance) > 0.00001f)
        {
			minDistance = FLT_MAX;
            std::vector<std::pair<unsigned int, unsigned int>> uniqueEdges;

			for (unsigned int i = 0; i < normals.size(); i++)
            {
				if (SameLine(normals[i], support))
                {
					unsigned int f = i * 3;

					AddIfUniqueEdge(uniqueEdges, faces, f, f + 1);
					AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
					AddIfUniqueEdge(uniqueEdges, faces, f + 2, f);

					faces[f + 2] = faces.back(); faces.pop_back();
					faces[f + 1] = faces.back(); faces.pop_back();
					faces[f] = faces.back(); faces.pop_back();

					normals[i] = normals.back();
					normals.pop_back();

					i--;
				}
			}
            
            std::vector<unsigned int> newFaces;
			for (auto [edgeIndex1, edgeIndex2] : uniqueEdges)
            {
				newFaces.push_back(edgeIndex1);
				newFaces.push_back(edgeIndex2);
				newFaces.push_back(polytope.size());
			}
			 
			polytope.push_back(support);

			auto [newNormals, newMinFace] = GetTriNormal(polytope, newFaces);


			float oldMinDistance = FLT_MAX;
			for (unsigned int i = 0; i < normals.size(); i++)
            {
				if (normals[i].w < oldMinDistance) 
                {
					oldMinDistance = normals[i].w;
					minFace = i;
				}
			}
 
			if (newNormals[newMinFace].w < oldMinDistance)
            {
				minFace = newMinFace + normals.size();
			}
 
			faces.insert(faces.end(), newFaces.begin(), newFaces.end());
			normals.insert(normals.end(), newNormals.begin(), newNormals.end());
		}
		count++;
	}

	return CollisionPoint(glm::vec3(0), minNormal, minDistance + 0.00001f);
}

bool NextSimplex(Simplex& simplex, glm::vec3& direction)
{
    switch (simplex.count)
    {
    case 2:
        return Simplex2(simplex, direction);
        break;
    case 3:
        return Simplex3(simplex, direction);
        break;
    case 4:
        return Simplex4(simplex, direction);
        break;
    }
    return false;
}

bool Simplex2(Simplex& simplex, glm::vec3& direction)
{
    glm::vec3 ab = simplex.b - simplex.a;
    glm::vec3 ao = -simplex.a;

    if(SameLine(ab, ao))
    {
        direction = glm::cross(glm::cross(ab, ao), ab);
    }
    else
    {
        simplex.count = 1;
        direction = ao;
    }

    return false;
}

bool Simplex3(Simplex& simplex, glm::vec3& direction)
{
    glm::vec3 ab = simplex.b - simplex.a;
	glm::vec3 ac = simplex.c - simplex.a;
	glm::vec3 ao = -simplex.a;
 
	glm::vec3 abc = cross(ab, ac);
 
    if (SameLine(cross(abc, ac), ao)) 
    {
	    if (SameLine(ac, ao)) 
        {
			simplex.count = 2;
            simplex.b = simplex.c;
			direction = cross(cross(ac, ao), ac);
		}

		else 
        {
            simplex.count = 2;
			return Simplex2(simplex, direction);
		}
	}
 
	else 
    {
		if (SameLine(cross(ab, abc), ao)) 
        {
			simplex.count = 2;
			return Simplex2(simplex, direction);
		}

		else 
        {
			if (SameLine(abc, ao)) 
            {
				direction = abc;
			}

			else 
            {
				glm::vec3 temp1 = simplex.b;
                glm::vec3 temp2 = simplex.c;
                simplex.c = temp1;
                simplex.b = temp2;
				direction = -abc;
			}
		}
	}

	return false;
}

bool Simplex4(Simplex& simplex, glm::vec3& direction)
{

	glm::vec3 ab = simplex.b - simplex.a;
	glm::vec3 ac = simplex.c - simplex.a;
	glm::vec3 ad = simplex.d - simplex.a;
	glm::vec3 ao = -simplex.a;
 
	glm::vec3 abc = cross(ab, ac);
	glm::vec3 acd = cross(ac, ad);
	glm::vec3 adb = cross(ad, ab);
 
	if (SameLine(abc, ao)) {
		return Simplex3(simplex, direction);
	}
		
	if (SameLine(acd, ao)) {
        simplex.b = simplex.c;
        simplex.c = simplex.d;
		return Simplex3(simplex, direction);
	}
 
	if (SameLine(adb, ao)) {
        simplex.c = simplex.b;
        simplex.b = simplex.d;
		return Simplex3(simplex, direction);
	}
    
	return true;
}

glm::vec3 GetSupportPoint(SpatialObject& object, glm::vec3 dir)
{
    glm::vec3 maxP;
	float maxDist = FLT_MIN;
    for (unsigned int i = 0; i < object.SO_mesh.vertexes.size(); i++)
	{
		float distance = dot(object.SO_mesh.vertexes[i].position, dir);
		if(distance > maxDist)
		{
			maxDist = distance;
			maxP = object.SO_mesh.vertexes[i].position;
		}
	}

    return TransformVec4(glm::vec4(maxP, 1.0f), object.SO_mesh.modelMatrix);
}

bool SameLine(glm::vec3 dir, glm::vec3 ao)
{
    return dot(dir, ao) > 0;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//------------------ GJK + EPA(epa dont work) ------------------
//--------------------------------------------------------------
//--------------------------------------------------------------





//--------------------------------------------------------------
//--------------------------------------------------------------
//---------------------------- SAT -----------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

float TransformToAxis(SpatialObject& obj, glm::vec3 axis, glm::vec3 axisX, glm::vec3 axisY, glm::vec3 axisZ)
{
	glm::vec3 half = (obj.SO_rigidbody.oriBoundBox.maxOri);
    return
        half.x * fabs(glm::dot(axis, axisX)) +
        half.y * fabs(glm::dot(axis, axisY)) +
        half.z * fabs(glm::dot(axis, axisZ));
}

float PenetrationOnAxis(SpatialObject& one, SpatialObject& two, glm::vec3 axis, glm::vec3 toCentre,
	glm::vec3 b1AxisX, glm::vec3 b1AxisY, glm::vec3 b1AxisZ, glm::vec3 b2AxisX, glm::vec3 b2AxisY, glm::vec3 b2AxisZ)
{
    // Project the half-size of one onto axis
    float oneProject = TransformToAxis(one, axis, b1AxisX, b1AxisY, b1AxisZ);
    float twoProject = TransformToAxis(two, axis, b2AxisX, b2AxisY, b2AxisZ);

    // Project this onto the axis
    float distance = fabs(glm::dot(toCentre, axis));

    // Return the overlap (i.e. positive indicates
    // overlap, negative indicates separation).
    return oneProject + twoProject - distance;
}

bool GetSeparatingPlane(glm::vec3 toCenter, glm::vec3 axis, SpatialObject& obj1, SpatialObject& obj2, glm::vec3 b1AxisX, glm::vec3 b1AxisY, glm::vec3 b1AxisZ,
	glm::vec3 b2AxisX,glm::vec3 b2AxisY, glm::vec3 b2AxisZ, float& minPen, int& axisNum, int axisNumInt, glm::vec3& setMinPlane, int& smallestSingle) 
{
	//if true returned, we have no seperation
	//if false returned, we have seperation, exit the loop there is no collision
	// Make sure we have a normalized axis, and don't check almost parallel axes
	if (sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z) < 0.0001f)
		return true;
	//normalize the axis by dividing it by its lenth(or multiplying it by the inverse of its length)
	axis *= (1.0f / sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z));

	float penetration = PenetrationOnAxis(obj1, obj2, axis, toCenter, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ);
	
	//if the penetration is less than 0, the objects are seperated
	if (penetration < 0.0f)
		return false;
	
	if (penetration < minPen) 
	{
		minPen = penetration;
		axisNum = axisNumInt;
		setMinPlane = axis;
		if (axisNumInt < 6) 
			smallestSingle = axisNumInt;
	}

	return true;
}

//the function to detect collisions
//collision point may not be correct and collision normal may also need to be reversed(*-1.0f)
std::pair<bool, CollisionPoint> CollisionCheckNarrowSat(SpatialObject& obj1, SpatialObject& obj2)
{
	
	glm::vec3 contactPoint;
	float penetration = FLT_MAX; //set this to a really high value so that we assume there is no penetration depth
	int smallestCase;
	int smallestCaseSingleAxis;
	glm::vec3 collisionNormal;//the axis that seperates the 2 obbs, also the collision normal

	glm::vec3 half1 = (obj1.SO_rigidbody.oriBoundBox.maxOri) * 0.5f;
	
	//create the orientation axis for the first box(idk why the order is switched up but it is, it is switched up in rendering the same way as well)
	float deg2rad = 3.14159265358979323846f / 180.0f;
	float cosA = cos(obj1.SO_rigidbody.rotation.x * deg2rad);//pitch
	float cosB = cos(obj1.SO_rigidbody.rotation.y * deg2rad);//yaw
	float cosC = cos(obj1.SO_rigidbody.rotation.z * deg2rad);//roll
	float sinA = sin(obj1.SO_rigidbody.rotation.x * deg2rad);//pitch
	float sinB = sin(obj1.SO_rigidbody.rotation.y * deg2rad);//yaw
	float sinC = sin(obj1.SO_rigidbody.rotation.z * deg2rad);//roll
	
	glm::vec3 b1AxisX;
	b1AxisX.x = cosB * cosC;
	b1AxisX.y = sinA * sinB * cosC + cosA * sinC;
	b1AxisX.z = -1.0f * cosA * sinB * cosC + sinA * sinC;
	
	glm::vec3 b1AxisY;
	b1AxisY.x = -1.0f * cosB * sinC;
	b1AxisY.y = -1.0f * sinA * sinB * sinC + cosA * cosC;
	b1AxisY.z = cosA * sinB * sinC + sinA * cosC;
	
	glm::vec3 b1AxisZ;
	b1AxisZ.x = sinB;
	b1AxisZ.y = -1.0f * sinA * cosB;
	b1AxisZ.z = cosA * cosB;
	
	//create the orientation axis for the second box(idk why the order is switched up but it is, it is switched up in rendering the same way as well)
	cosA = cos(obj2.SO_rigidbody.rotation.x * deg2rad);//pitch
	cosB = cos(obj2.SO_rigidbody.rotation.y * deg2rad);//yaw
	cosC = cos(obj2.SO_rigidbody.rotation.z * deg2rad);//roll
	sinA = sin(obj2.SO_rigidbody.rotation.x * deg2rad);//pitch
	sinB = sin(obj2.SO_rigidbody.rotation.y * deg2rad);//yaw
	sinC = sin(obj2.SO_rigidbody.rotation.z * deg2rad);//roll
		
	glm::vec3 b2AxisX;
	b2AxisX.x = cosB * cosC;
	b2AxisX.y = sinA * sinB * cosC + cosA * sinC;
	b2AxisX.z = -1.0f * cosA * sinB * cosC + sinA * sinC;
	
	glm::vec3 b2AxisY;
	b2AxisY.x = -1.0f * cosB * sinC;
	b2AxisY.y = -1.0f * sinA * sinB * sinC + cosA * cosC;
	b2AxisY.z = cosA * sinB * sinC + sinA * cosC;
	
	glm::vec3 b2AxisZ;
	b2AxisZ.x = sinB;
	b2AxisZ.y = -1.0f * sinA * cosB;
	b2AxisZ.z = cosA * cosB;
	
	
	glm::vec3 toCenter;
	toCenter = obj1.SO_rigidbody.position - obj2.SO_rigidbody.position; //vector from centre 2 to centre 1
	
	if (!(GetSeparatingPlane(toCenter, b1AxisX, obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 0, collisionNormal, smallestCaseSingleAxis) &&
		GetSeparatingPlane(toCenter, b1AxisY, obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 1, collisionNormal, smallestCaseSingleAxis) &&
		GetSeparatingPlane(toCenter, b1AxisZ, obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 2, collisionNormal, smallestCaseSingleAxis) &&
		GetSeparatingPlane(toCenter, b2AxisX, obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 3, collisionNormal, smallestCaseSingleAxis) &&
		GetSeparatingPlane(toCenter, b2AxisY, obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 4, collisionNormal, smallestCaseSingleAxis) &&
		GetSeparatingPlane(toCenter, b2AxisZ, obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 5, collisionNormal, smallestCaseSingleAxis) &&
		GetSeparatingPlane(toCenter, glm::cross(b1AxisX, b2AxisX), obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 6, collisionNormal, smallestCaseSingleAxis) &&
		GetSeparatingPlane(toCenter, glm::cross(b1AxisX, b2AxisY), obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 7, collisionNormal, smallestCaseSingleAxis) &&
		GetSeparatingPlane(toCenter, glm::cross(b1AxisX, b2AxisZ), obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 8, collisionNormal, smallestCaseSingleAxis) &&
		GetSeparatingPlane(toCenter, glm::cross(b1AxisY, b2AxisX), obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 9, collisionNormal, smallestCaseSingleAxis) &&
		GetSeparatingPlane(toCenter, glm::cross(b1AxisY, b2AxisY), obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 10, collisionNormal, smallestCaseSingleAxis) &&
		GetSeparatingPlane(toCenter, glm::cross(b1AxisY, b2AxisZ), obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 11, collisionNormal, smallestCaseSingleAxis) &&
		GetSeparatingPlane(toCenter, glm::cross(b1AxisZ, b2AxisX), obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 12, collisionNormal, smallestCaseSingleAxis) &&
		GetSeparatingPlane(toCenter, glm::cross(b1AxisZ, b2AxisY), obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 13, collisionNormal, smallestCaseSingleAxis) &&
		GetSeparatingPlane(toCenter, glm::cross(b1AxisZ, b2AxisZ), obj1, obj2, b1AxisX, b1AxisY, b1AxisZ, b2AxisX, b2AxisY, b2AxisZ, penetration, smallestCase, 14, collisionNormal, smallestCaseSingleAxis))) 
	{
		//if this is true, there has been a seperating axis and the collision is false
		return std::make_pair(false, CollisionPoint());
	} 
	else 
	{
		//there is a collision, we now need to build the collision data
		
		if (smallestCase < 3) 
		{
			//collision along one of the major axis of box a
			//vertex of box two on a face of box one
			
			//first work out which face the normal is on and change it accordingly
			if (glm::dot(collisionNormal, toCenter) > 0.0f)
				collisionNormal = -collisionNormal;
			
			//now find the vertex
			glm::vec3 tempContactPoint = half1;

			if (glm::dot(b1AxisX, collisionNormal) < 0.0f) 
				tempContactPoint.x = -tempContactPoint.x;
			if (glm::dot(b1AxisY, collisionNormal) < 0.0f) 
				tempContactPoint.y = -tempContactPoint.y;
			if (glm::dot(b1AxisZ, collisionNormal) < 0.0f) 
				tempContactPoint.z = -tempContactPoint.z;
			
			//transform this point to box 2's rotation
			contactPoint.x = b1AxisX.x * tempContactPoint.x + b1AxisY.x * tempContactPoint.y + b1AxisZ.x * tempContactPoint.z + obj1.SO_rigidbody.position.x;
			contactPoint.y = b1AxisX.y * tempContactPoint.x + b1AxisY.y * tempContactPoint.y + b1AxisZ.y * tempContactPoint.z + obj1.SO_rigidbody.position.y;
			contactPoint.z = b1AxisX.z * tempContactPoint.x + b1AxisY.z * tempContactPoint.y + b1AxisZ.z * tempContactPoint.z + obj1.SO_rigidbody.position.z;
			
		} 
		else if (smallestCase < 6) 
		{
			//collision along one of the major axis of box b
			//vertex of box one on a face of box two
		
			//first work out which face the normal is on and change it accordingly
			if (glm::dot(collisionNormal, toCenter) > 0.0f)
				collisionNormal = -collisionNormal;
			
			//now find the vertex
			glm::vec3 tempContactPoint = half1;
			if (glm::dot(b2AxisX, collisionNormal) < 0.0f) 
				tempContactPoint.x = -tempContactPoint.x;
			if (glm::dot(b2AxisY, collisionNormal) < 0.0f) 
				tempContactPoint.y = -tempContactPoint.y;
			if (glm::dot(b2AxisZ, collisionNormal) < 0.0f) 
				tempContactPoint.z = -tempContactPoint.z;
			
			//transform this point to box 1's rotation
			contactPoint.x = b2AxisX.x * tempContactPoint.x + b2AxisY.x * tempContactPoint.y + b2AxisZ.x * tempContactPoint.z + obj2.SO_rigidbody.position.x;
			contactPoint.y = b2AxisX.y * tempContactPoint.x + b2AxisY.y * tempContactPoint.y + b2AxisZ.y * tempContactPoint.z + obj2.SO_rigidbody.position.y;
			contactPoint.z = b2AxisX.z * tempContactPoint.x + b2AxisY.z * tempContactPoint.y + b2AxisZ.z * tempContactPoint.z + obj2.SO_rigidbody.position.z;
			
		}
		
		return std::make_pair(true, CollisionPoint(contactPoint, collisionNormal, penetration + 0.0001f));
	}
	
	//should never reach this point
	return std::make_pair(false, CollisionPoint());
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//---------------------------- SAT -----------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

