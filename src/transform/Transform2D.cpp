#include "Transform2D.h"
#include <cmath>

Matrix3 Matrix3::identity() {
    return {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};
}

Matrix3 Matrix3::operator*(const Matrix3& o) const {
    Matrix3 r{};
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 3; ++k) r.m[i][j] += m[i][k] * o.m[k][j];
    return r;
}

Matrix3 translate(float tx, float ty) {
    return {{{1, 0, tx}, {0, 1, ty}, {0, 0, 1}}};
}

Matrix3 rotate(float degrees) {
    float rad = degrees * 3.14159265f / 180.0f;
    float c = std::cos(rad), s = std::sin(rad);
    return {{{c, -s, 0}, {s, c, 0}, {0, 0, 1}}};
}

Matrix3 scale(float sx, float sy) {
    return {{{sx, 0, 0}, {0, sy, 0}, {0, 0, 1}}};
}

Matrix3 rotateAbout(float degrees, PointF pivot) {
    return translate(pivot.x, pivot.y) * rotate(degrees) * translate(-pivot.x, -pivot.y);
}

Matrix3 scaleAbout(float sx, float sy, PointF pivot) {
    return translate(pivot.x, pivot.y) * scale(sx, sy) * translate(-pivot.x, -pivot.y);
}

PointF apply(const Matrix3& m, PointF p) {
    return {m.m[0][0] * p.x + m.m[0][1] * p.y + m.m[0][2],
            m.m[1][0] * p.x + m.m[1][1] * p.y + m.m[1][2]};
}

Point toPoint(PointF p) {
    return {static_cast<int>(std::lround(p.x)), static_cast<int>(std::lround(p.y))};
}

std::vector<Point> transformPolygon(const Matrix3& m, const std::vector<PointF>& points) {
    std::vector<Point> out;
    out.reserve(points.size());
    for (const PointF& p : points) out.push_back(toPoint(apply(m, p)));
    return out;
}
