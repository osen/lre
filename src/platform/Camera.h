#include "math.h"

void _CameraInitialize();
void _CameraCleanup();

void _CameraApplyProjection();
void _CameraApplyView();

void CameraSetPosition(struct Vector3 position);
void CameraSetRotation(struct Vector3 rotation);
