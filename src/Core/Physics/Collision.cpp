#include "../../include/Core/Physics/Collision.hpp"

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

glm::vec3 GetSupportPoint(SpatialObject object, glm::vec3 dir);
bool NextSimplex(Simplex& simplex, glm::vec3& direction);
bool SameLine(glm::vec3 dir, glm::vec3 ao);
bool Simplex2(Simplex& simplex, glm::vec3& direction);
bool Simplex3(Simplex& simplex, glm::vec3& direction);
bool Simplex4(Simplex& simplex, glm::vec3& direction);

bool CollisionCheckNarrow(SpatialObject own, SpatialObject other)
{
    glm::vec3 direction = glm::vec3(1.0f,0.0f,0.0f);
    Simplex simplex;

    glm::vec3 support = GetSupportPoint(own, direction) - GetSupportPoint(other, -direction);
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
            return false;

        simplex.a = support;

        if(NextSimplex(simplex, direction))
        {
            return true;
        }
    }
    return false;
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

glm::vec3 GetSupportPoint(SpatialObject object, glm::vec3 dir)
{
    glm::vec3 maxP;
    float maxDist = FLT_MIN;
    dir = glm::vec3(glm::inverse(object.SO_mesh.GetModelMat()) * glm::vec4{dir, 0.0f});
    for (unsigned int i = 0; i < object.SO_mesh.vertexes.size(); i++)
    {
        float distance = dot(object.SO_mesh.vertexes[i].position, dir);
        if(distance > maxDist)
        {
            maxDist = distance;
            maxP = object.SO_mesh.vertexes[i].position;
        }
    }
    return glm::vec3(object.SO_mesh.GetModelMat() * glm::vec4(maxP, 1.0f));
}

bool SameLine(glm::vec3 dir, glm::vec3 ao)
{
    return dot(dir, ao) > 0;
}

