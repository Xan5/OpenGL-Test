class Node {
public:
	GLint level;
	GLfloat x, y;
	glm::mat4 translation;
	glm::mat4 rotation;
	GLboolean object = false;
	Node * NW;
	Node * NE;
	Node * SW;
	Node * SE;
public:
	Node(GLfloat _x, GLfloat _y, GLint _level) : NW(NULL), NE(NULL), SW(NULL), SE(NULL), x(_x), y(_y), level(_level) {}
	~Node() { delete NW, NE, SW, SE; }
	void make4Nodes(GLfloat x, GLfloat y, GLfloat q, GLint lev) {
		this->NW = new Node(x - q, y + q, lev + 1);
		this->NE = new Node(x + q, y + q, lev + 1);
		this->SW = new Node(x - q, y - q, lev + 1);
		this->SE = new Node(x + q, y - q, lev + 1);
	}
	void makeMats(float angle) {
		translation = glm::translate(translation, glm::vec3(x, 0.0f, y));
		rotation = glm::rotate(rotation, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	}
};

class QuadTree {
public:
	Node * root;
	GLfloat side;
	GLint maxLevel;
	QuadTree(GLfloat _side, GLint _maxlevel): side(_side), maxLevel(_maxlevel) {
		root = new Node(0,0,0);
	}
	void generateTree(Node * parent) {
		parent->make4Nodes(parent->x, parent->y, side/(2*pow(2,parent->level+1)),parent->level);
		if (parent->level + 1 < (maxLevel)) {
			generateTree(parent->NW);
			generateTree(parent->NE);
			generateTree(parent->SW);
			generateTree(parent->SE);
		}
	}
	int addRandomTree() {
		Node * n = root;
		int q = 0;
		for (int i = 0; i < maxLevel; i++) {
			q = rand() % 4;
			switch (q)
			{
			case 0: {n = n->NW; break; }
			case 1: {n = n->NE; break; }
			case 2: {n = n->SW; break; }
			case 3: {n = n->SE; break; }
			default:
				return 0;
			}
		}
		if (n->object == false) {
			float a = rand() % 360;
			n->makeMats(a);
			n->object = true;
			return 1;
		}
		else return 0;

	}
	void renderObjects(Node * node, GLint _modelLoc, GLint _count) {
		if (node->NW != NULL && node->NE != NULL && node->SW != NULL && node->SE != NULL) {
			renderObjects(node->NW, _modelLoc, _count);
			renderObjects(node->NE, _modelLoc, _count);
			renderObjects(node->SW, _modelLoc, _count);
			renderObjects(node->SE, _modelLoc, _count);
		}else{
			if (node->object == 0) return;
			glm::mat4 model;
			model *= node->translation * node->rotation;
			glUniformMatrix4fv(_modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, _count);
		}
	}

	~QuadTree() { delete root; }
};