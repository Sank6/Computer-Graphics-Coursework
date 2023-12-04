#include "Scene.h"

#define MAX_LINES 10000

Scene::Scene(DrawingWindow &window, Camera &camera) : window(window), objects(std::vector<Object3d>()), lights(std::vector<Light>()), camera(camera) {
  this->shadowPass = true;
  this->specularPass = true;
  this->falloffPass = true;
  this->aoiPass = true;
  this->ambientPass = true;
  this->reflectionPass = true;
  this->refractionPass = true;

  this->backgroundColour = Colour("Default", 0, 0, 0);
}

void Scene::loadModel(std::string fileName, float scalingFactor) {
  std::ifstream objFile(fileName);
  std::string line;

  // Check if file exists
  if (!objFile.good()) {
    std::cout << "File does not exist" << std::endl;
    return;
  }

  while (line.substr(0, 6) != "mtllib" && !objFile.eof()) {
    std::getline(objFile, line);
  }
  std::vector<Colour> colours;
  std::unordered_map<std::string, std::string> textureMap;
  if (!objFile.eof()) {
    std::string mtlFileName = line.substr(7, line.length() - 7);

    std::ifstream mtlFile(mtlFileName);

    if (mtlFile.good()) {
      std::string currentColourString = "";
      for (int i = 0; i < MAX_LINES; i++) {
        std::getline(mtlFile, line);
        if (line.substr(0, 6) == "newmtl") {
          currentColourString = line.substr(7, line.length() - 7);
        } else if (line.substr(0, 2) == "Kd") {
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
    };
    mtlFile.close();
  }
  objFile.clear();
  objFile.seekg(0, std::ios::beg);

  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> textureCoords;
  Colour currentColour = Colour("Default", 25, 25, 255);
  if (colours.size() > 0) currentColour = colours[0];
  std::string currentTexture = "";
  Object3d currentObject;

  for (int i = 1; i < MAX_LINES; i++) {
    std::getline(objFile, line);
    if (line.substr(0, 2) == "v ") {
      std::string x = line.substr(2, line.find(' ', 2) - 2);
      std::string y = line.substr(line.find(' ', 2) + 1, line.find(' ', line.find(' ', 2) + 1) - line.find(' ', 2) - 1);
      std::string z = line.substr(line.find(' ', line.find(' ', 2) + 1) + 1, line.length() - line.find(' ', line.find(' ', 2) + 1) - 1);
      vertices.push_back(glm::vec3(std::stof(x), std::stof(y), std::stof(z)) * scalingFactor);
    } else if (line.substr(0, 3) == "vt ") {
      std::string x = line.substr(3, line.find(' ', 3) - 3);
      std::string y = line.substr(line.find(' ', 3) + 1, line.length() - line.find(' ', 3) - 1);
      textureCoords.push_back(glm::vec2(std::stof(x), std::stof(y)));
    } else if (line.substr(0, 2) == "f ") {
      std::istringstream iss(line.substr(2));
      std::string v1, v2, v3;
      std::string t1, t2, t3;

      std::getline(iss, v1, ' ');
      std::getline(iss, v2, ' ');
      std::getline(iss, v3, ' ');

      if (v1.find('/') != std::string::npos) {
        t1 = v1.substr(v1.find('/') + 1);
        v1 = v1.substr(0, v1.find('/'));
      }
      if (v2.find('/') != std::string::npos) {
        t2 = v2.substr(v2.find('/') + 1);
        v2 = v2.substr(0, v2.find('/'));
      }
      if (v3.find('/') != std::string::npos) {
        t3 = v3.substr(v3.find('/') + 1);
        v3 = v3.substr(0, v3.find('/'));
      }

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
    } else if (line.substr(0, 7) == "usemtl ") {
      std::string colour = line.substr(7, line.length() - 7);
      for (long unsigned int j = 0; j < colours.size(); j++) {
        if (colours[j].name == colour) currentColour = colours[j];
      }

      // Check if texture
      if (textureMap.find(colour) != textureMap.end())
        currentTexture = textureMap[colour];
      else
        currentTexture = "";
    } else if (line.substr(0, 2) == "o ") {
      if (currentObject.triangles.size() > 0) {
        this->addObject(currentObject);
      }
      std::string name = line.substr(2, line.length() - 2);
      currentObject = Object3d(name, window);
    } else if (line.substr(0, 2) == "s ") {
      std::string shading = line.substr(2, line.length() - 2);
      if (shading == "off")
        currentObject.shading = FLAT;
      else if (shading == "1")
        currentObject.shading = PHONG;
      else if (shading == "2")
        currentObject.shading = GOURAUD;
      else
        currentObject.shading = FLAT;
    }
    if (objFile.eof()) {
      break;
    }
  }
  std::cout << "Loaded " << currentObject.name << std::endl;
  this->addObject(currentObject);

  objFile.close();

  // Calculating vertex normals
  for (Object3d &object : this->objects) {
    object.updateTriangles();
  }
}

void Scene::addLight(Light light) {
  lights.push_back(light);
}

void Scene::addObject(Object3d object) {
  
  if (object.name == "sphere") {
    // object.transparency = 1.0f;
    // object.refractiveIndex = 2.4f;
    object.shading = GOURAUD;
  }

  if (object.name == "floor") {
    object.reflectiveness = 0.4f;
  }

  if (object.name == "bunny") {
    object.transparency = 0.0f;
    object.reflectiveness = 1.0f;
  }

  objects.push_back(object);
}

void Scene::clearScene() {
  objects.clear();
  lights.clear();
}

void Scene::addEnvironmentMap(std::string filename) {
  this->environmentMap = TextureMap(filename);
}