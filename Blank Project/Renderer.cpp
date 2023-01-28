#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/SceneNode.h"
#include <algorithm>

//const int POST_PASSES = 1;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	root = new SceneNode();

	quad = Mesh::GenerateQuad();
	root->AddChild(new SceneNode(quad, Vector4(1, 1, 1, 1), 1));

	heightMap = new HeightMap(TEXTUREDIR"noise2.png");
	root->AddChild(new SceneNode(heightMap, Vector4(1, 1, 1, 1), 2));

	root->AddChild(new SceneNode(quad, Vector4(1, 1, 1, 1), 3));

	earthTex = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	grassTex = SOIL_load_OGL_texture(TEXTUREDIR"grass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	grassBump = SOIL_load_OGL_texture(TEXTUREDIR"grassNorm.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterTex2 = SOIL_load_OGL_texture(TEXTUREDIR"water_alt.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterBump = SOIL_load_OGL_texture(TEXTUREDIR"waterbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	crystalTex = SOIL_load_OGL_texture(TEXTUREDIR"crystal.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	//crystalBump = SOIL_load_OGL_texture(TEXTUREDIR"crystal_alt.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	environMap = SOIL_load_OGL_texture(TEXTUREDIR"environ.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg", TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg", TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	if (!earthTex || !earthBump || !grassTex || !grassBump || !waterTex || !waterTex2 || !waterBump || !crystalTex || !environMap || !cubeMap) {
		return;
	}
	
	terrainShader = new Shader("TerrainVertex.glsl", "TerrainFragment.glsl");
	waterShader = new Shader("WaterVertex.glsl", "WaterFragment.glsl");
	crystalShader = new Shader("CrystalVertex.glsl", "CrystalFragment.glsl");
	modelShader = new Shader("SkinningVertex.glsl", "TexturedFragment.glsl");
	skyboxShader = new Shader("SkyVertex.glsl", "SkyFragment.glsl");
	//sceneShader = new Shader("TexturedVertex.glsl", "presentFragment.glsl");
	//processShader = new Shader("TexturedVertex.glsl", "processfrag.glsl");
	if (!terrainShader->LoadSuccess() || !waterShader->LoadSuccess() || !crystalShader->LoadSuccess() || !modelShader->LoadSuccess() || !skyboxShader->LoadSuccess()) {
		return;
	}
	SetTextureRepeating(earthTex, true); 
	SetTextureRepeating(earthBump, true); 
	SetTextureRepeating(grassTex, true);
	SetTextureRepeating(grassBump, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(waterTex2, true);
	SetTextureRepeating(waterBump, true);
	SetTextureRepeating(crystalTex, true);
	SetTextureRepeating(crystalBump, true);
	SetTextureRepeating(environMap, true);

	cube = Mesh::LoadFromMeshFile("OffsetCubeY.msh");

	for (int i = 0; i < 30; ++i) {
		SceneNode* s = new SceneNode(cube, Vector4(1, 0, 1, 1), 5);
		s->SetTransform(Matrix4::Translation(Vector3((rand() % 32) + 3343, 253, (rand() % 240) - 20 + 1407)) * Matrix4::Rotation((rand() % 180) - 90, Vector3(1, 0, 0)) * Matrix4::Rotation((rand() % 180) - 90, Vector3(0, 0, 1)));
		s->SetModelScale(Vector3((rand() % 8) + i, (rand() % 20) + i, (rand() % 8) + i));
		root->AddChild(s);
	}

	mesh = Mesh::LoadFromMeshFile("Role_T.msh");
	anim = new MeshAnimation("Role_T.anm");
	material = new MeshMaterial("Role_T.mat");

	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);
	}

	SceneNode* model = new SceneNode(mesh, Vector4(1, 1, 1, 1), 4);
	model->SetTransform(Matrix4::Translation(Vector3(3293, 252, 1403)) * Matrix4::Rotation(-71.42857, Vector3(0, 1, 0)));
	model->SetModelScale(Vector3(100, 100, 100));
	root->AddChild(model);

	/*tree = Mesh::LoadFromMeshFile("Tree1.msh");
	treeMaterial = new MeshMaterial("Tree1.mat");

	for (int i = 0; i < tree->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = treeMaterial->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		treeMatTextures.emplace_back(texID);
	}

	SceneNode* treeModel = new SceneNode(tree, Vector4(1, 1, 1, 1), 4);
	treeModel->SetTransform(Matrix4::Translation(Vector3(3293, 252, 1403)) * Matrix4::Rotation(-71.42857, Vector3(0, 1, 0)));
	treeModel->SetModelScale(Vector3(100, 100, 100));
	root->AddChild(treeModel);*/

	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	vector<Node> nodes;
	nodes.push_back(Node(Vector3(20, 270, 20), 210));
	nodes.push_back(Node(Vector3(270, 260, 1900), 270));
	nodes.push_back(Node(Vector3(600, 260, 1532), 305));
	nodes.push_back(Node(Vector3(1345, 270, 1784), 225));
	nodes.push_back(Node(Vector3(2084, 295, 1325), 260));
	nodes.push_back(Node(Vector3(2806, 320, 1406), 260));

	camera = new Camera(0, nodes);

	light = new Light(heightmapSize * Vector3(0.5f, 2.5f, 0.5f), Vector4(1, 1, 1, 1), heightmapSize.x * 1.65f);
	light2 = new Light(Vector3(3293, 254, 1407), Vector4(1, 0, 1, 1), 400);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	/* glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO); 
	glGenFramebuffers(1, &processFBO);
	
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) {
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

	lightRotate = 0.0f;
	waterRotate = 0.0f;
	waterCycle = 0.0f;
	currentFrame = 0;
	frameTime = 0.0f;

	init = true;
}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete quad;
	delete cube;
	delete mesh;
	delete anim;
	delete material;
	delete tree;
	delete treeMaterial;
	delete light;
	delete light2;

	delete terrainShader;
	delete waterShader;
	delete crystalShader;
	delete modelShader;
	delete skyboxShader;
	//delete sceneShader;
	//delete processShader;
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	lightRotate += dt * 2.0f;
	waterRotate += dt * 2.0f;
	waterCycle += dt * 0.15f;

	frameTime -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % anim->GetFrameCount();
		frameTime += 1.0f / anim->GetFrameRate();
	}

	root->Update(dt);
}

void Renderer::RenderScene() {
	BuildNodeLists(root);
	SortNodeLists();

	//glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	DrawNode(root);
	// DrawNodes();

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//PostProcess();
	//PresentScene();
	
	ClearNodeLists();
}

void Renderer::DrawNode(SceneNode* n) {
	switch (n->type) {
	case 1:
		DrawSkybox(n);
		break;
	case 2:
		DrawTerrain(n);
		break;
	case 3:
		DrawWater(n);
		break;
	case 4:
		DrawModel(n);
		break;
	case 5:
		DrawCrystals(n);
		break;
	}

	for (vector <SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}

void Renderer::DrawSkybox(SceneNode* n) {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	n->Draw(*this);

	glDepthMask(GL_TRUE);
}

void Renderer::DrawTerrain(SceneNode* n) {
	BindShader(terrainShader);
	SetShaderLight(*light);
	SetShaderLight2(terrainShader, *light2);

	glUniform3fv(glGetUniformLocation(terrainShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(terrainShader->GetProgram(), "diffuseTexE"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glUniform1i(glGetUniformLocation(terrainShader->GetProgram(), "bumpTexE"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);

	glUniform1i(glGetUniformLocation(terrainShader->GetProgram(), "diffuseTexG"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, grassTex);

	glUniform1i(glGetUniformLocation(terrainShader->GetProgram(), "bumpTexG"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, grassBump);

	glUniform3fv(glGetUniformLocation(terrainShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
	textureMatrix.ToIdentity();

	glUniformMatrix4fv(glGetUniformLocation(terrainShader->GetProgram(), "lightRot"), 1, false, Matrix4::Rotation(lightRotate, Vector3(1, 0, 0)).values);

	UpdateShaderMatrices();

	n->Draw(*this);
}

void Renderer::DrawWater(SceneNode* n) {
	BindShader(waterShader);
	SetShaderLight(*light);
	SetShaderLight2(waterShader, *light2);

	Vector3 heightmapSize = heightMap->GetHeightmapSize();

	glUniform3fv(glGetUniformLocation(waterShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform3fv(glGetUniformLocation(waterShader->GetProgram(), "heightmapDim"), 1, (float*)&heightmapSize);

	glUniform1i(glGetUniformLocation(waterShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);
	glUniform1i(glGetUniformLocation(waterShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterBump);
	glUniform1i(glGetUniformLocation(waterShader->GetProgram(), "cubeTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	glUniform1i(glGetUniformLocation(waterShader->GetProgram(), "altTex"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, waterTex2);
	glUniform1i(glGetUniformLocation(waterShader->GetProgram(), "environTex"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, environMap);

	modelMatrix = Matrix4::Translation(heightmapSize * 0.5f) * Matrix4::Scale(heightmapSize * 0.5f) * Matrix4::Rotation(90, Vector3(-1, 0, 0));
	textureMatrix = Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) * Matrix4::Scale(Vector3(1, 1, 1)) * Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	glUniformMatrix4fv(glGetUniformLocation(waterShader->GetProgram(), "lightRot"), 1, false, Matrix4::Rotation(lightRotate, Vector3(1, 0, 0)).values);

	UpdateShaderMatrices();

	n->Draw(*this);
}

void Renderer::DrawCrystals(SceneNode* n) {
	BindShader(crystalShader);
	SetShaderLight(*light);
	SetShaderLight2(crystalShader, *light2);

	Vector3 heightmapSize = heightMap->GetHeightmapSize();

	glUniform3fv(glGetUniformLocation(crystalShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform3fv(glGetUniformLocation(crystalShader->GetProgram(), "heightmapDim"), 1, (float*)&heightmapSize);

	glUniform1i(glGetUniformLocation(crystalShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, crystalTex);
	glUniform1i(glGetUniformLocation(crystalShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);
	glUniform1i(glGetUniformLocation(crystalShader->GetProgram(), "cubeTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
	textureMatrix.ToIdentity();

	glUniformMatrix4fv(glGetUniformLocation(crystalShader->GetProgram(), "lightRot"), 1, false, Matrix4::Rotation(lightRotate, Vector3(1, 0, 0)).values);

	UpdateShaderMatrices();

	n->Draw(*this);
}

void Renderer::DrawModel(SceneNode* n) {
	BindShader(modelShader);
	SetShaderLight(*light);
	SetShaderLight2(modelShader, *light2);

	glUniform1i(glGetUniformLocation(modelShader->GetProgram(), "diffuseTex"), 0);

	modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

	glUniformMatrix4fv(glGetUniformLocation(crystalShader->GetProgram(), "lightRot"), 1, false, Matrix4::Rotation(lightRotate, Vector3(1, 0, 0)).values);

	UpdateShaderMatrices();

	vector <Matrix4> frameMatrices;

	Mesh* nodeMesh = n->GetMesh();

	const Matrix4* invBindPose = nodeMesh->GetInverseBindPose();
	const Matrix4* frameData = anim->GetJointData(currentFrame);

	for (unsigned int i = 0; i < nodeMesh->GetJointCount(); ++i) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}
	int j = glGetUniformLocation(modelShader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
	for (int i = 0; i < nodeMesh->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, matTextures[i]);
		nodeMesh->DrawSubMesh(i);
	}
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));
		
		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}
	for (vector <SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes() {
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::SetShaderLight2(Shader* currentShader, const Light& l) {
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "lightPos2"), 1, (float*)&l.GetPosition());

	glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "lightColour2"), 1, (float*)&l.GetColour());

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "lightRadius2"), l.GetRadius());
}

/* void Renderer::PostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	BindShader(processShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(processShader->GetProgram(), "sceneTex"), 0);
	for (int i = 0; i < POST_PASSES; ++i) {
		glUniform1i(glGetUniformLocation(processShader->GetProgram(), "axis"), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
		quad->Draw();

		glUniform1i(glGetUniformLocation(processShader->GetProgram(), "axis"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		quad->Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(sceneShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	quad->Draw();
} */