#include "Scene.h"

Scene::Scene(DrawingWindow &window, Camera &camera) : window(window), objects(std::vector<Object3d>()), lights(std::vector<Light>()), camera(camera) {}

void Scene::loadModel(std::string fileName) {
	std::ifstream objFile(fileName);
	std::string line;

	while (line.substr(0, 6) != "mtllib" && !objFile.eof()) {
		std::getline(objFile, line);
	}
	std::vector<Colour> colours;
	std::unordered_map<std::string, std::string> textureMap;
	if (!objFile.eof()) {
		std::string mtlFileName = line.substr(7, line.length() - 7);

		std::ifstream mtlFile(mtlFileName);
		std::string currentColourString = "";
		for (int i = 0; i < 5000; i++) {
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
			} else if (line.substr(0, 6) == "map_Kd") {
				std::string textureFileName = line.substr(7, line.length() - 7);
				std::string textureName = currentColourString;
				textureMap[textureName] = textureFileName;
			}

			if (mtlFile.eof()) break;
		}
		mtlFile.close();
	}
	objFile.clear();
	objFile.seekg(0, std::ios::beg);


	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> textureCoords;
	Colour currentColour = Colour("Default", 25, 25, 255);
	if (colours.size() > 0) currentColour = colours[0];
	std::string currentTexture = "";
    Object3d currentObject = Object3d(window);

	float scalingFactor = 0.35f;

	for (int i = 1; i < 5000; i++) {
		std::getline(objFile, line);
		if (line.substr(0, 2) == "v ") {
			std::string x = line.substr(2, line.find(' ', 2) - 2);
			std::string y = line.substr(line.find(' ', 2) + 1, line.find(' ', line.find(' ', 2) + 1) - line.find(' ', 2) - 1);
			std::string z = line.substr(line.find(' ', line.find(' ', 2) + 1) + 1, line.length() - line.find(' ', line.find(' ', 2) + 1) - 1);
			vertices.push_back(glm::vec3(std::stof(x), std::stof(y), std::stof(z)) * scalingFactor);
		}
		else if (line.substr(0, 3) == "vt ") {
			std::string x = line.substr(3, line.find(' ', 3) - 3);
			std::string y = line.substr(line.find(' ', 3) + 1, line.length() - line.find(' ', 3) - 1);
			textureCoords.push_back(glm::vec2(std::stof(x), std::stof(y)));
		}
		else if (line.substr(0, 2) == "f ") {
			std::string v1 = line.substr(2, line.find('/', 2) - 2);
			std::string t1 = line.substr(line.find('/', 2) + 1, line.find(' ', line.find('/', 2) + 1) - line.find('/', 2) - 1);
			std::string v2 = line.substr(line.find(' ', line.find('/', 2) + 1) + 1, line.find('/', line.find(' ', line.find('/', 2) + 1) + 1) - line.find(' ', line.find('/', 2) + 1) - 1);
			std::string t2 = line.substr(line.find('/', line.find(' ', line.find('/', 2) + 1) + 1) + 1, line.find(' ', line.find('/', line.find(' ', line.find('/', 2) + 1) + 1) + 1) - line.find('/', line.find(' ', line.find('/', 2) + 1) + 1) - 1);
			std::string v3 = line.substr(line.find(' ', line.find('/', line.find(' ', line.find('/', 2) + 1) + 1) + 1) + 1, line.find('/', line.find(' ', line.find('/', line.find(' ', line.find('/', 2) + 1) + 1) + 1) + 1) - line.find(' ', line.find('/', line.find(' ', line.find('/', 2) + 1) + 1) + 1) - 1);
			std::string t3 = line.substr(line.find('/', line.find(' ', line.find('/', line.find(' ', line.find('/', 2) + 1) + 1) + 1) + 1) + 1, line.length() - line.find('/', line.find(' ', line.find('/', line.find(' ', line.find('/', 2) + 1) + 1) + 1) + 1) - 1);

			ModelTriangle t = ModelTriangle(vertices[std::stoi(v1) - 1], vertices[std::stoi(v2) - 1], vertices[std::stoi(v3) - 1], currentColour);
			if (t1 != "" && t2 != "" && t3 != "" && textureCoords.size() > 0 && currentTexture != "") {
				std::array<TexturePoint, 3> texturePoints;
				texturePoints[0] = TexturePoint(textureCoords[std::stoi(t1) - 1].x, textureCoords[std::stoi(t1) - 1].y);
				texturePoints[1] = TexturePoint(textureCoords[std::stoi(t2) - 1].x, textureCoords[std::stoi(t2) - 1].y);
				texturePoints[2] = TexturePoint(textureCoords[std::stoi(t3) - 1].x, textureCoords[std::stoi(t3) - 1].y);
				t.texturePoints = texturePoints;
				t.textureMap = TextureMap(currentTexture);
			}

			t.normal = glm::normalize(glm::cross(t.vertices[1] - t.vertices[0], t.vertices[2] - t.vertices[0]));

            currentObject.addTriangle(t);
		}
		else if (line.substr(0, 7) == "usemtl ") {
			std::string colour = line.substr(7, line.length() - 7);
			for (long unsigned int j = 0; j < colours.size(); j++) {
				if (colours[j].name == colour) currentColour = colours[j];
			}
            
			// Check if texture
			if (textureMap.find(colour) != textureMap.end()) 
                currentTexture = textureMap[colour];
			else
                currentTexture = "";
		}
        else if (line.substr(0, 2) == "o ") {
            if (currentObject.triangles.size() > 0) {
                this->addObject(currentObject);
                currentObject = Object3d(window);
            }
        }
		if (objFile.eof()) {
			break;
		}
	}
	this->addObject(currentObject);

	objFile.close();

	// Calculating vertex normals
	for (size_t i = 0; i < objects.size(); i++) {
		Object3d &object = objects[i];
		for (size_t j = 0; j < object.triangles.size(); j++) {
			ModelTriangle triangle = object.triangles[j];
			for (size_t k = 0; k < 3; k++) {
				glm::vec3 vertex = triangle.vertices[k];
				glm::vec3 normal = glm::vec3(0, 0, 0);
				for (size_t l = 0; l < object.triangles.size(); l++) {
					ModelTriangle otherTriangle = object.triangles[l];
					if (otherTriangle.vertices[0] == vertex || otherTriangle.vertices[1] == vertex || otherTriangle.vertices[2] == vertex) {
						normal += otherTriangle.normal;
					}
				}
				normal = glm::normalize(normal);
				triangle.vertexNormals[k] = normal;
			}
			object.triangles[j] = triangle;
		}
		object.updateBoundingBox();
	}
}

void Scene::addLight(Light light) {
    lights.push_back(light);
}

void Scene::addObject(Object3d object) {
    objects.push_back(object);
}

void Scene::clearScene() {
    objects.clear();
    lights.clear();
}