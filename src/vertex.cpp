#include "vertex.h"

Vertex::Vertex() {
	this->t = new Transform(this);
	this->n = Normal();
	this->c = Color();
}

Vertex::Vertex(Transform* t, Color c, Normal n) {
	this->t = t;
	this->n = n;
	this->c = c;
}

Vertex::Vertex(Transform* t, Normal n, Color c) {
	this->t = t;
	this->n = n;
	this->c = c;
}

Vertex::Vertex(Transform* t) {
	this->t = t;
	this->n = Normal();
	this->c = Color();
}

Vertex::Vertex(glm::vec3 p, Transform* parent) {
	this->t = new Transform(p, this, parent);
	this->n = Normal();
	this->c = Color();

}

Vertex::Vertex(glm::vec3 p, int id) {
	this->t = new Transform(p, this);
	this->n = Normal();
	this->c = Color();
	this->id = id;
}

Vertex::Vertex(float p0, float p1, float p2) {
	this->t = new Transform(glm::vec3(p0, p1, p2), this);
	this->n = Normal();
	this->c = Color();
}

Vertex::Vertex(float p0, float p1, float p2, COLORS color_name) {
	this->t = new Transform(glm::vec3(p0, p1, p2), this);
	this->n = Normal();
	this->c = Color(color_name);
}
