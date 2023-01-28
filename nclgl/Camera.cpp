#include "Camera.h"
#include "Window.h"
#include <algorithm>
void Camera::UpdateCamera(float dt) {
	if (!fixed) {
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);
	}

	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_M)) {
		multiplier += 5.0f;
		multiplier = std::min(multiplier, 660.0f);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_N)) {
		multiplier -= 5.0f;
		multiplier = std::max(multiplier, 0.0f);
	}

	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);
	float speed = (30.0f + multiplier) * dt;

	if (!fixed) {
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
			position += forward * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
			position -= forward * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
			position -= right * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
			position += right * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
			position.y += speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
			position.y -= speed;
		}
	}
	else {
		if (index >= nodes.size() - 1) {
			fixed = false;
			return;
		}

		Node current = nodes.at(index);
		Node next = nodes.at(index + 1);
		Vector3 direction = next.position - current.position;
		direction.Normalise();
		Vector3 movement = direction * speed;
		position += movement;

		if (next.yaw - yaw > 0.1f) {
			float angledDifference = next.yaw - yaw;
			angledDifference < -180 ? angledDifference += 360 : angledDifference > 180 ? angledDifference -= 360 : angledDifference;
			float increments = ((next.position - position) / movement).x;
			float angle = angledDifference / increments;
			yaw += angle;
		}

		if ((next.position - position).Length() < movement.Length()) {
			index += 1;
			position += next.position - position;
			yaw = nodes.at(index).yaw;
		}
	}
}

Matrix4 Camera::BuildViewMatrix() {
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
};