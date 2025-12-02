#ifndef LOADER_H
#define LOADER_H

#include "Loader.h"
#include <unordered_map>
#include <filesystem>

struct PLYProperty {
    std::string name;
    std::string type;
    size_t size;
};

static size_t getTypeSize(const std::string& type) {
    static const std::unordered_map<std::string, size_t> typeSizes = {
        {"char", 1}, {"uchar", 1},
        {"int8", 1}, {"uint8", 1},
        {"short", 2}, {"ushort", 2},
        {"int16", 2}, {"uint16", 2},
        {"int", 4}, {"uint", 4},
        {"int32", 4}, {"uint32", 4},
        {"float", 4}, {"float32", 4},
        {"double", 8}, {"float64", 8}
    };
    auto it = typeSizes.find(type);
    return (it != typeSizes.end()) ? it->second : 0;
}

bool Loader::loadFromtxtFile(const std::string& filePath, std::vector<Point>& vertices, glm::vec3& minVertex, glm::vec3& maxVertex)
{
	std::ifstream file(filePath, std::ios::in);
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open txt file: " << filePath << std::endl;
		return false;
	}
	bool firstVertex = true;
	glm::vec3 vertex;
	std::string line;
	int cont = 0;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		Point point;

		float intensity; // Variable temporal para almacenar el valor de intensidad
		//No quiero que lea la intensity
		iss >> point.position.x >> point.position.y >> point.position.z
			>> intensity >> point.color.r >> point.color.g >> point.color.b;

		point.color.r /= 255.0f;
		point.color.g /= 255.0f;
		point.color.b /= 255.0f;

		vertex = point.position;
		if (firstVertex)
		{
			minVertex = maxVertex = vertex;
			firstVertex = false;
		}
		else
		{
			minVertex = glm::min(minVertex, vertex);
			maxVertex = glm::max(maxVertex, vertex);
		}

		// Aqu� se pueden a�adir otros datos adicionales necesarios para cada punto
		vertices.push_back(point);
		cont += 1;

		if (cont % 1000000 == 0)
		{
			std::cerr << cont << "Points" << std::endl;
		}
		if (cont % 10500000 == 0)
		{
			break;
		}
	}

	// El vector 'vertices' ahora contiene todos los puntos le�dos del archivo
	return true;
}


glm::vec3 Loader::getColor(int index) {
	glm::vec3 colors[8] = {
		//glm::vec3(1.0, 0.0, 0.0), // Rojo
		glm::vec3(0.0, 1.0, 0.0), // Verde
		glm::vec3(0.0, 0.0, 1.0), // Azul
		glm::vec3(1.0, 1.0, 0.0), // Amarillo
		glm::vec3(1.0, 0.0, 1.0), // Magenta
		glm::vec3(0.0, 1.0, 1.0), // Cyan
		glm::vec3(0.5, 0.5, 0.5), // Gris
		glm::vec3(0.0, 0.0, 0.0) // Negro
	};
	return colors[index];
}

bool Loader::loadFromOBJFile(const std::string& filePath, std::vector<Point>& vertices, glm::vec3& minVertex, glm::vec3& maxVertex)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open OBJ file: " << filePath << std::endl;
		return false;
	}

	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec3> temp_normals;
	std::vector<glm::vec3> temp_color;
	bool firstVertex = true;
	int cont = 0;
	glm::vec3 vertex;
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string type;
		iss >> type;

		if (type == "v")
		{

			iss >> vertex.x >> vertex.y >> vertex.z;
			temp_vertices.push_back(vertex);

			if (firstVertex)
			{
				minVertex = maxVertex = vertex;
				firstVertex = false;
			}
			else
			{
				minVertex = glm::min(minVertex, vertex);
				maxVertex = glm::max(maxVertex, vertex);
			}
			cont += 1;

			if (cont % 1000000 == 0)
			{
				std::cerr << cont << "Points" << std::endl;
			}
		}
		else if (type == "vn")
		{
			iss >> vertex.x >> vertex.y >> vertex.z;
			temp_normals.push_back(vertex);
		}
	}

	file.close();
	int con2t = 0;
	for (int i = 0; i < temp_vertices.size(); i++)
	{	
		Point newP;
		newP.position = temp_vertices[i];
		if (temp_normals.size() == temp_vertices.size())
		{
			newP.normal = temp_normals[i];
		}
		//newP.color = getColor(con2t);
		//con2t++;
		//newP.color = temp_color[i];
		vertices.push_back(newP);
	}
	//vertices = temp_vertices;
	// Procesar los datos de temp_vertices seg�n las necesidades de tu aplicaci�n
	// y almacenarlos en las estructuras de datos de la clase PointCloud.

	return true;
}

bool Loader::loadFromBigEndianPLYFile(const std::string& filePath, std::vector<Point>& vertices, glm::vec3& minVertex, glm::vec3& maxVertex)
{
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open PLY file: " << filePath << std::endl;
		return false;
	}

	int numVertices = -1;

	std::unordered_map<double, int> lCounts; // Diccionario para contar los valores de "l"
	std::unordered_map<double, int> pCounts; // Diccionario para contar los valores de "p"

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string type;
		iss >> type;

		if (type == "element")
		{
			std::string elementType;
			int count;
			iss >> elementType >> count;
			if (elementType == "vertex")
			{
				numVertices = count;
			}
		}
		else if (type == "end_header")
		{
			if (numVertices == -1)
			{
				std::cerr << "Error: Invalid PLY file format." << std::endl;
				return false;
			}

			vertices.resize(numVertices);
			file.seekg(0, std::ios_base::cur);
			for (int i = 0; i < numVertices; i++)
			{
				file.seekg(0, std::ios_base::cur);

				float x, y, z;
				bool error;
				file.read(reinterpret_cast<char*>(&x), sizeof(float));
				file.read(reinterpret_cast<char*>(&y), sizeof(float));
				file.read(reinterpret_cast<char*>(&z), sizeof(float));

				if (!isBigEndian())
				{
					swapBytes(reinterpret_cast<char*>(&x), sizeof(float));
					swapBytes(reinterpret_cast<char*>(&y), sizeof(float));
					swapBytes(reinterpret_cast<char*>(&z), sizeof(float));
				}

				glm::vec3 vertex(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

				if (i == 0)
				{
					minVertex = maxVertex = vertex;
				}
				else
				{
					minVertex = glm::min(minVertex, vertex);
					maxVertex = glm::max(maxVertex, vertex);
				}

				Point point;
				point.position = vertex;
				/*point.labels = glm::vec3(l, p, l==p);
				if (p != l)
				{
					//point.color = getColor(0);
				}
				else
				{
					//point.color = getColor(1);
				}*/
				point.color = glm::vec3(1.f, 0.f, 0.f);
				//point.normal = getColor(p);//glm::vec3(1.f, 0.f, 0.f);
				vertices[i] = point;

				//file.seekg(sizeof(double), std::ios_base::cur);

				// Contar los valores distintos de "l"
				/*if (lCounts.find(l) != lCounts.end())
				{
					lCounts[l]++;
				}
				else
				{
					lCounts[l] = 1;
				}

				// Contar los valores distintos de "p"
				if (pCounts.find(p) != pCounts.end())
				{
					pCounts[p]++;
				}
				else
				{
					pCounts[p] = 1;
				}*/
			}
		}
	}

	file.close();
	return true;
}

bool Loader::loadFromLittleEndianPLYFile(
    const std::string& filePath,
    std::vector<Point>& vertices,
    glm::vec3& minVertex,
    glm::vec3& maxVertex)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open PLY file: " << filePath << std::endl;
        return false;
    }

    std::string line;
    bool littleEndian = true;
    int numVertices = 0;
    int numFaces = 0;
    bool inVertexSection = false;
    bool inFaceSection = false;
    std::vector<PLYProperty> vertexProperties;

    // --- Parse header ---
    while (std::getline(file, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "format") {
            std::string fmt;
            iss >> fmt;
            if (fmt == "binary_big_endian") littleEndian = false;
        } 
        else if (token == "element") {
            std::string elementType;
            int count;
            iss >> elementType >> count;
            if (elementType == "vertex") {
                numVertices = count;
                inVertexSection = true;
                inFaceSection = false;
            } else if (elementType == "face") {
                numFaces = count;
                inVertexSection = false;
                inFaceSection = true;
            } else {
                inVertexSection = inFaceSection = false;
            }
        } 
        else if (token == "property" && inVertexSection) {
            std::string type, name;
            iss >> type >> name;
            vertexProperties.push_back({name, type, getTypeSize(type)});
        } 
        else if (token == "end_header") {
            break;
        }
    }

    if (numVertices == 0) {
        std::cerr << "Error: No vertex element found in PLY file." << std::endl;
        return false;
    }

    vertices.resize(numVertices);

    // --- Compute vertex stride ---
    size_t vertexStride = 0;
    for (auto& prop : vertexProperties) vertexStride += prop.size;

    // --- Read vertex data ---
    std::vector<char> buffer(vertexStride);
    for (int i = 0; i < numVertices; ++i) {
        file.read(buffer.data(), vertexStride);

        float x = 0, y = 0, z = 0;
        unsigned char r = 255, g = 255, b = 255;
        size_t offset = 0;

        for (auto& prop : vertexProperties) {
            const char* ptr = buffer.data() + offset;

            if (prop.name == "x")
                x = *reinterpret_cast<const float*>(ptr);
            else if (prop.name == "y")
                y = *reinterpret_cast<const float*>(ptr);
            else if (prop.name == "z")
                z = *reinterpret_cast<const float*>(ptr);
            else if (prop.name == "red")
                r = *reinterpret_cast<const unsigned char*>(ptr);
            else if (prop.name == "green")
                g = *reinterpret_cast<const unsigned char*>(ptr);
            else if (prop.name == "blue")
                b = *reinterpret_cast<const unsigned char*>(ptr);

            offset += prop.size;
        }

        glm::vec3 position(x, y, z);
        glm::vec3 color(r / 255.0f, g / 255.0f, b / 255.0f);

        if (i == 0)
            minVertex = maxVertex = position;
        else {
            minVertex = glm::min(minVertex, position);
            maxVertex = glm::max(maxVertex, position);
        }

        Point p;
        p.position = position;
        p.color = color;
        vertices[i] = p;
    }

    file.close();
    return true;
}



bool Loader::isBigEndian()
{
	const uint32_t one = 1;
	return *reinterpret_cast<const char*>(&one) == 0;
}

void Loader::swapBytes(char* data, int size)
{
	for (int i = 0; i < size / 2; ++i)
	{
		std::swap(data[i], data[size - i - 1]);
	}
}

bool Loader::loadFromPLYFile(const std::string& filePath, std::vector<Point>& vertices, glm::vec3& minVertex, glm::vec3& maxVertex)
{
	std::ifstream file(filePath);
	
	//std::cout << std::filesystem::current_path() << filePath << "\n";
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open PLY file: " << filePath << std::endl;
		return false;
	}

	std::string line;
	std::getline(file, line);
	
	if (line != "ply")
	{
		std::cerr << "Error: Not a valid PLY file: " << filePath << std::endl;
		return false;
	}

	std::getline(file, line);
	file.close();

	if (line == "format ascii 1.0")
	{
		return loadFromASCIIPLYFile(filePath, vertices, minVertex, maxVertex);
	}
	else if (line == "format binary_big_endian 1.0")
	{
		return loadFromBigEndianPLYFile(filePath, vertices, minVertex, maxVertex);
	}
	else if (line == "format binary_little_endian 1.0")
	{
		return loadFromLittleEndianPLYFile(filePath, vertices, minVertex, maxVertex);
	}
	else
	{
		std::cerr << "Error: Unsupported PLY format: " << line << std::endl;
		return false;
	}
}


bool Loader::loadFromASCIIPLYFile(
    const std::string& filePath,
    std::vector<Point>& vertices,
    glm::vec3& minVertex,
    glm::vec3& maxVertex)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open ASCII PLY file: " << filePath << std::endl;
        return false;
    }

    std::string line;
    int numVertices = 0;
    bool inVertexSection = false;
    std::vector<PLYProperty> vertexProperties;

    // --- Parse header ---
    while (std::getline(file, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "element") {
            std::string type;
            int count;
            iss >> type >> count;

            if (type == "vertex") {
                numVertices = count;
                inVertexSection = true;
            } else {
                inVertexSection = false;
            }
        }
        else if (token == "property" && inVertexSection) {
            std::string type, name;
            iss >> type >> name;

            vertexProperties.push_back({name, type, getTypeSize(type)});
        }
        else if (token == "end_header") {
            break;
        }
    }

    if (numVertices == 0) {
        std::cerr << "Error: No vertices found in ASCII PLY." << std::endl;
        return false;
    }

    vertices.resize(numVertices);

    // --- Read vertex lines ---
    for (int i = 0; i < numVertices; i++) {
        if (!std::getline(file, line)) {
            std::cerr << "Error: Unexpected end of vertex data." << std::endl;
            return false;
        }

        std::istringstream iss(line);

        float x = 0, y = 0, z = 0;
        int ri = 255, gi = 255, bi = 255;  // use ints here
        size_t propIndex = 0;

        for (auto& prop : vertexProperties) {
            if (prop.name == "x") iss >> x;
            else if (prop.name == "y") iss >> y;
            else if (prop.name == "z") iss >> z;
            else if (prop.name == "red") iss >> ri;
            else if (prop.name == "green") iss >> gi;
            else if (prop.name == "blue") iss >> bi;
            else {
                // skip unsupported properties
                float dummy;
                iss >> dummy;
            }
            propIndex++;
        }

        glm::vec3 position(x, y, z);
        glm::vec3 color(ri / 255.0f, gi / 255.0f, bi / 255.0f);

        if (i == 0)
            minVertex = maxVertex = position;
        else {
            minVertex = glm::min(minVertex, position);
            maxVertex = glm::max(maxVertex, position);
        }

        Point p;
        p.position = position;
        p.color = color;
        vertices[i] = p;
    }

    return true;
}

#endif LOADER_H