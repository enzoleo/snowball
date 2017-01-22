/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (wuyaohongdio@gmail.com). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#ifndef _MODEL_H_
#define _MODEL_H_

#include <string>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "objects.h"
#include "shader.hpp"
#include "texture.h"

/* STRUCT: The struct of mesh vertex
** Each member is important in Mesh class */
struct MeshVertex
{
    /* The position coordinates of vertices */
    glm::vec3 position;
    /* The normals of vertices */
    glm::vec3 normal;
    /* The texture coordinates of vertices */
    glm::vec2 texCoords;
    /* The tangent vector of vertices */
    glm::vec3 tangent;
    /* The bitangent vector of vertices */
    glm::vec3 bitangent;
};

/* CLASS: Mesh */
class Mesh 
{
public:
    /*  Mesh Data  */
    std::vector<MeshVertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    GLuint VAO;

    /* Default constructor & Constructor */
    Mesh(std::vector<MeshVertex> _vertices,
         std::vector<GLuint> _indices,
         std::vector<Texture> _textures)
        : vertices(_vertices),
          indices(_indices),
          textures(_textures)
    {
        // Set up the mesh
        setup();
    }

    // Render the mesh
    void draw(Shader shader)
    {
        shader.install();
        // Bind appropriate textures
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;
        GLuint normalNr = 1;
        GLuint heightNr = 1;
        
        for (GLuint i = 0; i < this->textures.size(); i++)
        {   
            //GLuint temp;
            std::string number;
            std::string name = this->textures[i].getType();
            if (name == "diffuse")
                number = std::to_string(diffuseNr++); // Transfer GLuint to string
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);  
            else if (name == "texture_normal")
                number = std::to_string(normalNr++);
            else if (name == "texture_height")
                number = std::to_string(heightNr++);
            
            // Set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.getProgram(), ("material." + name + number).c_str()), i + 1);
            // Active proper texture unit before binding
            glActiveTexture(GL_TEXTURE0 + i + 1); 
            // And finally bind the texture
            glBindTexture(GL_TEXTURE_2D, this->textures[i].getID()); 
        }

        // draw mesh
        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        shader.uninstall();

        // set everything back to defaults once configured.
        for (GLuint i = 0; i < this->textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i + 1);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

private:
    /*  Render data  */
    GLuint VBO, EBO;

    /* PRIVATE MEMBER: Do mesh SET UP.
    ** Initializes all the buffer objects/arrays */
    void setup()
    {
        // Create VAO (arrays), VBO and EBO (buffers)
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // The memory layout of structs is sequential for all its items.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)offsetof(MeshVertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)offsetof(MeshVertex, texCoords));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)offsetof(MeshVertex, tangent));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)offsetof(MeshVertex, bitangent));
        glBindVertexArray(0);
    }
};

class Model : public Object
{
public:
    /*  Model Data */
    std::vector<Texture> textures_loaded;   // Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes;
    std::string directory;

    Model() 
    {
        Object::texture_ptr = NULL;  // No need to use this variable
        Object::kd = 1.0;  // Set default kd, ks, shininess
        Object::ks = 0.0;
        Object::shininess = 10;
    }

    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    Model(const std::string & path)
    {
        this->loadModel(path);
    }

    Model(const std::string & model_path, const std::string & texture_path, 
        const std::string& type = "diffuse") 
    {
        this->loadModel(model_path);
         
        if (textures_loaded.size() == 0) {
            this->reloadTextureFromFile(texture_path, type);
        }
    }

    // draws the model, and thus all its meshes
    void draw(Shader shader)
    {
        shader.install();
        if (shader.getFuncType() == NORMAL) {
            shader.setUniform1f("material.kd", kd);
            shader.setUniform1f("material.ks", ks);
            shader.setUniform1f("material.shininess", shininess);
        }
        shader.setUniformMatrix4fv("model", model2world);
        for (GLuint i = 0; i < this->meshes.size(); i++)
            this->meshes[i].draw(shader);
        shader.uninstall();
    }

    //reload texture from another file if there is no texture loaded before
    //type must be one of the follows:
    //"diffuse", "specular", "normal", "height" 
    void reloadTextureFromFile(const std::string& path, const std::string& type = "diffuse")
    {
        Texture tex;
        tex.setID(TextureFromFile(path.c_str()));
        tex.setType(type);
        tex.setPath(aiString(path.c_str()));
        textures_loaded.push_back(tex);  //needed?
        for (int i = 0; i < meshes.size(); ++i) {
            meshes[i].textures.push_back(tex);
        }
    }

    void load_from_file(const std::string& path) 
    { 
        this->loadModel(path); 
    }

    void load_from_files(const std::string & model_path, const std::string & texture_path, const std::string& type = "diffuse")
    {
        this->loadModel(model_path);
        if (textures_loaded.size() == 0) {
            this->reloadTextureFromFile(texture_path, type);
        }
    }

private:
    /*  Functions  */
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string path)
    {
        // Read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);
        // Check for errors
        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR: " << importer.GetErrorString() << std::endl;
            return;
        }
        // Retrieve the directory path of the filepath
        this->directory = path.substr(0, path.find_last_of('/'));

        // Process ASSIMP's root node recursively
        this->processNode(scene->mRootNode, scene);
    }

    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // Process each mesh located at the current node
        for (GLuint i = 0; i < node->mNumMeshes; i++)
        {
            // The node object only contains indices to index the actual objects in the scene. 
            // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            this->meshes.push_back(this->processMesh(mesh, scene));
        }
        // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (GLuint i = 0; i < node->mNumChildren; i++)
        {
            this->processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // Data to fill
        std::vector<MeshVertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

        // Walk through each of the mesh's vertices
        for (GLuint i = 0; i < mesh->mNumVertices; i++)
        {
            MeshVertex vertex;
            glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
                              // Positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;
            // Normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
            // Texture Coordinates
            if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoords = vec;
            }
            else
                vertex.texCoords = glm::vec2(0.0f, 0.0f);
            // Tangent
            if (mesh->mTangents) {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.tangent = vector;
            }
            // Bitangent
            if (mesh->mBitangents) {
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.bitangent = vector;
            }
            vertices.push_back(vertex);
        }
        // Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // Retrieve all indices of the face and store them in the indices vector
            for (GLuint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // Process materials
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
            // Same applies to other texture as the following list summarizes:
            // Diffuse: diffuseN
            // Specular: texture_specularN
            // Normal: texture_normalN

            // 1. Diffuse maps
            std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. Specular maps
            std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. Normal maps
            std::vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. Height maps
            std::vector<Texture> heightMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        // Return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    // The required info is returned as a Texture struct.
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
        std::vector<Texture> textures;
        for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            GLboolean skip = false;
            for (GLuint j = 0; j < textures_loaded.size(); j++)
            {
                if (textures_loaded[j].getPath() == str)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // If texture hasn't been loaded already, load it
                Texture texture;
                texture.setID(TextureFromFileDir(str.C_Str(), this->directory));
                texture.setType(typeName);
                texture.setPath(str);
                textures.push_back(texture);
                this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }
};

#endif
