#ifndef _SUBDIVISIONSURFACE_H_
#define _SUBDIVISIONSURFACE_H_

#define PI 3.14159265359
#include "vec.h"
#include <vector>
#include <math.h>
#include <FL/gl.h>

using namespace std;
class Triface;
class Vertex {
private:
	Vec3f position;
	vector<Vertex*> neighbours;
	float alpha() {
		int n = neighbours.size();
		float betaN = 5 / 4.0f - pow(3 + 2 * cosf(2 * PI / n), 2) / 32.0f;
		return n * (1 - betaN) / betaN;
	}
public:
	static vector<float> averageMask;

	Vertex(Vec3f pos) { position = pos; }

	void updateNeighbour(Vertex* origin, Vertex* newOne){
		for (Vertex* p : neighbours) 
			if (p == origin)
				p = newOne;
	}

	void addNeightBour(Vertex* neighbour) {
		neighbours.push_back(neighbour);
	}

	void average() {
		float alphaN = alpha();
		int n = neighbours.size();
		Vec3f sum;
		for (int i = 0; i < n; i++) {
			float factor = 1;
			if (averageMask.size() > i)
				factor = averageMask[i];
			sum += neighbours[i]->position * factor;
		}
		sum += alphaN * position;
		position = sum / (alphaN + n);
	}

	Vec3f& getPosition() {
		return position;
	}
};

class Edge {
private:
	Edge* subEdges[2];
	Vertex* midPoint;
	Vertex* vertices[2];
	Triface* faces[2];
	bool hasBeenAdded;
public:
	Edge(Vertex* v1, Vertex* v2) {
		vertices[0] = v1;
		vertices[1] = v2;
		subEdges[0] == NULL;
		subEdges[1] == NULL;
		midPoint = NULL;
		faces[0] = NULL;
		faces[1] = NULL;
		hasBeenAdded = false;

		//v1 and v2 are now new neighbours 
		v1->addNeightBour(v2);
		v2->addNeightBour(v1);
	}

	void addMidPoint() {
		if (midPoint != NULL)
			return;
		midPoint = new Vertex((vertices[0]->getPosition() + vertices[1]->getPosition()) / 2.0f);
		subEdges[0] = new Edge(vertices[0], midPoint);
		subEdges[1] = new Edge(midPoint, vertices[1]);
	}

	void addFaces(Triface* face) {
		if (faces[0] == NULL)
			faces[0] = face;
		if (faces[1] == NULL)
			faces[1] = face;
	}

	bool has(Vertex* v) {
		if (vertices[0] == v)
			return true;
		if (vertices[1] == v)
			return true;
		return false;
	}

	Edge** getSubEdges() {
		return subEdges;
	}

	Edge* getSubEdge(Vertex* v) {//get the sub edge containing this vertex
		if (midPoint == NULL)//no subedge
			return NULL;
		if (subEdges[0]->has(v))
			return subEdges[0];
		if (subEdges[1]->has(v))
			return subEdges[1];
		return NULL;
	}

	Vertex* getMidPoint() {
		return midPoint;
	}

	Vertex* getAnotherVertex(Vertex* v) {
		if (vertices[0] == v)
			return vertices[1];
		if (vertices[1] == v)
			return vertices[0];
		return NULL;
	}

	void addToThis(vector<Edge*>& list) {
		if (hasBeenAdded)
			return;
		hasBeenAdded = true;
		list.push_back(this);
	}
	void addMidPointToThis(vector<Vertex*>& list) {
		list.push_back(midPoint);
	}

	Vertex* get(int n) {
		return vertices[n];
	}

	void updateNeighbour() {
		vertices[0]->updateNeighbour(vertices[1], midPoint);
		vertices[1]->updateNeighbour(vertices[0], midPoint);
	}

	void addSubEdgeToThis(vector<Edge*>& list) {
		list.push_back(subEdges[0]);
		list.push_back(subEdges[1]);
	}
};

class Triface {
private:
	Edge* edges[3];
	Triface* subfaces[4];
	vector<Edge*> newEdges;
	Triface* getSubface(Vertex* v) {//create a surface containing this vertex
		Edge* subEdge[2];
		int index = 0;
		for (int i = 0; i < 3; i++) {
			Edge* cur = edges[i]->getSubEdge(v);
			if (cur != NULL) {
				subEdge[index] = cur;
				index++;
			}
		}
		//the index should be 2 now
		Edge* newEdge = new Edge(subEdge[0]->getAnotherVertex(v), subEdge[1]->getAnotherVertex(v));
		newEdges.push_back(newEdge);
		Triface* t = new Triface(subEdge[0], subEdge[1], newEdge);
		return t;
	}

	Vertex** getVertices() {
		Vertex* vertices[3];
		vertices[0] = edges[0]->get(0);
		vertices[1] = edges[0]->get(1);
		vertices[2] = edges[1]->getAnotherVertex(vertices[0]);
		if (vertices[2] == NULL)
			vertices[2] = edges[2]->getAnotherVertex(vertices[0]);
		return vertices;
	}

	void TriNeighbour(Vertex* v1, Vertex* v2, Vertex* v3) {
		v1->addNeightBour(v2);
		v1->addNeightBour(v3);

		v2->addNeightBour(v1);
		v2->addNeightBour(v3);

		v3->addNeightBour(v1);
		v3->addNeightBour(v2);
	}
public:
	Triface(Edge* edge1, Edge* edge2, Edge* edge3) {
		edges[0] = edge1;
		edge1->addFaces(this);
		edges[1] = edge2;
		edge2->addFaces(this);
		edges[2] = edge3;
		edge3->addFaces(this);
	}

	void split() {
		for (int i = 0; i < 3; i++) 
			edges[i]->addMidPoint();

		Vertex** vertices = getVertices();

		//add 3 faces 
		for (int i = 0; i < 3; i++) {
			subfaces[i] = getSubface(vertices[i]);
		}
		//add the center face
		subfaces[4] = new Triface(newEdges[0], newEdges[1], newEdges[2]);

		//the three new points are neighbours of each other
		TriNeighbour(edges[0]->getMidPoint(), edges[1]->getMidPoint(), edges[2]->getMidPoint());
	}

	void addSubfaceToThis(vector<Triface*>& list) {
		for (int i = 0; i < 4; i++) {
			list.push_back(subfaces[i]);
		}
		
	}

	void draw() {
		Vertex** vertices = getVertices();
		glBegin(GL_TRIANGLES);
		glVertex3fv(vertices[0]->getPosition().getPointer());
		glVertex3fv(vertices[1]->getPosition().getPointer());
		glVertex3fv(vertices[2]->getPosition().getPointer());
		glEnd();
	}
};

class Diamond {
private:
	vector<Triface*> faces;
	vector<Vertex*>vertices;
	vector<Edge*> edges;
public:
	Diamond(Vec3f* positions) {
		//#0 & #5: position of vertex points
		//#1 ~ # 4: position of edge points	
		for (int i = 0; i < 6; i++) 
			vertices.push_back(new Vertex(positions[i]));
		
		//#0 ~ #3: upper pyramid edge
		//#4 ~ #7: middle edge
		//#8 ~ #11: lower pyramid edge
		for (int i = 1; i < 5; i++)
			edges.push_back(new Edge(vertices[0], vertices[i]));
		for (int i = 1; i < 5; i++) {
			int next = i + 1;
			if (next > 4)//loop back
				next = 1;
			edges.push_back(new Edge(vertices[i], vertices[next]));
		}
		for (int i = 1; i < 5; i++)
			edges.push_back(new Edge(vertices[5], vertices[i]));

		//add faces
		for (int i = 0; i < 4; i++)
			faces.push_back(new Triface(edges[i], edges[(i + 1) % 4], edges[i + 4]));
		for (int i = 8; i < 12; i++)
			faces.push_back(new Triface(edges[i], edges[(i - 8) % 4 + 8], edges[i - 4]));
	}
	void draw() {
		for (Triface* t : faces)
			t->draw();
	}
	void split(int n) {
		for (int i = 0; i < n; i++) {
			vector<Triface*> newFaces;
			vector<Edge*> newEdges;
			for (Triface* t : faces) {
				t->split();
				t->addSubfaceToThis(newFaces);
			}

			//update the neighbour and the edges
			for (Edge* e : edges) {
				e->updateNeighbour();
				e->addSubEdgeToThis(newEdges);
			}

			//averaging the old vertices
			for (Vertex* v : vertices) 
				v->average();
			
			for (Edge* e : edges)
				e->addMidPointToThis(vertices);

			//delete the old faces and edges
			for (Triface* t : faces) 
				delete t;
			for (Edge* e : edges)
				delete e;

			faces = newFaces;
			edges = newEdges;
		}
	}
};
#endif
