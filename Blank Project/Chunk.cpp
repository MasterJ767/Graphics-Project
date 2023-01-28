#include "Chunk.h"

Chunk::Chunk(Noise* noise, Vector3 volume, Vector3 position, Material* materials) {
	terrainMesh = new ChunkMesh(noise, volume, position, materials);
	position = position;

	isEnabled = true;
}

Chunk::~Chunk() {
	delete terrainMesh;
}

void Chunk::Draw(OGLRenderer& r) {
	if (terrainMesh) { 
		r.BindShader(terrainMesh->material->shader);
		r.UpdateShaderMatrices();

		glUniform1i(glGetUniformLocation(terrainMesh->material->shader->GetProgram(), "diffuseTex"), 1);

		terrainMesh->Draw(); 
	}
}