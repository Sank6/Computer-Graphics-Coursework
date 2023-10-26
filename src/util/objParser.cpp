#include "objParser.h"


std::vector<ModelTriangle> loadObjFile(std::string fileName) {
	std::vector<ModelTriangle> triangles;

	std::ifstream objFile(fileName);
	std::string line;

	while (line.substr(0, 6) != "mtllib") {
		std::getline(objFile, line);
	}
	std::string mtlFileName = line.substr(7, line.length() - 7);
	
	std::ifstream mtlFile(mtlFileName);
	std::string currentColourString = "";
	std::vector<Colour> colours;
	for (int i = 0; i < 1000; i++) {
		std::getline(mtlFile, line);
		if (line.substr(0, 6) == "newmtl") {
			currentColourString = line.substr(7, line.length() - 7);
		}
		else if (line.substr(0, 2) == "Kd") {
			int index = 3;

			std::string rString = line.substr(index, line.find(' ', index) - index);
			int r = round(std::stof(rString) * 255);
			index = line.find(' ', index) + 1;

			std::string gString = line.substr(index, line.find(' ', index) - index);
			int g = round(std::stof(gString) * 255);
			index = line.find(' ', index) + 1;

			std::string bString = line.substr(index, line.length() - index);
			int b = round(std::stof(bString) * 255);
			
			Colour nc = Colour(currentColourString, r, g, b);
			colours.push_back(nc);
		}

		if (mtlFile.eof()) {
			break;
		}
	}

	objFile.clear();
	objFile.seekg(0, std::ios::beg);


	std::vector<glm::vec3> vertices;
	Colour currentColour = colours[0];

	float scalingFactor = 0.35f;

	for (int i = 1; i < 1000; i++) {
		std::getline(objFile, line);
		if (line[0] == 'v') {
			std::string x = line.substr(2, line.find(' ', 2) - 2);
			std::string y = line.substr(line.find(' ', 2) + 1, line.find(' ', line.find(' ', 2) + 1) - line.find(' ', 2) - 1);
			std::string z = line.substr(line.find(' ', line.find(' ', 2) + 1) + 1, line.length() - line.find(' ', line.find(' ', 2) + 1) - 1);
			vertices.push_back(glm::vec3(std::stof(x), std::stof(y), std::stof(z)) * scalingFactor);
		}
		else if (line[0] == 'f') {
			std::string v1 = line.substr(2, line.find(' ', 2) - 2);
			std::string v2 = line.substr(line.find(' ', 2) + 1, line.find(' ', line.find(' ', 2) + 1) - line.find(' ', 2) - 1);
			std::string v3 = line.substr(line.find(' ', line.find(' ', 2) + 1) + 1, line.length() - line.find(' ', line.find(' ', 2) + 1) - 1);
			triangles.push_back(ModelTriangle(vertices[std::stoi(v1) - 1], vertices[std::stoi(v2) - 1], vertices[std::stoi(v3) - 1], currentColour));
		}
		else if (line.substr(0, 6) == "usemtl") {
			std::string colour = line.substr(7, line.length() - 7);
			for (int j = 0; j < colours.size(); j++) {
				if (colours[j].name == colour) {
					currentColour = colours[j];
				}
			}
		}

		if (objFile.eof()) {
			break;
		}
	}

	return triangles;
}
