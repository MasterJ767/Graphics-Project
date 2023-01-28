#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Frustum.h"

class HeightMap;
class Camera;
class Light;
class Shader;
class MeshAnimation;
class MeshMaterial;
class SceneNode;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	void SetShaderLight2(Shader* currentShader, const Light& l);
	void DrawNode(SceneNode* n);
	void DrawSkybox(SceneNode* n);
	void DrawTerrain(SceneNode* n);
	void DrawWater(SceneNode* n);
	void DrawCrystals(SceneNode* n);
	void DrawModel(SceneNode* n);
	void BuildNodeLists(SceneNode* from);
	void DrawNodes();
	void SortNodeLists();
	void ClearNodeLists();
	/* void PostProcess();
	void PresentScene(); */

	vector <SceneNode*> transparentNodeList;
	vector <SceneNode*> nodeList;

	Camera* camera;
	HeightMap* heightMap;
	Mesh* quad;
	Mesh* cube;
	Mesh* mesh;
	Mesh* tree;
	MeshAnimation* anim;
	MeshMaterial* material;
	MeshMaterial* treeMaterial;
	vector<GLuint> matTextures;
	vector<GLuint> treeMatTextures;
	Light* light; 
	Light* light2;
	SceneNode* root;
	Frustum frameFrustum;

	Shader* terrainShader;
	Shader* waterShader;
	Shader* crystalShader;
	Shader* modelShader;
	Shader* skyboxShader;
	//Shader* sceneShader;
	//Shader* processShader;

	GLuint earthTex;
	GLuint earthBump;
	GLuint grassTex;
	GLuint grassBump;
	GLuint waterTex;
	GLuint waterTex2;
	GLuint waterBump;
	GLuint crystalTex;
	GLuint crystalBump;
	GLuint environMap;
	GLuint cubeMap;

	//GLuint bufferFBO;
	//GLuint processFBO;
	//GLuint bufferColourTex[2];
	//GLuint bufferDepthTex;

	float lightRotate;
	float waterRotate;
	float waterCycle;

	int currentFrame;
	float frameTime;
};
