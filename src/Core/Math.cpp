#include "../../include/Core/Math.hpp"

// matrix math in coloum major for faster speeds

glm::vec4 TransformVec4(glm::vec4 vec, glm::mat4& mat)
{
    __m128 x = _mm_loadu_ps(glm::value_ptr(vec));
    __m128 a = _mm_loadu_ps(glm::value_ptr(mat[0]));
    __m128 b = _mm_loadu_ps(glm::value_ptr(mat[1]));
    __m128 c = _mm_loadu_ps(glm::value_ptr(mat[2]));
    __m128 d = _mm_loadu_ps(glm::value_ptr(mat[3]));

    _MM_TRANSPOSE4_PS(a,b,c,d);

    __m128 p1 = _mm_mul_ps(x, a);
    __m128 p2 = _mm_mul_ps(x, b);
    __m128 p3 = _mm_mul_ps(x, c);
    __m128 p4 = _mm_mul_ps(x, d);

    __m128 sum_01 = _mm_hadd_ps(p1, p2);
    __m128 sum_23 = _mm_hadd_ps(p3, p4);
    __m128 result = _mm_hadd_ps(sum_01, sum_23);

    _mm_storeu_ps(glm::value_ptr(vec), result);

    return vec;
}