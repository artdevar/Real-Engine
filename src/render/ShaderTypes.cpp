#include <glad/glad.h>
#include "ShaderTypes.h"
#include "render/Buffer.h"

const unsigned TEXTURE_BASIC_COLOR_UNIT  = GL_TEXTURE1;
const int      TEXTURE_BASIC_COLOR_INDEX = 1;

const unsigned TEXTURE_NORMAL_UNIT  = GL_TEXTURE2;
const int      TEXTURE_NORMAL_INDEX = 2;

const unsigned TEXTURE_EMISSIVE_UNIT  = GL_TEXTURE3;
const int      TEXTURE_EMISSIVE_INDEX = 3;

const unsigned TEXTURE_METALLIC_ROUGHNESS_UNIT  = GL_TEXTURE4;
const int      TEXTURE_METALLIC_ROUGHNESS_INDEX = 4;

const unsigned TEXTURE_OCCLUSION_UNIT  = GL_TEXTURE5;
const int      TEXTURE_OCCLUSION_INDEX = 5;

const unsigned TEXTURE_DEPTH_MAP_UNIT  = GL_TEXTURE19;
const int      TEXTURE_DEPTH_MAP_INDEX = 19;

const unsigned TEXTURE_SHADOW_MAP_UNIT  = GL_TEXTURE20;
const int      TEXTURE_SHADOW_MAP_INDEX = 20;

const unsigned TEXTURE_SKYBOX_UNIT  = GL_TEXTURE21;
const int      TEXTURE_SKYBOX_INDEX = 21;

const unsigned TEXTURE_IRRADIANCE_MAP_UNIT  = GL_TEXTURE22;
const int      TEXTURE_IRRADIANCE_MAP_INDEX = 22;

const unsigned TEXTURE_BLOOM_UNIT  = GL_TEXTURE23;
const int      TEXTURE_BLOOM_INDEX = 23;

const unsigned TEXTURE_VELOCITY_UNIT  = GL_TEXTURE24;
const int      TEXTURE_VELOCITY_INDEX = 24;

const unsigned TEXTURE_TAA_HISTORY_UNIT  = GL_TEXTURE25;
const int      TEXTURE_TAA_HISTORY_INDEX = 25;
