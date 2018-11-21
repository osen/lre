#ifndef PLATFORM_PLATFORM_H
#define PLATFORM_PLATFORM_H

#include "Environment.h"
#include "Actor.h"
#include "Cache.h"
#include "Image.h"
#include "Font.h"
#include "error.h"
#include "Gui.h"
#include "math.h"
#include "util.h"
#include "Camera.h"
#include "Model.h"
#include "ModelRenderer.h"
#include "Transform.h"
#include "Debug.h"
#include "Input.h"
#include "BoxCollider.h"
#include "ModelCollider.h"
#include "Animation.h"
#include "Gravity.h"

#include "h.out.h"

void PlatformInitialize(int argc, char *argv[]);
void PlatformCleanup();
void PlatformRun();

#endif
