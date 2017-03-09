#ifndef __main_hpp__
#define __main_hpp__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "scene.hpp"
//#include "controller.hpp"

extern ScenePtr scene;
extern LightPtr light;
extern CubeMapPtr env_map;

void toggleWireframe();

#endif // __main_hpp__
