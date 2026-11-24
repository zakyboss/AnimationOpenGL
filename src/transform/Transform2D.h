#pragma once
#include <vector>
#include "../core/Geometry.h"

// 3x3 matrix for 2D transformations in homogeneous coordinates:
//   [x']   [m00 m01 m02] [x]
//   [y'] = [m10 m11 m12] [y]
//   [1 ]   [ 0   0   1 ] [1]
// Combining transformations is matrix multiplication. With A * B, B is applied
// first, then A.
struct Matrix3 {
    float m[3][3];

    static Matrix3 identity();
    Matrix3 operator*(const Matrix3& other) const;
};

Matrix3 translate(float tx, float ty);
// Rotation about the origin; positive degrees turn anticlockwise (y points up).
Matrix3 rotate(float degrees);
Matrix3 scale(float sx, float sy);

// Rotation and scaling about a pivot: translate the pivot to the origin,
// transform, then translate back.
Matrix3 rotateAbout(float degrees, PointF pivot);
Matrix3 scaleAbout(float sx, float sy, PointF pivot);

PointF apply(const Matrix3& m, PointF p);
Point toPoint(PointF p);  // rounds to the nearest pixel

// Transforms every vertex of a polygon, ready for scanlineFill.
std::vector<Point> transformPolygon(const Matrix3& m, const std::vector<PointF>& points);
