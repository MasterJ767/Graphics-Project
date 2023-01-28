#pragma once

#include "../nclgl/OGLRenderer.h"
#include "Noise.h"

class ChunkMesh : public Mesh {
public:
	ChunkMesh(Noise* noise, Vector3 volume, Vector3 chunkPositon);
	~ChunkMesh();

protected:
	float GenerateElevation(Vector3 point);
	
	Vector3 volume;
	Noise* noise;
};
