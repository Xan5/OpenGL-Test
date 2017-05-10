class HierarchyTree {
private:
	NodeTree * root;
	glm::vec3 cameraPos;
	float maxH;
	float maxW;
public:
	HierarchyTree() : cameraPos(glm::vec3(0.0f, 0.0f, 0.0f)), maxH(1000), maxW(1000) {
		root = new NodeTree(glm::vec3(0.0f, 0.0f, 0.0f), 0);
	}
	HierarchyTree(glm::vec3 rootPos, glm::vec3 cameraP= glm::vec3(0.0f, 0.0f, 0.0f)) : cameraPos(cameraP), maxH(1000), maxW(1000) {
		root = new NodeTree(rootPos, 0);
	}
	void addNode(glm::vec3 position, float angle) {
		//gowno

	}
	~HierarchyTree() { delete root; }//
};

class NodeTree {
private:
	glm::mat4 translation;
	glm::mat4 rotation;

	int level = 0;
	NodeTree * parent;
	NodeTree * child1;
	NodeTree * child2;
public:
	NodeTree() : parent(NULL), child1(NULL), child2(NULL) {}
	NodeTree(glm::vec3 position, float angle) : parent(NULL), child1(NULL), child2(NULL) {
		translation = glm::translate(translation, position);
		rotation = glm::rotate(rotation, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	~NodeTree() { delete parent, child1, child2; }
};