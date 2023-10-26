#include "../../include/Core/Physics/Collision.hpp"


bool CollisionCheckBroad(SpatialObject& own, SpatialObject& other)
{
    BoundingBox boxOwn = own.SO_rigidbody.boundbox;
    BoundingBox boxOther = other.SO_rigidbody.boundbox;

    return (boxOwn.min.x <= boxOther.max.x && boxOwn.max.x >= boxOther.min.x && boxOwn.min.y <= boxOther.max.y && boxOwn.max.y >= boxOther.min.y
    && boxOwn.min.z <= boxOther.max.z && boxOwn.max.z >= boxOther.min.z);
}

bool CollisionCheckBroader(SpatialObject& own, SpatialObject& other)
{
    float dist1 = glm::distance(own.SO_rigidbody.position, own.SO_rigidbody.boundbox.max);
    float dist2 = glm::distance(other.SO_rigidbody.position, other.SO_rigidbody.boundbox.max);

    if(dist1 + dist2 > glm::distance(own.SO_rigidbody.position, other.SO_rigidbody.position))
        return true;
    return false;
}


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

CollisionPoint::CollisionPoint()
{

}

CollisionPoint::CollisionPoint(glm::vec3 point, float dist)
{
    this->point = point;
    this->dist = dist;
}

CollisionPoint::~CollisionPoint()
{

}

glm::vec3 GetSupportPoint(SpatialObject& object, glm::vec3 dir);
bool NextSimplex(Simplex& simplex, glm::vec3& direction);
bool SameLine(glm::vec3 dir, glm::vec3 ao);
bool Simplex2(Simplex& simplex, glm::vec3& direction);
bool Simplex3(Simplex& simplex, glm::vec3& direction);
bool Simplex4(Simplex& simplex, glm::vec3& direction);
CollisionPoint GetCollisionPoint(Simplex& a, SpatialObject& own, SpatialObject& other);

std::pair<bool, CollisionPoint> CollisionCheckNarrow(SpatialObject& own, SpatialObject& other)
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
            return std::make_pair(true, GetCollisionPoint(simplex, own, other));
        }
    }
    return std::make_pair(false, CollisionPoint());
}

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
			normal   *= -1;
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
    {
		edges.erase(reverse);
	}
 
	else 
    {
		edges.emplace_back(faces[a], faces[b]);
	}
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
		if(count >= 64)
			break;
		minNormal = normals[minFace];
		minDistance = normals[minFace].w;
 
		glm::vec3 support = GetSupportPoint(own, minNormal) - GetSupportPoint(other, -minNormal);
		float sDistance = dot(minNormal, support);
 
		if (abs(sDistance - minDistance) > 0.001f)
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
	
	return CollisionPoint(minNormal, minDistance + 0.001f);
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
	glm::vec3 ao =   - simplex.a;
 
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
	glm::vec3 ao =   - simplex.a;
 
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

