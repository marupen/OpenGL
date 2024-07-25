#pragma once

#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "mesh.h"
#include "assimp/Importer.hpp"
#include <vector>
#include <string>

class Shader;

class Model
{
public:
    Model(std::string const &path)
    {
        loadModel(path);
    }
    void Draw(Shader& shader);
private:

    // Данные модели
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(char const* path, const std::string& directory);
};