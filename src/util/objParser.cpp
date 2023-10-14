#include "objParser.h"


std::vector<ModelTriangle> loadObjFile(std::string fileName) {
	std::vector<ModelTriangle> triangles;

	std::ifstream file(fileName);
	std::string line;

	std::vector<glm::vec3> vertices;

	Colour currentColour = Colour(255, 255, 255);

	for (int i = 0; i < 1000; i++) {
		std::getline(file, line);
		if (line[0] == 'v') {
			std::string x = line.substr(2, line.find(' ', 2) - 2);
			std::string y = line.substr(line.find(' ', 2) + 1, line.find(' ', line.find(' ', 2) + 1) - line.find(' ', 2) - 1);
			std::string z = line.substr(line.find(' ', line.find(' ', 2) + 1) + 1, line.length() - line.find(' ', line.find(' ', 2) + 1) - 1);
			vertices.push_back(glm::vec3(std::stof(x), std::stof(y), std::stof(z)));
		}
		else if (line[0] == 'f') {
			std::string v1 = line.substr(2, line.find(' ', 2) - 2);
			std::string v2 = line.substr(line.find(' ', 2) + 1, line.find(' ', line.find(' ', 2) + 1) - line.find(' ', 2) - 1);
			std::string v3 = line.substr(line.find(' ', line.find(' ', 2) + 1) + 1, line.length() - line.find(' ', line.find(' ', 2) + 1) - 1);
			triangles.push_back(ModelTriangle(vertices[std::stoi(v1) - 1], vertices[std::stoi(v2) - 1], vertices[std::stoi(v3) - 1], currentColour));
		}
	}

	return triangles;
}
