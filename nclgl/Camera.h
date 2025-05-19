# pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include <vector>;

class Camera {
public:
    Camera(void) {
        yaw = 0.0f;
        pitch = 0.0f;
    }

    Camera(float pitch, float yaw, Vector3 position) {
        this->pitch = pitch;
        this->yaw = yaw;
        this->position = position;
    }

    ~Camera(void) {}

    void UpdateCamera(float dt = 1.0f);
    void FreeCamera(float dt = 1.0f);
    void FollowPath(float dt = 1.0f);
    Matrix4 BuildViewMatrix();

	const bool CompareByCameraDistance(const Vector3& a, const Vector3& b) {
		return getDistanceFromCamera(a) < getDistanceFromCamera(b);
	}
	const float getDistanceFromCamera(const Vector3& p) {
		return (p - position).Length();
	}

    Vector3 GetPosition() const { return position; }
    void SetPosition(Vector3 val) { position = val; }


    float GetYaw() const { return yaw; }
    void SetYaw(float y) { yaw = y; }

    float GetPitch() const { return pitch; }
    void SetPitch(float p) { pitch = p; }
    
    void LockCamera();
    std::vector<Vector3> cameraPath;
    

protected:
    bool locked = false;
    float yaw;
    float pitch;
    Vector3 position; // Set to 0,0,0 by Vector3 constructor
    int currentPos = 0;
};
