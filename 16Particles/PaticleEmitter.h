#pragma once

#include "Vector.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

struct Particle2 {
	Vector3f pos, speed;
	unsigned char r, g, b, a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle2& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

class ParticleEmitter {

public:
	ParticleEmitter();
	~ParticleEmitter();

	void update(const float& dt, const Camera& camera);
	void addParticles(const float& dt);
	void render(const Camera& camera);

private:
	Shader* particleSchader;
	Texture* texture;

	const int MaxParticles = 100000;
	Particle2 ParticlesContainer[100000];
	int LastUsedParticle = 0;

	GLfloat* g_particule_position_size_data;
	GLubyte* g_particule_color_data;
	GLuint billboard_vertex_buffer;
	GLuint particles_position_buffer;
	GLuint particles_color_buffer;

	int FindUnusedParticle();
	void SortParticles();

	int ParticlesCount = 0;
};