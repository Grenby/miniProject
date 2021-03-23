#ifndef MINIPROJECT_OBJECTS_H
#define MINIPROJECT_OBJECTS_H

#include <ostream>

struct vec3 {
    float
            x = 0,
            y = 0,
            z = 0;

    vec3();

    vec3(float _x, float _y, float _z);

    vec3(const vec3 &other);

    vec3 &set(float _x, float _y, float _z);

    vec3 &operator=(const vec3 &c) = default;

    float len() const;

    float len2() const;

    vec3 &normalize();

    void operator*=(float fac);

    void operator*=(const vec3 &other);

    void operator-=(const vec3 &other);

    void operator+=(const vec3 &other);

    vec3 operator-() const;

    float &operator[](int index);

};

struct mat3{
public:
    static const int M00 = 0;
    static const int M01 = 3;
    static const int M02 = 6;
    static const int M10 = 1;
    static const int M11 = 4;
    static const int M12 = 7;
    static const int M20 = 2;
    static const int M21 = 5;
    static const int M22 = 8;
    float* val = new float[9];

    mat3 ();

    mat3 (const mat3& matrix);

    mat3(float x00,float x01,float x02,float x10,float x11,float x12,float x20,float x21,float x22);

    ~mat3();

    mat3& idt ();

    mat3& set(float x00,float x01,float x02,float x10,float x11,float x12,float x20,float x21,float x22);

    mat3& set(const mat3& m);

    float det () const;

    void inv ();

    float operator[](int index) const;

};

std::ostream &operator<<(std::ostream &os, const mat3 &mat3);

struct camera {
    vec3
            pos = {0, 0, 0},
            dir = {0, 0, 1},
            up = {0, 1, 0};

    void look(const vec3& point);

};

class SDFObject {
public:
    vec3 color{0, 0, 0};

    virtual const vec3 &getColor(const vec3 &pos) const;

    virtual const vec3 &getColor() const;

    virtual float sdf(float px, float py, float pz) const;

    virtual float sdf(vec3 v) const;

};

class SDFSphere : public SDFObject {
private:
    vec3 pos;
    float r;
public:

    SDFSphere(float x, float y, float z, float r);

    const vec3 &getPos() const;

    void setPos(const vec3 &pos);

    float getR() const;

    void setR(float r);

    float sdf(vec3 v) const override;

    float sdf(float px, float py, float pz) const override;
};

class SDFPlane : public SDFObject {
private:
    vec3 pos;
    vec3 n;

public:
    SDFPlane(float x, float y, float z, float nx, float ny, float nz);

    const vec3 &getPos() const;

    void setPos(const vec3 &pos);

    const vec3 &getN() const;

    void setN(const vec3 &n);

    float sdf(float px, float py, float pz) const override;

    float sdf(vec3 v) const override;
};

class SDFSphere2 : public SDFObject {
public:
    SDFSphere s1, s2;
private:

public:
    SDFSphere2(SDFSphere s1, SDFSphere s2);

private:

public:
    const vec3 &getColor(const vec3 &pos) const override;

private:

    float sdf(float px, float py, float pz) const override;

    float sdf(vec3 v) const override;
};

class ShaderGenerator{
private:
    std::string uniforms;
    std::string in;
    std::string out;
    std::string sdf;

    void addUniform(const std::string& s);
    void addIn(const std::string& s);
    void addOut(const std::string& s);
    void addSDF(const std::string& s);

    std::string getVertex();
    std::string getFragment();

};


std::ostream &operator<<(std::ostream &os, const vec3 &vec3);

vec3 operator*(float fac, const vec3 &s);

vec3 operator*(const vec3 &s, float fac);

vec3 operator+(const vec3 &f, const vec3 &s);

vec3 operator-(const vec3 &f, const vec3 &s);

vec3 operator*(const vec3 &f, const vec3 &s);

float inProd(const vec3 &f, const vec3 &s);

vec3 outProd(const vec3 &f, const vec3 &s);

vec3 reflect(const vec3 &v, const vec3 &n);

vec3 operator*(const mat3 &m,const vec3 &v);

mat3 inv(const mat3 &m);

#endif //MINIPROJECT_OBJECTS_H
