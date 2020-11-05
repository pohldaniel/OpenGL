#include <iostream>
#include "Animation.h"

Animation::Animation(ColladaLoader loader){
	
	loader.loadAnimation(m_keyFrames, m_duration, m_name);
}
