#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include <vector>

using namespace std;

struct Node {
	Vector3 position;
	float yaw;

	Node() {
		position = Vector3(0, 0, 0);
		yaw = 0;
	}

	Node(Vector3 pos, float rot) {
		position = pos;
		yaw = rot;
	}
};

class Camera {
public:
	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
		multiplier = 0.0f;
		fixed = false;
	};
	
	Camera(float pitch, vector<Node> nodes) {
		this->pitch = pitch;
		this->yaw = nodes.at(0).yaw;
		this->position = nodes.at(0).position;
		multiplier = 0.0f;
		fixed = true;
		this->nodes = nodes;
		index = 0;
	}

	Camera(float pitch, float yaw, Vector3 position) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
		multiplier = 0.0f;
	}
	~Camera(void) {};
	void UpdateCamera(float dt = 1.0f);

	Matrix4 BuildViewMatrix();
	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }
	float GetYaw() const { return yaw; }
	void SetYaw(float y) { yaw = y; }
	float GetPitch() const { return pitch; }
	void SetPitch(float p) { pitch = p; }

protected:
	float yaw;
	float pitch;
	Vector3 position;
	float multiplier;
	bool fixed;
	vector<Node> nodes;
	int index;
};