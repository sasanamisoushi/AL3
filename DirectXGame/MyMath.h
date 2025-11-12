#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;

namespace KamataEngine {

// 単項演算子オーバーロード
Vector3 operator+(const Vector3& v);
Vector3 operator-(const Vector3& v);
Vector3 operator/(const Vector3& v, float s);

Vector3& operator+=(Vector3& lhv, const Vector3& rhv);
Vector3& operator-=(Vector3& lhv, const Vector3& rhv);
Vector3& operator*=(Vector3& v, float s);
Vector3& operator/=(Vector3& v, float s);

Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator*(const Vector3& v1, const float f);
Vector3 operator-(const Vector3& lhv, const Vector3& rhv);
} // namespace KamataEngine

// 平行移動
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// 拡大縮小
Matrix4x4 MkeScaleMatrix(const Vector3& scale);

// X軸回転行列
Matrix4x4 MakeRoteXMatrix(float radian);

// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);

// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

// 積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

// アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

// 線形保管
float Lerp(float a, float b, float t);

// イーズインアウト
float EaseInOut(float a, float b, float t);

// イーズアウト
float EaseOut(float x1, float x2, float t);

// イーズイン
float EaseIn(float x1, float x2, float t);

// 02_06のスライド24枚目のLerp関数
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

// 角度差を考慮した線形補間
float LerpAngle(float a, float b, float t);

// AABB関数
struct AABB {
	Vector3 min;
	Vector3 max;
};

bool IsCollision(const AABB& aabb1, const AABB& aabb2);

// トランスフォーム
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix);

// ワールドトランスフォーム更新
void WorldTransformUpdate(KamataEngine::WorldTransform& worldTransform);

inline float ToRadians(float degrees) { return degrees * (3.1415f / 180.0f); }
inline float ToDegrees(float radians) { return radians * (180.0f / 3.1415f); }

float Length(const Vector3& v);

Vector3 Normalize(const Vector3& v);