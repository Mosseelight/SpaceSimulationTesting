#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <pmmintrin.h>

glm::vec4 TransformVec4(glm::vec4 vec, glm::mat4& mat);
