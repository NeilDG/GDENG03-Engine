#include "AnitoMeshGenerator.h"
#include <vector>
#include <cmath>

namespace Anito {

AnitoMeshGenerator::MeshData AnitoMeshGenerator::createCube(float size) {
    float halfSize = size * 0.5f;

    // 24 vertices (4 per face for proper normals)
    // abgr format: 0xFFFFFFFF = white
    PosNormalTexcoordVertex vertices[] = {
        // Front face (+Z)
        {-halfSize, -halfSize,  halfSize,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0xFFFFFFFF},
        { halfSize, -halfSize,  halfSize,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, 0xFFFFFFFF},
        { halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, 0xFFFFFFFF},
        {-halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 0xFFFFFFFF},

        // Back face (-Z)
        { halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0xFFFFFFFF},
        {-halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 0xFFFFFFFF},
        {-halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, 0xFFFFFFFF},
        { halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0xFFFFFFFF},

        // Right face (+X)
        { halfSize, -halfSize,  halfSize,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0xFFFFFFFF},
        { halfSize, -halfSize, -halfSize,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0xFFFFFFFF},
        { halfSize,  halfSize, -halfSize,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, 0xFFFFFFFF},
        { halfSize,  halfSize,  halfSize,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0xFFFFFFFF},

        // Left face (-X)
        {-halfSize, -halfSize, -halfSize, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0xFFFFFFFF},
        {-halfSize, -halfSize,  halfSize, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0xFFFFFFFF},
        {-halfSize,  halfSize,  halfSize, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, 0xFFFFFFFF},
        {-halfSize,  halfSize, -halfSize, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0xFFFFFFFF},

        // Top face (+Y)
        {-halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0xFFFFFFFF},
        { halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0xFFFFFFFF},
        { halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, 0xFFFFFFFF},
        {-halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0xFFFFFFFF},

        // Bottom face (-Y)
        {-halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0xFFFFFFFF},
        { halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 0xFFFFFFFF},
        { halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, 0xFFFFFFFF},
        {-halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, 0xFFFFFFFF}
    };

    // 36 indices (6 faces * 2 triangles * 3 vertices)
    uint16_t indices[] = {
        // Front
        0, 1, 2,  2, 3, 0,
        // Back
        4, 5, 6,  6, 7, 4,
        // Right
        8, 9, 10,  10, 11, 8,
        // Left
        12, 13, 14,  14, 15, 12,
        // Top
        16, 17, 18,  18, 19, 16,
        // Bottom
        20, 21, 22,  22, 23, 20
    };

    MeshData mesh;
    mesh.vertexBuffer = std::make_shared<AnitoVertexBuffer>(vertices, 24, PosNormalTexcoordVertex::ms_layout);
    mesh.indexBuffer = std::make_shared<AnitoIndexBuffer>(indices, 36, false);

    return mesh;
}

AnitoMeshGenerator::MeshData AnitoMeshGenerator::createSphere(float radius, uint32_t segments, uint32_t rings) {
    std::vector<PosNormalTexcoordVertex> vertices;
    std::vector<uint16_t> indices;

    const float PI = 3.14159265359f;

    // Generate vertices
    for (uint32_t ring = 0; ring <= rings; ++ring) {
        float phi = PI * static_cast<float>(ring) / static_cast<float>(rings);
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);

        for (uint32_t seg = 0; seg <= segments; ++seg) {
            float theta = 2.0f * PI * static_cast<float>(seg) / static_cast<float>(segments);
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            PosNormalTexcoordVertex vertex;
            vertex.nx = sinPhi * cosTheta;
            vertex.ny = cosPhi;
            vertex.nz = sinPhi * sinTheta;

            vertex.x = radius * vertex.nx;
            vertex.y = radius * vertex.ny;
            vertex.z = radius * vertex.nz;

            vertex.u = static_cast<float>(seg) / static_cast<float>(segments);
            vertex.v = static_cast<float>(ring) / static_cast<float>(rings);

            vertices.push_back(vertex);
        }
    }

    // Generate indices
    for (uint32_t ring = 0; ring < rings; ++ring) {
        for (uint32_t seg = 0; seg < segments; ++seg) {
            uint16_t current = static_cast<uint16_t>(ring * (segments + 1) + seg);
            uint16_t next = static_cast<uint16_t>(current + segments + 1);

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    MeshData mesh;
    mesh.vertexBuffer = std::make_shared<AnitoVertexBuffer>(vertices.data(), static_cast<uint32_t>(vertices.size()), PosNormalTexcoordVertex::ms_layout);
    mesh.indexBuffer = std::make_shared<AnitoIndexBuffer>(indices.data(), static_cast<uint32_t>(indices.size()), false);

    return mesh;
}

AnitoMeshGenerator::MeshData AnitoMeshGenerator::createPlane(float width, float height, uint32_t subdivisionsX, uint32_t subdivisionsY) {
    std::vector<PosNormalTexcoordVertex> vertices;
    std::vector<uint16_t> indices;

    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;

    // Generate vertices
    for (uint32_t y = 0; y <= subdivisionsY; ++y) {
        float v = static_cast<float>(y) / static_cast<float>(subdivisionsY);
        float posY = -halfHeight + v * height;

        for (uint32_t x = 0; x <= subdivisionsX; ++x) {
            float u = static_cast<float>(x) / static_cast<float>(subdivisionsX);
            float posX = -halfWidth + u * width;

            PosNormalTexcoordVertex vertex;
            vertex.x = posX;
            vertex.y = 0.0f;
            vertex.z = posY;

            vertex.nx = 0.0f;
            vertex.ny = 1.0f;
            vertex.nz = 0.0f;

            vertex.u = u;
            vertex.v = v;

            vertices.push_back(vertex);
        }
    }

    // Generate indices — CCW winding when viewed from above (+Y) to match BGFX_STATE_CULL_CW
    for (uint32_t y = 0; y < subdivisionsY; ++y) {
        for (uint32_t x = 0; x < subdivisionsX; ++x) {
            uint16_t topLeft = static_cast<uint16_t>(y * (subdivisionsX + 1) + x);
            uint16_t topRight = topLeft + 1;
            uint16_t bottomLeft = static_cast<uint16_t>((y + 1) * (subdivisionsX + 1) + x);
            uint16_t bottomRight = bottomLeft + 1;

            // CCW from above: topLeft -> topRight -> bottomLeft
            indices.push_back(topLeft);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);

            indices.push_back(topRight);
            indices.push_back(bottomRight);
            indices.push_back(bottomLeft);
        }
    }

    MeshData mesh;
    mesh.vertexBuffer = std::make_shared<AnitoVertexBuffer>(vertices.data(), static_cast<uint32_t>(vertices.size()), PosNormalTexcoordVertex::ms_layout);
    mesh.indexBuffer = std::make_shared<AnitoIndexBuffer>(indices.data(), static_cast<uint32_t>(indices.size()), false);

    return mesh;
}

AnitoMeshGenerator::MeshData AnitoMeshGenerator::createCylinder(float radius, float height, uint32_t segments) {
    std::vector<PosNormalTexcoordVertex> vertices;
    std::vector<uint16_t> indices;

    const float PI = 3.14159265359f;
    float halfHeight = height * 0.5f;

    // Generate vertices for the sides
    for (uint32_t i = 0; i <= segments; ++i) {
        float angle = 2.0f * PI * static_cast<float>(i) / static_cast<float>(segments);
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        float u = static_cast<float>(i) / static_cast<float>(segments);

        // Bottom vertex
        PosNormalTexcoordVertex bottomVert;
        bottomVert.x = x;
        bottomVert.y = -halfHeight;
        bottomVert.z = z;
        bottomVert.nx = x / radius;
        bottomVert.ny = 0.0f;
        bottomVert.nz = z / radius;
        bottomVert.u = u;
        bottomVert.v = 0.0f;
        vertices.push_back(bottomVert);

        // Top vertex
        PosNormalTexcoordVertex topVert;
        topVert.x = x;
        topVert.y = halfHeight;
        topVert.z = z;
        topVert.nx = x / radius;
        topVert.ny = 0.0f;
        topVert.nz = z / radius;
        topVert.u = u;
        topVert.v = 1.0f;
        vertices.push_back(topVert);
    }

    // Generate indices for sides
    for (uint32_t i = 0; i < segments; ++i) {
        uint16_t base = static_cast<uint16_t>(i * 2);

        indices.push_back(base);
        indices.push_back(base + 2);
        indices.push_back(base + 1);

        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }

    MeshData mesh;
    mesh.vertexBuffer = std::make_shared<AnitoVertexBuffer>(vertices.data(), static_cast<uint32_t>(vertices.size()), PosNormalTexcoordVertex::ms_layout);
    mesh.indexBuffer = std::make_shared<AnitoIndexBuffer>(indices.data(), static_cast<uint32_t>(indices.size()), false);

    return mesh;
}

AnitoMeshGenerator::MeshData AnitoMeshGenerator::createCone(float radius, float height, uint32_t segments) {
    std::vector<PosNormalTexcoordVertex> vertices;
    std::vector<uint16_t> indices;

    const float PI = 3.14159265359f;
    float halfHeight = height * 0.5f;

    // Apex vertex
    PosNormalTexcoordVertex apex;
    apex.x = 0.0f;
    apex.y = halfHeight;
    apex.z = 0.0f;
    apex.nx = 0.0f;
    apex.ny = 1.0f;
    apex.nz = 0.0f;
    apex.u = 0.5f;
    apex.v = 1.0f;
    uint16_t apexIndex = static_cast<uint16_t>(vertices.size());
    vertices.push_back(apex);

    // Base vertices
    for (uint32_t i = 0; i <= segments; ++i) {
        float angle = 2.0f * PI * static_cast<float>(i) / static_cast<float>(segments);
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);

        PosNormalTexcoordVertex vertex;
        vertex.x = x;
        vertex.y = -halfHeight;
        vertex.z = z;

        // Calculate normal (pointing outward and slightly up)
        float len = std::sqrt(x * x + z * z + height * height);
        vertex.nx = x / len;
        vertex.ny = height / len;
        vertex.nz = z / len;

        vertex.u = static_cast<float>(i) / static_cast<float>(segments);
        vertex.v = 0.0f;

        vertices.push_back(vertex);
    }

    // Generate indices
    for (uint32_t i = 0; i < segments; ++i) {
        uint16_t current = static_cast<uint16_t>(i + 1);
        uint16_t next = static_cast<uint16_t>(i + 2);

        indices.push_back(apexIndex);
        indices.push_back(current);
        indices.push_back(next);
    }

    MeshData mesh;
    mesh.vertexBuffer = std::make_shared<AnitoVertexBuffer>(vertices.data(), static_cast<uint32_t>(vertices.size()), PosNormalTexcoordVertex::ms_layout);
    mesh.indexBuffer = std::make_shared<AnitoIndexBuffer>(indices.data(), static_cast<uint32_t>(indices.size()), false);

    return mesh;
}

AnitoMeshGenerator::MeshData AnitoMeshGenerator::createScreenQuad() {
    PosNormalTexcoordVertex vertices[] = {
        {-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f},
        { 1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f},
        { 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f},
        {-1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f}
    };

    uint16_t indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    MeshData mesh;
    mesh.vertexBuffer = std::make_shared<AnitoVertexBuffer>(vertices, 4, PosNormalTexcoordVertex::ms_layout);
    mesh.indexBuffer = std::make_shared<AnitoIndexBuffer>(indices, 6, false);

    return mesh;
}

} // namespace Anito
