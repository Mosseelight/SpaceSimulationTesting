#include "../../include/Core/Math.hpp"

// matrix math in coloum major for faster speeds

float* ConvertVec4(glm::vec4 vec)
{
    float array[4] __attribute__ ((aligned(16)));
    array[0] = vec.x;
    array[1] = vec.y;
    array[2] = vec.z;
    array[3] = vec.w;
    return array;
}

glm::vec4 TransformVec4(glm::vec4 vec, glm::mat4& mat)
{
    /*__m128 x = _mm_load_ps(glm::value_ptr(vec));
    __m128 a = _mm_load_ps(ConvertVec4(mat[0]));
    __m128 b = _mm_load_ps(ConvertVec4(mat[1]));
    __m128 c = _mm_load_ps(ConvertVec4(mat[2]));
    __m128 d = _mm_load_ps(ConvertVec4(mat[3]));*/

    // this solution seems better for performance right now because ConvertVec4 is slow but I dont know how it compares to
    // loadu and load if its not aligned
    __m128 x = _mm_load_ps(glm::value_ptr(vec));
    __m128 a = _mm_loadu_ps(glm::value_ptr(mat[0]));
    __m128 b = _mm_loadu_ps(glm::value_ptr(mat[1]));
    __m128 c = _mm_loadu_ps(glm::value_ptr(mat[2]));
    __m128 d = _mm_loadu_ps(glm::value_ptr(mat[3]));

    _MM_TRANSPOSE4_PS(a,b,c,d);

    __m128 sum_01 = _mm_hadd_ps(_mm_mul_ps(x, a), _mm_mul_ps(x, b));
    __m128 sum_23 = _mm_hadd_ps(_mm_mul_ps(x, c), _mm_mul_ps(x, d));
    __m128 result = _mm_hadd_ps(sum_01, sum_23);

    _mm_storeu_ps(glm::value_ptr(vec), result);

    return vec;
}