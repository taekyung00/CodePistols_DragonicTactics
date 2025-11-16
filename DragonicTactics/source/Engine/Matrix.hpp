/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Taekyung ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Vec2.hpp"

namespace Math
{
    class TransformationMatrix
    {
    public:
        TransformationMatrix();

        double* operator[](int index);

        const double* operator[](int index) const;

        TransformationMatrix  operator*(TransformationMatrix m) const;
        
        TransformationMatrix& operator*=(TransformationMatrix m);
        vec2                  operator*(vec2 v) const;
        vec3                  operator*(vec3 v) const;
        void                  Reset();

        

    protected:
        double matrix[3][3];
    };

    class TranslationMatrix : public TransformationMatrix
    {
    public:
        TranslationMatrix(ivec2 translate);
        TranslationMatrix(vec2 translate);
    };

    class RotationMatrix : public TransformationMatrix
    {
    public:
        RotationMatrix(double theta);
    };

    class ScaleMatrix : public TransformationMatrix
    {
    public:
        ScaleMatrix(double scale);
        ScaleMatrix(vec2 scale);
    };

    inline static bool IsIdentityMatrix(const Math::TransformationMatrix& mat)
    {
        return mat[0][0] == 1.0 && mat[0][1] == 0.0 && mat[0][2] == 0.0 && mat[1][0] == 0.0 && mat[1][1] == 1.0 && mat[1][2] == 0.0 && mat[2][0] == 0.0 && mat[2][1] == 0.0 && mat[2][2] == 1.0;
    }
}
