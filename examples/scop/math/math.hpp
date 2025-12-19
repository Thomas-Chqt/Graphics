/*
 * ---------------------------------------------------
 * math.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/12/05 16:39:14
 * ---------------------------------------------------
 */

#ifndef MATH_HPP
#define MATH_HPP

#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numbers>

namespace scop::math
{

struct vec4;

struct vec2
{
	float x = 0.0f;
	float y = 0.0f;

	constexpr vec2() = default;
	constexpr explicit vec2(float v) : x(v), y(v) {}
	constexpr vec2(float x_, float y_) : x(x_), y(y_) {}
};

struct vec3
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	constexpr vec3() = default;
	constexpr explicit vec3(float v) : x(v), y(v), z(v) {}
	constexpr vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
	constexpr vec3(const vec4& v);
};

struct vec4
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

	constexpr vec4() = default;
	constexpr explicit vec4(float v) : x(v), y(v), z(v), w(v) {}
	constexpr vec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
	constexpr vec4(const vec3& v, float w_) : x(v.x), y(v.y), z(v.z), w(w_) {}
};

constexpr vec3::vec3(const vec4& v) : x(v.x), y(v.y), z(v.z) {}

inline constexpr vec3 operator+(const vec3& a, const vec3& b)
{
	return vec3{a.x + b.x, a.y + b.y, a.z + b.z};
}

inline constexpr vec3 operator-(const vec3& a, const vec3& b)
{
	return vec3{a.x - b.x, a.y - b.y, a.z - b.z};
}

inline constexpr vec3 operator-(const vec3& v)
{
	return vec3{-v.x, -v.y, -v.z};
}

inline constexpr vec3 operator*(const vec3& v, float s)
{
	return vec3{v.x * s, v.y * s, v.z * s};
}

inline constexpr vec3 operator*(float s, const vec3& v)
{
	return v * s;
}

inline constexpr vec3 operator/(const vec3& v, float s)
{
	return v * (1.0f / s);
}

inline constexpr vec3& operator+=(vec3& a, const vec3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

inline constexpr vec3& operator-=(vec3& a, const vec3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

inline constexpr vec3& operator*=(vec3& v, float s)
{
	v = v * s;
	return v;
}

inline constexpr vec4 operator-(const vec4& v)
{
	return vec4{-v.x, -v.y, -v.z, -v.w};
}

inline constexpr bool operator==(const vec4& lhs, const vec4& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}

inline constexpr float dot(const vec3& a, const vec3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline constexpr vec3 cross(const vec3& a, const vec3& b)
{
	return vec3{
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x};
}

inline float length(const vec3& v)
{
	return std::sqrt(dot(v, v));
}

inline vec3 normalize(const vec3& v)
{
	float len = length(v);
	if (len <= std::numeric_limits<float>::epsilon())
		return vec3(0.0f);
	return v / len;
}

struct mat4x4
{
	std::array<vec4, 4> columns{
		vec4(1.0f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 1.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 1.0f, 0.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f)};

	constexpr mat4x4() = default;
	constexpr explicit mat4x4(float diag)
		: columns{vec4(diag, 0.0f, 0.0f, 0.0f), vec4(0.0f, diag, 0.0f, 0.0f), vec4(0.0f, 0.0f, diag, 0.0f), vec4(0.0f, 0.0f, 0.0f, diag)} {}

	constexpr vec4& operator[](std::size_t idx) { return columns.at(idx); }
	constexpr const vec4& operator[](std::size_t idx) const { return columns.at(idx); }
};

using mat4 = mat4x4;

inline constexpr vec4 operator*(const mat4x4& m, const vec4& v)
{
	return vec4{
		m[0].x * v.x + m[1].x * v.y + m[2].x * v.z + m[3].x * v.w,
		m[0].y * v.x + m[1].y * v.y + m[2].y * v.z + m[3].y * v.w,
		m[0].z * v.x + m[1].z * v.y + m[2].z * v.z + m[3].z * v.w,
		m[0].w * v.x + m[1].w * v.y + m[2].w * v.z + m[3].w * v.w};
}

inline constexpr mat4x4 operator*(const mat4x4& lhs, const mat4x4& rhs)
{
	mat4x4 result{0.0f};
	for (std::size_t column = 0; column < 4; ++column)
		result[column] = lhs * rhs[column];
	return result;
}

inline constexpr mat4x4& operator*=(mat4x4& lhs, const mat4x4& rhs)
{
	lhs = lhs * rhs;
	return lhs;
}

inline constexpr float radians(float deg)
{
	return deg * (std::numbers::pi_v<float> / 180.0f);
}

inline mat4x4 translate(const mat4x4& m, const vec3& offset)
{
	mat4x4 translation(1.0f);
	translation[3] = vec4(offset, 1.0f);
	return m * translation;
}

inline mat4x4 scale(const mat4x4& m, const vec3& scale)
{
	mat4x4 factors(1.0f);
	factors[0].x = scale.x;
	factors[1].y = scale.y;
	factors[2].z = scale.z;
	return m * factors;
}

inline mat4x4 rotate(const mat4x4& m, float angle, const vec3& axis)
{
	vec3 n = normalize(axis);
	float c = std::cos(angle);
	float s = std::sin(angle);
	float t = 1.0f - c;

	mat4x4 rotation(1.0f);
	rotation[0].x = c + n.x * n.x * t;
	rotation[0].y = n.x * n.y * t + n.z * s;
	rotation[0].z = n.x * n.z * t - n.y * s;

	rotation[1].x = n.y * n.x * t - n.z * s;
	rotation[1].y = c + n.y * n.y * t;
	rotation[1].z = n.y * n.z * t + n.x * s;

	rotation[2].x = n.z * n.x * t + n.y * s;
	rotation[2].y = n.z * n.y * t - n.x * s;
	rotation[2].z = c + n.z * n.z * t;

	return m * rotation;
}

inline mat4x4 perspective(float fovYRadians, float aspect, float nearPlane, float farPlane)
{
	float f = 1.0f / std::tan(fovYRadians * 0.5f);
	mat4x4 result(0.0f);
	result[0].x = f / aspect;
	result[1].y = f;
	result[2].z = (farPlane + nearPlane) / (nearPlane - farPlane);
	result[2].w = -1.0f;
	result[3].z = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
	return result;
}

inline mat4x4 lookAt(const vec3& eye, const vec3& center, const vec3& up)
{
	vec3 f = normalize(center - eye);
	vec3 s = normalize(cross(f, up));
	vec3 u = cross(s, f);

	mat4x4 result(1.0f);
	result[0] = vec4(s.x, s.y, s.z, 0.0f);
	result[1] = vec4(u.x, u.y, u.z, 0.0f);
	result[2] = vec4(-f.x, -f.y, -f.z, 0.0f);
	result[3] = vec4(-dot(s, eye), -dot(u, eye), dot(f, eye), 1.0f);
	return result;
}

} // namespace scop::math

#endif // MATH_HPP
