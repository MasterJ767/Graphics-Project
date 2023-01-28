#include "ChunkMesh.h"

ChunkMesh::ChunkMesh(Noise* noise, Vector3 volume, Vector3 chunkPositon) {
	this->noise = noise;

	numVertices = volume.x * volume.z;
	numIndices = (volume.x - 1) * (volume.z - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];
	Vector3 vertexScale = Vector3(16.0f, 1.0f, 16.0f);
	Vector2 textureScale = Vector2(1 / 16.0f, 1 / 16.0f);

	for (int z = 0; z < volume.z; ++z) {
		for (int x = 0; x < volume.x; ++x) {
			int offset = (z * volume.x) + x;
			Vector3 position = Vector3(x + chunkPositon.x, 0, z + chunkPositon.z);
			position.y += GenerateElevation(position) * volume.y;
			vertices[offset] = position * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;
		}
	}

	int i = 0;

	for (int z = 0; z < volume.z - 1; ++z) {
		for (int x = 0; x < volume.x - 1; ++x) {
			int a = (z * (volume.x)) + x;
			int b = (z * (volume.x)) + (x + 1);
			int c = ((z + 1) * (volume.x)) + (x + 1);
			int d = ((z + 1) * (volume.x)) + x;

			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;

			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}
	GenerateNormals();
	GenerateTangents();

	BufferData();

	this->volume.x = vertexScale.x * (volume.x - 1);
	this->volume.y = vertexScale.y * (volume.y - 1);
	this->volume.z = vertexScale.z * (volume.z - 1);
}

ChunkMesh::~ChunkMesh() {
	delete noise;
}

float ChunkMesh::GenerateElevation(Vector3 point) {
	float result = noise->rawNoise(Vector2(point.x, point.z));
	std::cout << result  << " | (" << point.x << ", " << point.y << ", " << point.z << ")\n";
	return result;
}