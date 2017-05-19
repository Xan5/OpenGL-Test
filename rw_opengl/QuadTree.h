class Node {
public:
	GLint level, maxLevel;
	glm::vec3 v;
	GLfloat offSet, side;
	glm::mat4 translation;
	glm::mat4 rotation;
	GLboolean object = false;
	Node * NW;
	Node * NE;
	Node * SW;
	Node * SE;
public:
	Node(GLfloat _x, GLfloat _y, GLint _level, GLfloat _side, GLint _maxLevel) : 
		NW(NULL), NE(NULL), SW(NULL), SE(NULL), level(_level), side(_side), maxLevel(_maxLevel) 
	{ v = (glm::vec3(_x, 0, _y)); }

	~Node() { delete NW, NE, SW, SE; }

	void generateTree() {
		offSet = side / (2 * pow(2, level + 1));
		NW = new Node(v.x - offSet, v.z + offSet, level + 1, side, maxLevel);
		NE = new Node(v.x + offSet, v.z + offSet, level + 1, side, maxLevel);
		SW = new Node(v.x - offSet, v.z - offSet, level + 1, side, maxLevel);
		SE = new Node(v.x + offSet, v.z - offSet, level + 1, side, maxLevel);
		if (level + 1 < (maxLevel)) {
			NW->generateTree();
			NE->generateTree();
			SW->generateTree();
			SE->generateTree();
		}
	}

	void makeMats(GLfloat angle) {
		translation = glm::translate(translation, glm::vec3(v.x, 0.0f, v.z));
		rotation = glm::rotate(rotation, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void renderNodes(GLint _modelLoc, GLint _count, glm::vec3 camFront, glm::vec3 camPos, GLfloat param){
		if (this->NW != NULL) {
			camFront.y = 0;
			camPos.y = 0;
			glm::vec3 treeDir = glm::normalize(camPos - v);
			//test widocznosci
			if ((camPos.x >= v.x - offSet && camPos.x <= v.x + offSet &&
				camPos.z >= v.z - offSet && camPos.z <= v.z + offSet) ||
				dot(camFront, treeDir) <= param) {
				NW->renderNodes(_modelLoc, _count, camFront, camPos, param / 1.2);
				NE->renderNodes(_modelLoc, _count, camFront, camPos, param / 1.2);
				SW->renderNodes(_modelLoc, _count, camFront, camPos, param / 1.2);
				SE->renderNodes(_modelLoc, _count, camFront, camPos, param / 1.2);
			}
			
		}
		else {
				if (object == 0) return;
				glm::mat4 model;
				model *= translation * rotation;
				glUniformMatrix4fv(_modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, _count);
		}
	}

	int addRandomTree() {
		if (NW != NULL) {
			int q = rand() % 4;
			switch (q)
			{
			case 0: {return NW->addRandomTree(); }
			case 1: {return NE->addRandomTree(); }
			case 2: {return SW->addRandomTree(); }
			case 3: {return SE->addRandomTree(); }
			default:
				return 0;
			}
		}else
		if (object == false) {
			float a = rand() % 360;
			makeMats(a);
			object = true;
			return 1;
		}
		else return 0;
	}
};