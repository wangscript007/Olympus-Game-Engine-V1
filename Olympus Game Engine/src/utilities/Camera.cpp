#include "Camera.h"

Camera* Camera::m_Instance = nullptr;

Camera * Camera::Instance() {
	if (m_Instance == nullptr) {
		Logger::Instance()->write("INVALID CAMERA ACCESS");
	}
	else
		return m_Instance;
}

Camera* Camera::Instance(glm::vec3 position, glm::vec3 front, glm::vec3 up, MessageBus* messageBus) {
	if(m_Instance == nullptr)
		m_Instance = new Camera(position, front, up, messageBus);
	return m_Instance;
}




Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, MessageBus* messageBus) : BusNode(messageBus) {
	m_position = position;
	m_front = front;
	m_worldUp = up;

	//constants
	m_yaw = -90.0f;
	m_pitch = 0.0f;
	m_movementSpeed = 10;
	m_mouseSensitivity = 0.5f;
	m_zoom;

	update();
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(m_position,m_position + m_front, m_up);
}
glm::vec3 Camera::getPosition() { return m_position; }


void Camera::onNotify(Message message) {
	handleInput(message.getEvent());
}

void Camera::handleInput(std::string movement) {


	float velocity = m_movementSpeed * Time::Instance()->getDeltaTime();
	if (movement == "CAMERA_W_A") {
		m_position -= m_right * velocity;
		m_position += m_front * velocity;
	}
	else if (movement == "CAMERA_W_D") {
		m_position += m_front * velocity;
		m_position += m_right * velocity;
	}

	else if (movement == "CAMERA_S_D") {
		std::cout << "CAM SD" << std::endl;
	}

	else if (movement == "CAMERA_S_A") {
		std::cout << "CAM SA" << std::endl;
	}

	else if (movement == "CAMERA_LSHIFT")
		m_position -= velocity * glm::vec3(0.0f, 1.0f, 0.0f);
	else if (movement == "CAMERA_SPACE")
		m_position += velocity * glm::vec3(0.0f, 1.0f, 0.0f);
	else if (movement == "CAMERA_W")
		m_position += m_front * velocity;
	else if (movement == "CAMERA_A")
		m_position -= m_right * velocity;
	else if (movement == "CAMERA_S")
		m_position -= m_front * velocity;
	else if (movement == "CAMERA_D")
		m_position += m_right * velocity;
	else if (movement == "CAMERA_GET_POSITION") {
		std::cout << m_position.x << " " << m_position.y << " " << m_position.z << std::endl;
	}
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
	xoffset *= m_mouseSensitivity;
	yoffset *= m_mouseSensitivity;

	m_yaw += xoffset;
	m_pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (m_pitch > 89.0f)
			m_pitch = 89.0f;
		if (m_pitch < -89.0f)
			m_pitch = -89.0f;
	}

	update();
}

void Camera::update_time(float delta) {
	deltaTime = delta;
}

void Camera::update() {
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	m_right = glm::normalize(glm::cross(m_front, m_worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_up = glm::normalize(glm::cross(m_right,m_front));
}