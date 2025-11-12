#include <cmath>
#define _USE_MATH_DEFINES
#include <algorithm>

#include "MyMath.h"
#include <numbers>

namespace KamataEngine {

// 単項演算子オーバーロード
Vector3 operator+(const Vector3& v) { return v; }
Vector3 operator-(const Vector3& v) { return Vector3(-v.x, -v.y, -v.z); }

Vector3& operator+=(Vector3& lhv, const Vector3& rhv) {
	lhv.x += rhv.x;
	lhv.y += rhv.y;
	lhv.z += rhv.z;
	return lhv;
}

Vector3& operator-=(Vector3& lhv, const Vector3& rhv) {
	lhv.x -= rhv.x;
	lhv.y -= rhv.y;
	lhv.z -= rhv.z;
	return lhv;
}

Vector3& operator*=(Vector3& v, float s) {
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}

Vector3& operator/=(Vector3& v, float s) {
	v.x /= s;
	v.y /= s;
	v.z /= s;
	return v;
}

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	Vector3 temp(v1);
	return temp += v2;
}

Vector3 operator*(const Vector3& v1, const float f) {
	Vector3 temp(v1);
	return temp *= f;
}

Vector3 operator-(const Vector3& lhv, const Vector3& rhv) { return {lhv.x - rhv.x, lhv.y - rhv.y, lhv.z - rhv.z}; }

Vector3 operator/(const Vector3& v, float s) { return Vector3{v.x / s, v.y / s, v.z / s}; }

} // namespace KamataEngine
// 平行移動
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, translate.x, translate.y, translate.z, 1};
	return result;
}

// 拡大縮小
Matrix4x4 MkeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result = {scale.x, 0, 0, 0, 0, scale.y, 0, 0, 0, 0, scale.z, 0, 0, 0, 0, 1};
	return result;
}

// X軸回転行列
Matrix4x4 MakeRoteXMatrix(float radian) {
	Matrix4x4 result = {
	    1, 0, 0, 0, 0, std::cosf(radian), std::sinf(radian), 0, 0, -std::sinf(radian), std::cosf(radian), 0, 0, 0, 0, 1,
	};
	return result;
}

// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result = {
	    std::cosf(radian), 0, -std::sinf(radian), 0, 0, 1, 0, 0, std::sinf(radian), 0, std::cosf(radian), 0, 0, 0, 0, 1,
	};
	return result;
}

// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result = {
	    std::cosf(radian), std::sinf(radian), 0, 0, -std::sinf(radian), std::cosf(radian), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
	};
	return result;
}

// 積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

// アフィン変換
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 rot = Multiply(Multiply(MakeRoteXMatrix(rotate.x), MakeRotateYMatrix(rotate.y)), MakeRotateZMatrix(rotate.z));
	Matrix4x4 result = Multiply(Multiply(MkeScaleMatrix(scale), rot), MakeTranslateMatrix(translate));

	return result;
}

float Lerp(float a, float b, float t) { return (1.0f - t) * a + t * b; }

// イーズインアウト
float EaseInOut(float a, float b, float t) {
	float easedT = -(std::cosf(std::numbers::pi_v<float> * t) - 1.0f) / 2.0f;
	return Lerp(a, b, easedT);
}

float EaseOut(float x1, float x2, float t) {
	float easedT = 1.0f - std::powf(1.0f - t, 3.0f);

	return Lerp(x1, x2, easedT);
}

float EaseIn(float x1, float x2, float t) {
	float easedT = t * t;

	return Lerp(x1, x2, easedT);
}

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) { return Vector3(Lerp(v1.x, v2.x, t), Lerp(v1.y, v2.y, t), Lerp(v1.z, v2.z, t)); }

float LerpAngle(float a, float b, float t) {
	float diff = b - a;
	while (diff > std::numbers::pi_v<float>)
		diff -= std::numbers::pi_v<float> * 2.0f;
	while (diff < -std::numbers::pi_v<float>)
		diff += std::numbers::pi_v<float> * 2.0f;
	return a + diff * t;
}

bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	return (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && // x軸
	       (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && // y軸
	       (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z);   // z軸
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	float x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
	float y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
	float z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];

	if (w != 0.0f) {
		x /= w;
		y /= w;
		z /= w;
	}

	return {x, y, z};
}

Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix) {
	float x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0];
	float y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1];
	float z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2];

	return {x, y, z};
}

void WorldTransformUpdate(WorldTransform& worldTransform) {
	Matrix4x4 affin_mat = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
	worldTransform.matWorld_ = affin_mat;

	// 定数バッファに転送する
	worldTransform.TransferMatrix();
}

float Length(const Vector3& v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	if (len == 0.0f) {
		return {0.0f, 0.0f, 0.0f};
	}
	return {v.x / len, v.y / len, v.z / len};
}
