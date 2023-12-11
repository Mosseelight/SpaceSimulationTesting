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
	normal = glm::vec3(0.0f);
	point = glm::vec3(0.0f);
	dist = 0.0f;
}

CollisionPoint::CollisionPoint(glm::vec3 normal, float dist)
{
    this->normal = normal;
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
//------------------------- Libccd MPR -------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

void support(const void *obj, const ccd_vec3_t *dir, ccd_vec3_t *vec);
void center(const void *obj, ccd_vec3_t *center);

std::pair<bool, CollisionPoint> CollisionCheckNarrowGjk(SpatialObject& own, SpatialObject& other)
{
    ccd_t ccd;
	CCD_INIT(&ccd);

	ccd.support1 = support;
    ccd.support2 = support;
	ccd.center1 = center;
    ccd.center2 = center;
    ccd.max_iterations = 500;
    ccd.epa_tolerance  = 0.00001;

    ccd_real_t depth;
    ccd_vec3_t dir, pos;
    int intersect = ccdMPRPenetration(&own, &other, &ccd, &depth, &dir, &pos);
	if(intersect == -1)
	{
		return std::make_pair(false, CollisionPoint());
	}
	return std::make_pair(true, CollisionPoint(glm::vec3(pos.v[0], pos.v[1], pos.v[2]), glm::vec3(dir.v[0], dir.v[1], dir.v[2]), static_cast<double>(depth)));
}

void center(const void *obj, ccd_vec3_t *center)
{
    SpatialObject *_obj = (SpatialObject *)obj;
	center->v[0] = _obj->SO_mesh.position.x;
	center->v[1] = _obj->SO_mesh.position.y;
	center->v[2] = _obj->SO_mesh.position.z;
}

void support(const void *obj, const ccd_vec3_t *dir, ccd_vec3_t *vec)
{
	SpatialObject *_obj = (SpatialObject *)obj;
    glm::vec3 maxP;
	double maxDist = DBL_MIN;
	glm::vec3 dirGlm(dir->v[0], dir->v[1], dir->v[2]);
	glm::mat4 mat = glm::inverse(_obj->SO_mesh.rotMatrix);
	dirGlm = TransformVec4(glm::vec4(dirGlm, 1.0f), mat);
    for (unsigned int i = 0; i < _obj->SO_mesh.vertexes.size(); i++)
	{
		double distance = glm::dot(_obj->SO_mesh.vertexes[i].position, dirGlm);
		if(distance > maxDist)
		{
			maxDist = distance;
			maxP = _obj->SO_mesh.vertexes[i].position;
		}
	}

	maxP = TransformVec4(glm::vec4(maxP, 1.0f), _obj->SO_mesh.modelMatrix);
    vec->v[0] = maxP.x;
	vec->v[1] = maxP.y;
	vec->v[2] = maxP.z;
}


//--------------------------------------------------------------
//--------------------------------------------------------------
//------------------------- Libccd MPR -------------------------
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
std::pair<bool, CollisionPoint> CollisionCheckNarrowSat(SpatialObject& obj2, SpatialObject& obj1)
{
	
	glm::vec3 contactPoint;
	float penetration = FLT_MAX; //set this to a really high value so that we assume there is no penetration depth
	int smallestCase;
	int smallestCaseSingleAxis;
	glm::vec3 collisionNormal;//the axis that seperates the 2 obbs, also the collision normal

	glm::vec3 half1 = (obj1.SO_rigidbody.position - obj1.SO_rigidbody.boundbox.max);
	glm::vec3 half2 = (obj2.SO_rigidbody.position - obj2.SO_rigidbody.boundbox.max);
	
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
			glm::vec3 tempContactPoint = half2;

			if (glm::dot(b2AxisX, collisionNormal) < 0.0f) 
				tempContactPoint.x = -tempContactPoint.x;
			if (glm::dot(b2AxisY, collisionNormal) < 0.0f) 
				tempContactPoint.y = -tempContactPoint.y;
			if (glm::dot(b2AxisZ, collisionNormal) < 0.0f) 
				tempContactPoint.z = -tempContactPoint.z;
			
			//transform this point to box 2's rotation
			contactPoint.x = b2AxisX.x * tempContactPoint.x + b2AxisY.x * tempContactPoint.y + b2AxisZ.x * tempContactPoint.z + obj2.SO_rigidbody.position.x;
			contactPoint.y = b2AxisX.y * tempContactPoint.x + b2AxisY.y * tempContactPoint.y + b2AxisZ.y * tempContactPoint.z + obj2.SO_rigidbody.position.y;
			contactPoint.z = b2AxisX.z * tempContactPoint.x + b2AxisY.z * tempContactPoint.y + b2AxisZ.z * tempContactPoint.z + obj2.SO_rigidbody.position.z;
			
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
			if (glm::dot(b1AxisX, collisionNormal) < 0.0f) 
				tempContactPoint.x = -tempContactPoint.x;
			if (glm::dot(b1AxisY, collisionNormal) < 0.0f) 
				tempContactPoint.y = -tempContactPoint.y;
			if (glm::dot(b1AxisZ, collisionNormal) < 0.0f) 
				tempContactPoint.z = -tempContactPoint.z;
			
			//transform this point to box 1's rotation
			contactPoint.x = b1AxisX.x * tempContactPoint.x + b1AxisY.x * tempContactPoint.y + b1AxisZ.x * tempContactPoint.z + obj1.SO_rigidbody.position.x;
			contactPoint.y = b1AxisX.y * tempContactPoint.x + b1AxisY.y * tempContactPoint.y + b1AxisZ.y * tempContactPoint.z + obj1.SO_rigidbody.position.y;
			contactPoint.z = b1AxisX.z * tempContactPoint.x + b1AxisY.z * tempContactPoint.y + b1AxisZ.z * tempContactPoint.z + obj1.SO_rigidbody.position.z;
		}
		else
		{
			smallestCase -= 6;
			int oneAxisIndex = smallestCase / 3;
			int twoAxisIndex = smallestCase % 3;
			glm::vec3 oneAxis;
			if (oneAxisIndex == 0) 
			{
				oneAxis = b1AxisX;
			} 
			else if (oneAxisIndex == 1) 
			{
				oneAxis = b1AxisY;
			} 
			else 
			{
				oneAxis = b1AxisZ;
			}
			glm::vec3 twoAxis;
			if (twoAxisIndex == 0) 
			{
				twoAxis = b2AxisX;
			} 
			else if (twoAxisIndex == 1) 
			{
				twoAxis = b2AxisY;
			} 
			else 
			{
				twoAxis = b2AxisZ;
			}
			glm::vec3 axis = glm::cross(oneAxis, twoAxis);
			axis *= sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);//normalize the axis
			
			//the axis should point from box one to box 2
			if (glm::dot(axis, toCenter) > 0) 
				axis *= -1;
			
			//find out which edges to use
			glm::vec3 ptOnOneEdge = half1;
			glm::vec3 ptOnTwoEdge = half2;
			
			if (0 == oneAxisIndex) 
				ptOnOneEdge.x = 0;
			else if (glm::dot(b1AxisX, axis) > 0) 
				ptOnOneEdge.x = -ptOnOneEdge.x;
			if (1 == oneAxisIndex) 
				ptOnOneEdge.y = 0;
			else if (glm::dot(b1AxisY, axis) > 0) 
				ptOnOneEdge.y = -ptOnOneEdge.y;
			if (2 == oneAxisIndex) 
				ptOnOneEdge.z = 0;
			else if (glm::dot(b1AxisZ, axis) > 0) 
				ptOnOneEdge.z = -ptOnOneEdge.z;
			
			if (0 == twoAxisIndex) 
				ptOnTwoEdge.x = 0;
			else if (glm::dot(b2AxisX, axis) > 0) 
				ptOnTwoEdge.x = -ptOnTwoEdge.x;
			if (1 == twoAxisIndex) { ptOnTwoEdge.y = 0; }
			else if (glm::dot(b2AxisY, axis) > 0) 
				ptOnTwoEdge.y = -ptOnTwoEdge.y;	
			if (2 == twoAxisIndex) 
				ptOnTwoEdge.z = 0;
			else if (glm::dot(b2AxisZ, axis) > 0) 
				ptOnTwoEdge.z = -ptOnTwoEdge.z;
			
			//scale them in reverse to fix some issues
			//ptOnOneEdge = scaleVec3(ptOnOneEdge, -1.0);
			//ptOnTwoEdge = scaleVec3(ptOnTwoEdge, -1.0);
			
			//move them into world coordinates(add the obb centre)
			ptOnOneEdge += obj1.SO_mesh.position;
			ptOnTwoEdge += obj2.SO_mesh.position;
			
			//if this is true and the contact point is outside the edge(edge-face contact), we use one's midpoint, otherwise two's
			bool useOne = smallestCaseSingleAxis > 2;
			
			//find the final collision point
			//create some variables
			//these variables are from the cycloneEngine code, so I dont know exactly what they mean but they work
			glm::vec3 toSt, cOne, cTwo;
			float dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo, denom, mua, mub;
			//set some of the variables
			smOne = sqrt(oneAxis.x * oneAxis.x + oneAxis.y * oneAxis.y + oneAxis.z * oneAxis.z);
			smTwo = sqrt(twoAxis.x * twoAxis.x + twoAxis.y * twoAxis.y + twoAxis.z * twoAxis.z);
			dpOneTwo = glm::dot(twoAxis, oneAxis);
			toSt = ptOnOneEdge -ptOnTwoEdge;
			dpStaOne = glm::dot(oneAxis, toSt);
			dpStaTwo = glm::dot(twoAxis, toSt);
			denom = smOne * smTwo - dpOneTwo * dpOneTwo;
			//now break off into different cases
			if (fabs(denom) < 0.0001) 
			{
				//zero denominator indicates parallel lines
				if (useOne == true) 
				{
					contactPoint = ptOnOneEdge;
				} 
				else 
				{
					contactPoint = ptOnTwoEdge;
				}
				
			} 
			else 
			{
				
				mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
				mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;
				
				float oneSize;
				if (oneAxisIndex == 0) 
				{
					oneSize = half1.x;
				} 
				else if (oneAxisIndex == 1) 
				{
					oneSize = half1.y;
				} 
				else 
				{
					oneSize = half1.z;
				}
				float twoSize;
				if (twoAxisIndex == 0) 
				{

					twoSize = half2.x;
				} 
				else if (twoAxisIndex == 1) 
				{
					twoSize = half2.y;
				} 
				else 
				{
					twoSize = half2.z;
				}
				
				if (mua > oneSize || mua < -oneSize || mub > twoSize || mub < -twoSize) 
				{
					if (useOne == true) 
					{
						contactPoint = ptOnOneEdge;
					} 
					else 
					{
						contactPoint = ptOnTwoEdge;
					}
				} 
				else 
				{
					cOne = (ptOnOneEdge + oneAxis) * mua;
					cTwo = (ptOnTwoEdge + twoAxis) * mub;
					
					contactPoint = (cOne * 0.5f) + (cTwo * 0.5f);
				}
				
			}
		}
		
		return std::make_pair(true, CollisionPoint(contactPoint, -collisionNormal, penetration + 0.0001f));
	}
	
	//should never reach this point
	return std::make_pair(false, CollisionPoint());
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//---------------------------- SAT -----------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

