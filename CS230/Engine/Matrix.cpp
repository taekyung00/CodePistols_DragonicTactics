/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Matrix.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    April 1, 2025
*/

#include "Matrix.h"

Math::TransformationMatrix::TransformationMatrix(){
	Reset();
}


Math::TransformationMatrix Math::TransformationMatrix::operator * (TransformationMatrix m) const {
	TransformationMatrix result;

	result.matrix[0][0] = matrix[0][0] * m[0][0] + matrix[0][1] * m[1][0] + matrix[0][2] * m[2][0];
	result.matrix[0][1] = matrix[0][0] * m[0][1] + matrix[0][1] * m[1][1] + matrix[0][2] * m[2][1];
	result.matrix[0][2] = matrix[0][0] * m[0][2] + matrix[0][1] * m[1][2] + matrix[0][2] * m[2][2];
	result.matrix[1][0] = matrix[1][0] * m[0][0] + matrix[1][1] * m[1][0] + matrix[1][2] * m[2][0];
	result.matrix[1][1] = matrix[1][0] * m[0][1] + matrix[1][1] * m[1][1] + matrix[1][2] * m[2][1];
	result.matrix[1][2] = matrix[1][0] * m[0][2] + matrix[1][1] * m[1][2] + matrix[1][2] * m[2][2];
	result.matrix[2][0] = matrix[2][0] * m[0][0] + matrix[2][1] * m[1][0] + matrix[2][2] * m[2][0];
	result.matrix[2][1] = matrix[2][0] * m[0][1] + matrix[2][1] * m[1][1] + matrix[2][2] * m[2][1];
	result.matrix[2][2] = matrix[2][0] * m[0][2] + matrix[2][1] * m[1][2] + matrix[2][2] * m[2][2];

	return result;
}

Math::TransformationMatrix& Math::TransformationMatrix::operator *= (Math::TransformationMatrix m) {
	(*this) = (*this) * m;
	return (*this);
}

Math::TransformationMatrix Math::TransformationMatrix::operator*(double scale) const
{	
	TransformationMatrix result;
	result.matrix[0][0] *= scale;
	result.matrix[0][1] *= scale;
	result.matrix[0][2] *= scale;
	result.matrix[1][0] *= scale;
	result.matrix[1][1] *= scale;
	result.matrix[1][2] *= scale;
	result.matrix[2][0] *= scale;
	result.matrix[2][1] *= scale;
	result.matrix[2][2] *= scale;
	return result;
}

Math::TransformationMatrix& Math::TransformationMatrix::operator*=(double scale)
{
	(*this) = (*this) * scale;
	return (*this);
}

Math::vec2 Math::TransformationMatrix::operator * (vec2 v) const {
	Math::vec2 result;
	result.x = matrix[0][0] * v.x + matrix[0][1] * v.y + matrix[0][2];
	result.y = matrix[1][0] * v.x + matrix[1][1] * v.y + matrix[1][2];
	return result;
}

void Math::TransformationMatrix::Reset() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			matrix[i][j] = 0;
		}
	}
	matrix[0][0] = 1.0;
	matrix[1][1] = 1.0;
	matrix[2][2] = 1.0;
}

Math::TranslationMatrix::TranslationMatrix(ivec2 translate){
	Reset();
	matrix[0][2] = translate.x;
	matrix[1][2] = translate.y;
}

Math::TranslationMatrix::TranslationMatrix(vec2 translate)
{
	Reset();
	matrix[0][2] = translate.x;
	matrix[1][2] = translate.y;
}

Math::ScaleMatrix::ScaleMatrix(double scale)
{
	Reset();
	matrix[0][0] *= scale;
	matrix[1][1] *= scale;
}

Math::ScaleMatrix::ScaleMatrix(vec2 scale)
{
	Reset();
	matrix[0][0] *= scale.x;
	matrix[1][1] *= scale.y;
}

Math::RotationMatrix::RotationMatrix(double theta)
{
	Reset();
	matrix[0][0] = std::cos(theta);
	matrix[0][1] = -std::sin(theta);
	matrix[1][0] = std::sin(theta);
	matrix[1][1] = std::cos(theta);
}
