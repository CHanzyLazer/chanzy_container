#pragma once
#ifdef _MSC_VER
#pragma warning(disable: 4577)
#pragma warning(disable: 4530)
#pragma warning(disable: 4514)
#pragma warning(disable: 4061)
#endif
#include <cmath>
#include <array>
#include <ostream>

using std::array;
using std::sqrt;
using std::ostream;

/*----------------------*/
//使用三维array存储
class Position {
	friend double get_distance_squared(const Position& p1, const Position& p2);
	friend double dot(const Position& p1, const Position& p2);
	friend Position cross(const Position& p1, const Position& p2);
private:
	double m_pos[3] {0.0, 0.0, 0.0};
    using size_type = size_t;
public:
    Position() = default;
	constexpr Position(const double& a_x, const double& a_y, const double& a_z);
	explicit Position(const array<double, 3>& arr3);

	void get_position(double& a_x, double& a_y, double& a_z) const;
	void set_position(const double& a_x, const double& a_y, const double& a_z);
    constexpr double& operator[] (size_type a_i);
    constexpr const double& operator[] (size_type a_i) const;

	//一些运算的接口
    constexpr Position operator+ (const Position& p) const; //必须用Position返回因为out_p是局部变量
    constexpr Position operator- (const Position& p) const;
    constexpr Position operator* (const double& x) const;
    constexpr Position operator/ (const double& x) const;
    constexpr Position& operator+= (const Position& p);
    constexpr Position& operator-= (const Position& p);
	constexpr Position& operator*= (const double& x);
	constexpr Position& operator/= (const double& x);
	// 对右值进行优化
	Position operator+ (Position&& p) const;
	Position operator- (Position&& p) const;

	double norm() const;
    constexpr double get_distance_squared(const Position& p) const;
    constexpr double dot(const Position& p) const;
    constexpr Position cross(const Position& p) const;

    constexpr bool operator==(const Position& p) const;
    constexpr bool operator!=(const Position& p) const;
};

/*--------------*/
//实现部分（inline必须要在一个文件中才行）

inline constexpr Position::Position(const double& a_x, const double& a_y, const double& a_z) {
	m_pos[0] = a_x; m_pos[1] = a_y; m_pos[2] = a_z;
}
inline Position::Position(const array<double, 3>& arr3) {
	m_pos[0] = arr3[0]; m_pos[1] = arr3[1]; m_pos[2] = arr3[2];
}

inline void Position::get_position(double& a_x, double& a_y, double& a_z) const {
	a_x = m_pos[0];
	a_y = m_pos[1];
	a_z = m_pos[2];
}
inline void Position::set_position(const double& a_x, const double& a_y, const double& a_z) {
	m_pos[0] = a_x; m_pos[1] = a_y; m_pos[2] = a_z;
}
inline constexpr double& Position::operator[] (const size_type a_i) {
	return m_pos[a_i];
}
inline constexpr const double& Position::operator[] (const size_type a_i) const {
	return m_pos[a_i];
}

inline constexpr Position Position::operator+ (const Position& p) const {
	return {m_pos[0] + p.m_pos[0], m_pos[1] + p.m_pos[1], m_pos[2] + p.m_pos[2]};
}
inline constexpr Position Position::operator- (const Position& p) const {
	return {m_pos[0] - p.m_pos[0], m_pos[1] - p.m_pos[1], m_pos[2] - p.m_pos[2]};
}
inline constexpr Position Position::operator* (const double& x) const {
	return {m_pos[0] * x, m_pos[1] * x, m_pos[2] * x};
}
inline constexpr Position Position::operator/ (const double& x) const {
	return {m_pos[0] / x, m_pos[1] / x, m_pos[2] / x};
}
inline constexpr Position &Position::operator+= (const Position& p) {
	m_pos[0] += p[0]; m_pos[1] += p[1]; m_pos[2] += p[2];
	return *this;
}
inline constexpr Position &Position::operator-= (const Position& p) {
	m_pos[0] -= p[0]; m_pos[1] -= p[1]; m_pos[2] -= p[2];
	return *this;
}
inline constexpr Position& Position::operator*= (const double& x) {
	m_pos[0] *= x; m_pos[1] *= x; m_pos[2] *= x;
	return *this;
}
inline constexpr Position& Position::operator/= (const double& x) {
	m_pos[0] /= x; m_pos[1] /= x; m_pos[2] /= x;
	return *this;
}
inline Position Position::operator+ (Position&& p) const {
    p[0] = m_pos[0] + p[0]; p[1] = m_pos[1] + p[1]; p[2] = m_pos[2] + p[2];
	return p;
}
inline Position Position::operator- (Position&& p) const {
	p[0] = m_pos[0] - p[0]; p[1] = m_pos[1] - p[1]; p[2] = m_pos[2] - p[2];
	return p;
}
inline Position operator+ (Position&& p1, const Position& p2) {
	p1[0] += p2[0]; p1[1] += p2[1]; p1[2] += p2[2];
	return p1;
}
inline Position operator- (Position&& p1, const Position& p2) {
	p1[0] -= p2[0]; p1[1] -= p2[1]; p1[2] -= p2[2];
	return p1;
}
inline Position operator+ (Position&& p1, Position&& p2) {
    p1[0] += p2[0]; p1[1] += p2[1]; p1[2] += p2[2];
    return p1;
}
inline Position operator- (Position&& p1, Position&& p2) {
    p1[0] -= p2[0]; p1[1] -= p2[1]; p1[2] -= p2[2];
    return p1;
}

inline double Position::norm() const {
	return sqrt(
		m_pos[0] * m_pos[0] +
		m_pos[1] * m_pos[1] +
		m_pos[2] * m_pos[2]);
}
inline constexpr double Position::get_distance_squared(const Position& p) const {
	return
		(p.m_pos[0] - m_pos[0]) * (p.m_pos[0] - m_pos[0]) +
		(p.m_pos[1] - m_pos[1]) * (p.m_pos[1] - m_pos[1]) +
		(p.m_pos[2] - m_pos[2]) * (p.m_pos[2] - m_pos[2]);
}
inline constexpr double Position::dot(const Position& p) const {
	return
		m_pos[0] * p.m_pos[0] +
		m_pos[1] * p.m_pos[1] +
		m_pos[2] * p.m_pos[2];
}
inline constexpr Position Position::cross(const Position& p) const {
	return {
        m_pos[1] * p.m_pos[2] - p.m_pos[1] * m_pos[2],
        m_pos[2] * p.m_pos[0] - p.m_pos[0] * m_pos[2],
        m_pos[0] * p.m_pos[1] - p.m_pos[0] * m_pos[1]};
}
inline double get_distance_squared(const Position& p1, const Position& p2) {
	return
		(p1.m_pos[0] - p2.m_pos[0]) * (p1.m_pos[0] - p2.m_pos[0]) +
		(p1.m_pos[1] - p2.m_pos[1]) * (p1.m_pos[1] - p2.m_pos[1]) +
		(p1.m_pos[2] - p2.m_pos[2]) * (p1.m_pos[2] - p2.m_pos[2]);
}
inline double dot(const Position& p1, const Position& p2) {
	return
		p1.m_pos[0] * p2.m_pos[0] +
		p1.m_pos[1] * p2.m_pos[1] +
		p1.m_pos[2] * p2.m_pos[2];
}
inline Position cross(const Position& p1, const Position& p2) {
	return {
		p1.m_pos[1] * p2.m_pos[2] - p2.m_pos[1] * p1.m_pos[2],
		p1.m_pos[2] * p2.m_pos[0] - p2.m_pos[0] * p1.m_pos[2],
		p1.m_pos[0] * p2.m_pos[1] - p2.m_pos[0] * p1.m_pos[1]};
}

inline constexpr bool Position::operator==(const Position& p) const {
    return m_pos[0] == p.m_pos[0] && m_pos[1] == p.m_pos[1] && m_pos[2] == p.m_pos[2];
}
inline constexpr bool Position::operator!=(const Position& p) const {
    return !(p == *this);
}



inline ostream& operator<<(ostream& a_cout, const Position& a_pos) {
    a_cout << a_pos[0] << " " << a_pos[1] << " " << a_pos[2];
    return a_cout;
}

