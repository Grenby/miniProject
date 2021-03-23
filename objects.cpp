#include "objects.h"
#include <iostream>
#include <utility>
#include "cmath"


float SDFObject::sdf(float px, float py, float pz) const {
    return 0;
}

float SDFObject::sdf(vec3 v) const {
    return 0;
}

const vec3 &SDFObject::getColor() const {
    return color;
}

const vec3 &SDFObject::getColor(const vec3 &pos) const {
    return color;
}

float SDFSphere::sdf(float px, float py, float pz) const {
    float dx = (pos.x - px);
    float dy = (pos.y - py);
    float dz = (pos.z - pz);
    return std::sqrt(dx * dx + dy * dy + dz * dz) - r;
}

SDFSphere::SDFSphere(float x, float y, float z, float r) : pos{x, y, z}, r(r) {}

float SDFSphere::getR() const {
    return r;
}

void SDFSphere::setR(float r) {
    SDFSphere::r = r;
}

float SDFSphere::sdf(vec3 v) const {
    return this->sdf(v.x, v.y, v.z);
}

const vec3 &SDFSphere::getPos() const {
    return pos;
}

void SDFSphere::setPos(const vec3 &pos) {
    SDFSphere::pos = pos;
}


SDFPlane::SDFPlane(float x, float y, float z, float nx, float ny, float nz) {
    pos.set(x, y, z);
    n.set(nx, ny, nz).normalize();
}


float SDFPlane::sdf(float px, float py, float pz) const {
    return std::abs(inProd(vec3(px, py, pz) - pos, n));
}

float SDFPlane::sdf(vec3 v) const {
    return std::abs(inProd(v - pos, n));
}

const vec3 &SDFPlane::getPos() const {
    return pos;
}

void SDFPlane::setPos(const vec3 &pos) {
    SDFPlane::pos = pos;
}

const vec3 &SDFPlane::getN() const {
    return n;
}

void SDFPlane::setN(const vec3 &n) {
    SDFPlane::n = n;
}

vec3::vec3() {

}

vec3::vec3(float _x, float _y, float _z) {
    x = _x;
    y = _y;
    z = _z;
}

vec3 &vec3::set(float _x, float _y, float _z) {
    x = _x;
    y = _y;
    z = _z;
    return *this;
}

float vec3::len() const {
    return sqrt(x * x + y * y + z * z);
}

float vec3::len2() const {
    return x * x + y * y + z * z;
}

vec3 &vec3::normalize() {
    float r = len();
    if (r != 0)
        *this *= 1 / r;
    return *this;
}

void vec3::operator*=(float fac) {
    x *= fac;
    y *= fac;
    z *= fac;
}

void vec3::operator-=(const vec3 &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
}

void vec3::operator+=(const vec3 &other) {
    x += other.x;
    y += other.y;
    z += other.z;
}

vec3 operator*(float fac, const vec3 &s) {
    return vec3(fac * s.x, fac * s.y, fac * s.z);
}

std::ostream &operator<<(std::ostream &os, const vec3 &vec3) {
    os << "x: " << vec3.x << " y: " << vec3.y << " z: " << vec3.z;
    return os;
}

void vec3::operator*=(const vec3 &other) {
    x *= other.x;
    y *= other.y;
    z *= other.z;
}

float &vec3::operator[](int index) {
    if (index == 0)
        return x;
    else if (index == 1)
        return y;
    else
        return z;
}

vec3 vec3::operator-() const {
    return vec3(-x, -y, -z);
}

vec3::vec3(const vec3 &other) {
    x = other.x;
    y = other.y;
    z = other.z;
}

vec3 operator*(const vec3 &s, float fac) {
    return vec3(fac * s.x, fac * s.y, fac * s.z);
}

vec3 operator+(const vec3 &f, const vec3 &s) {
    return vec3(f.x + s.x, f.y + s.y, f.z + s.z);
}

vec3 operator-(const vec3 &f, const vec3 &s) {
    return vec3(f.x - s.x, f.y - s.y, f.z - s.z);;
}

vec3 operator*(const vec3 &f, const vec3 &s) {
    return vec3(f.x * s.x, f.y * s.y, f.z * s.z);
}

float inProd(const vec3 &f, const vec3 &s) {
    return f.x * s.x + f.y * s.y + f.z * s.z;
}

vec3 outProd(const vec3 &f, const vec3 &s) {
    return vec3(f.y * s.z - f.z * s.y, f.z * s.x - f.x * s.z, f.x * s.y - f.y * s.x);
}

vec3 reflect(const vec3 &v, const vec3 &n) {
    return v - 2 * inProd(v, n) * n;
}

float SDFSphere2::sdf(float px, float py, float pz) const {
    float l1 = s1.sdf(px, py, pz);
    float l2 = s2.sdf(px, py, pz);
    float k = 8.;
    float h = std::max(k - std::abs(l1 - l2), 0.f) / k;
    return std::min(l1, l2) - h * h * h * k / 6.0f;
}

float SDFSphere2::sdf(vec3 v) const {
    return this->sdf(v.x, v.y, v.z);
}

SDFSphere2::SDFSphere2(SDFSphere s1, SDFSphere s2) : s1(std::move(s1)), s2(std::move(s2)) {}

const vec3 &SDFSphere2::getColor(const vec3 &pos) const {
    float l1 = s1.sdf(pos);
    float l2 = s2.sdf(pos);
    if (l1 > l2)
        return s2.getColor();
    return s1.getColor();
}

void ShaderGenerator::addUniform(const std::string &s) {
    uniforms += s;
}

void ShaderGenerator::addIn(const std::string &s) {
    in += s;
}

void ShaderGenerator::addOut(const std::string &s) {
    out += s;
}

std::string ShaderGenerator::getVertex() {
    return std::string();
}

std::string ShaderGenerator::getFragment() {
    return std::string();
}

void ShaderGenerator::addSDF(const std::string &s) {
    sdf += s;
}

void camera::look(const vec3 &point) {
    vec3 v = point - pos;
    if (v.len2() != 0)
        dir = v.normalize();
}

mat3::mat3() {

}

mat3::mat3(const mat3 &matrix) {

}

mat3 &mat3::idt() {
    float *_val = this->val;
    _val[M00] = 1;
    _val[M10] = 0;
    _val[M20] = 0;
    _val[M01] = 0;
    _val[M11] = 1;
    _val[M21] = 0;
    _val[M02] = 0;
    _val[M12] = 0;
    _val[M22] = 1;
    return *this;
}

float mat3::det() const {
    float *v = this->val;
    return v[M00] * v[M11] * v[M22] + v[M01] * v[M12] * v[M20] + v[M02] * v[M10] * v[M21] - v[M00]
                                                                                            * v[M12] * v[M21] -
           v[M01] * v[M10] * v[M22] - v[M02] * v[M11] * v[M20];
}

mat3 &mat3::set(const mat3 &m) {
    for (int i = 0; i < 9; ++i)
        val[i] = m[i];
    return *this;
}

void mat3::inv() {
    float d = det();

    float inv_det = 1.0f / d;
    float *v = this->val;

    float x00 = v[M11] * v[M22] - v[M21] * v[M12];
    float x10 = v[M20] * v[M12] - v[M10] * v[M22];
    float x20 = v[M10] * v[M21] - v[M20] * v[M11];
    float x01 = v[M21] * v[M02] - v[M01] * v[M22];
    float x11 = v[M00] * v[M22] - v[M20] * v[M02];
    float x21 = v[M20] * v[M01] - v[M00] * v[M21];
    float x02 = v[M01] * v[M12] - v[M11] * v[M02];
    float x12 = v[M10] * v[M02] - v[M00] * v[M12];
    float x22 = v[M00] * v[M11] - v[M10] * v[M01];

    val[mat3::M00] = x00 * inv_det;
    val[mat3::M01] = x01 * inv_det;
    val[mat3::M02] = x02 * inv_det;
    val[mat3::M10] = x10 * inv_det;
    val[mat3::M11] = x11 * inv_det;
    val[mat3::M22] = x12 * inv_det;
    val[mat3::M20] = x20 * inv_det;
    val[mat3::M21] = x21 * inv_det;
    val[mat3::M22] = x22 * inv_det;
}

mat3::mat3(float x00, float x01, float x02, float x10, float x11, float x12, float x20, float x21, float x22) {
    this->val[M00] = x00;
    this->val[M01] = x01;
    this->val[M02] = x02;
    this->val[M10] = x10;
    this->val[M11] = x11;
    this->val[M12] = x12;
    this->val[M20] = x20;
    this->val[M21] = x21;
    this->val[M22] = x22;
}


mat3::~mat3() {
    delete[]val;
}

float mat3::operator[](const int index) const {
    return val[index];
}

mat3 &mat3::set(float x00, float x01, float x02, float x10, float x11, float x12, float x20, float x21, float x22) {
    this->val[M00] = x00;
    this->val[M01] = x01;
    this->val[M02] = x02;
    this->val[M10] = x10;
    this->val[M11] = x11;
    this->val[M12] = x12;
    this->val[M20] = x20;
    this->val[M21] = x21;
    this->val[M22] = x22;

    return *this;
}

std::ostream &operator<<(std::ostream &os, const mat3 &m) {
    os << "val:\n "
       << m.val[mat3::M00] << '|' << m.val[mat3::M01] << '|' << m.val[mat3::M02] << "|\n"
       << m.val[mat3::M10] << '|' << m.val[mat3::M11] << '|' << m.val[mat3::M12] << "|\n"
       << m.val[mat3::M20] << '|' << m.val[mat3::M21] << '|' << m.val[mat3::M22] << '|';
    return os;
}

vec3 operator*(const mat3 &m, const vec3 &v) {
    float x = m[mat3::M00] * v.x + m[mat3::M01] * v.y + m[mat3::M02] * v.z;
    float y = m[mat3::M10] * v.x + m[mat3::M11] * v.y + m[mat3::M12] * v.z;
    float z = m[mat3::M20] * v.x + m[mat3::M21] * v.y + m[mat3::M22] * v.z;
    return vec3(x, y, z);
}

mat3 inv(const mat3 &m) {

    float d = m.det();

    float inv_det = 1.0f / d;
    float *v = m.val;

    float x00 = v[mat3::M11] * v[mat3::M22] - v[mat3::M21] * v[mat3::M12];
    float x10 = v[mat3::M20] * v[mat3::M12] - v[mat3::M10] * v[mat3::M22];
    float x20 = v[mat3::M10] * v[mat3::M21] - v[mat3::M20] * v[mat3::M11];
    float x01 = v[mat3::M21] * v[mat3::M02] - v[mat3::M01] * v[mat3::M22];
    float x11 = v[mat3::M00] * v[mat3::M22] - v[mat3::M20] * v[mat3::M02];
    float x21 = v[mat3::M20] * v[mat3::M01] - v[mat3::M00] * v[mat3::M21];
    float x02 = v[mat3::M01] * v[mat3::M12] - v[mat3::M11] * v[mat3::M02];
    float x12 = v[mat3::M10] * v[mat3::M02] - v[mat3::M00] * v[mat3::M12];
    float x22 = v[mat3::M00] * v[mat3::M11] - v[mat3::M10] * v[mat3::M01];

    x00 *= inv_det;
    x01 *= inv_det;
    x02 *= inv_det;
    x10 *= inv_det;
    x11 *= inv_det;
    x12 *= inv_det;
    x20 *= inv_det;
    x21 *= inv_det;
    x22 *= inv_det;

    return mat3(x00, x01, x02, x10, x11, x12, x20, x21, x22);
}
