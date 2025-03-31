#include "StaticMeshManager.h"
#include <filesystem>
#include <string>
#include <mutex>
#include <execution>
#include "RenderDebugger.h"

bool StaticMeshManager::Insert(const std::string& path, StaticMesh& sMesh)
{
    std::lock_guard<std::mutex> guard(smmMutex);
    staticMeshMap[path] = sMesh;

    return true;
}
bool StaticMeshManager::InsertMeshDataMap(const std::string& path, MeshData& meshData)
{
    std::lock_guard<std::mutex> guard(smmMutex);
    meshDataMap[path] = meshData;

    return true;
}
bool StaticMeshManager::InsertHullMap(const std::string& path, ConvexHull& hull)
{
    std::lock_guard<std::mutex> guard(smmMutex);
    convexHullMap[path] = hull;
    return true;
}
bool StaticMeshManager::HullExists(const std::string& path)
{
    return convexHullMap.find(path) != convexHullMap.end();
}
bool StaticMeshManager::Exists(const std::string& path)
{
    return staticMeshMap.find(path) != staticMeshMap.end();
}
StaticMesh StaticMeshManager::Get(const std::string& path)
{
    return staticMeshMap[path];
}
const MeshData* StaticMeshManager::GetMeshData(const std::string& path)
{
    return &meshDataMap[path];
}
ConvexHull &StaticMeshManager::GetConvexHull(const std::string& path)
{
    return convexHullMap[path];
}
bool StaticMeshManager::UploadStaticMesh(const std::string& name, std::vector<StaticMeshVertex>& vertices, std::vector<unsigned>& indices, std::vector<Texture>& textures, Mesh& outMesh)
{
    initializeOpenGLFunctions();

    unsigned VAO, VBO, EBO;

    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(StaticMeshVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StaticMeshVertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(StaticMeshVertex), (void*)offsetof(StaticMeshVertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(StaticMeshVertex), (void*)offsetof(StaticMeshVertex, texCord));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(StaticMeshVertex), (void*)offsetof(StaticMeshVertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(StaticMeshVertex), (void*)offsetof(StaticMeshVertex, bitTangent));

    glBindVertexArray(0);

    outMesh.vao = VAO;
    outMesh.indices = indices.size();

    for (auto& texture : textures) outMesh.AddTexture(texture);
        
    return true;
}

bool StaticMeshManager::LoadMesh(aiMesh* mesh, const aiScene* scene, const std::string& path, StaticMesh& outStaticMesh, MeshData& outMeshData)
{
    std::vector<StaticMeshVertex> vertices;
    std::vector<unsigned> indices;
    std::vector<Texture> textures;

    glm::vec3 meshCenter{ 0.f, 0.f, 0.f };

    for (auto i = 0; i < mesh->mNumVertices; i++)
    {
        StaticMeshVertex vertex;
       
        {	//Position and normals
            for (auto j = 0; j < 3; j++)
            {
                vertex.pos[j] = mesh->mVertices[i][j];   
                meshCenter[j] += vertex.pos[j];
            }
        	
            if (mesh->HasNormals())
	            for (auto j = 0; j < 3; j++)
	                vertex.normal[j] = mesh->mNormals[i][j];            
        }
        {	//Texture coords
            if (mesh->mTextureCoords[0])
            {
                for (auto j = 0; j < 2; j++)
                {
                    vertex.texCord[j] = mesh->mTextureCoords[0][i][j];
                }
            }
        }
        {	//Tangents
        	if (mesh->HasTangentsAndBitangents())
	            for (auto j = 0; j < 3; j++)
	            {
	                vertex.tangent[j] = mesh->mTangents[i][j];
	                vertex.bitTangent[j] = mesh->mBitangents[i][j];
	            }
        }
        vertices.emplace_back(vertex);
    }
    {	//Indices
        for (auto i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (auto j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
    }
    {	//Bonedata
        if (mesh->HasBones())
        {
            std::cerr << "Static mesh has bones, use skeletal mesh manager if you need bones! current path: " << path << std::endl;
        }
    }
    bool bIsTransparent{ false };
    {	//loading materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::cout << "Loading material for path: " << path << std::endl;
        std::cout << "Material types:" << std::endl;
        for (auto i = 1; i < (int)aiTextureType::aiTextureType_UNKNOWN; i++)
        {
            aiTextureType type = aiTextureType(i);
            for (auto j = 0; j < material->GetTextureCount(type); j++)
            {

                std::cout << "Texture types " << Texture::GetTypeString((Texture::Types)type) << std::endl;

                aiString texturePath;
                material->GetTexture(type, j, &texturePath);

                std::filesystem::path p(texturePath.C_Str());
                std::cout << "texture path " << texturePath.C_Str() << std::endl;
                const std::string fullPath = (outStaticMesh.pathDirectory + '/' + p.filename().string());
                std::cout << "full path " << fullPath << std::endl;
                Texture texture;

                if (type == aiTextureType_OPACITY)
                {
                    bIsTransparent = true;
                }

                if (textureManager.LoadTexture(fullPath, texture, (Texture::Types)i))
                {
                    textures.push_back(texture);

                }
                else
                {
                    printf("Failed to load texture: %s \n", fullPath.c_str());
                }
            }
        }
    }

    { //Uploading mesh
        Mesh inMesh; std::string meshName = mesh->mName.C_Str(); 

        if (!UploadStaticMesh(meshName, vertices, indices, textures, inMesh))
        {
            printf("Failed to upload mesh: %s", meshName.c_str());
            return false;
        }
    	
        inMesh.bIsTransparent = bIsTransparent;

        meshCenter /= (float)mesh->mNumVertices;
        inMesh.transform.SetPosition(meshCenter);

        outStaticMesh.AddMesh(inMesh);
    }

    for (size_t i = 0; i < vertices.size(); i++)
    {
        outMeshData.verts.emplace_back(vertices[i].pos);
    }

    std::cout << "This mesh vert size " << vertices.size() << std::endl;
    std::cout << "outMeshData verts size " << outMeshData.verts.size() << std::endl;

    auto* rd = RenderDebugger::Get();

    /*
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        int a = indices[i];
        int b = indices[i + 1];
        int c = indices[i + 2];

        rd->AddDebugSegment(100.f, vertices[a].pos, vertices[b].pos, glm::vec3(0.f, 0.f, 1.f));
        rd->AddDebugSegment(100.f, vertices[b].pos, vertices[c].pos, glm::vec3(0.f, 0.f, 1.f));
        rd->AddDebugSegment(100.f, vertices[c].pos, vertices[a].pos, glm::vec3(0.f, 0.f, 1.f));
    }
    */
    /*
    for (size_t i = 0; i < vertices.size(); i++)
    {
        rd->AddDebugPoint(100.f, vertices[i].pos, glm::vec3(0.f, 1.f, 0.f), 50.f, false);
    }
    */

    unsigned lastIndex = outMeshData.indices.size();

    for (size_t i = 0; i < indices.size(); i++)
    {
        outMeshData.indices.emplace_back(indices[i] + lastIndex);
    }

    return true;
}

bool StaticMeshManager::LoadMeshAsync(aiMesh* mesh, const aiScene* scene, const std::string& path, StaticMesh& outStaticMesh, int threadIndex)
{
    AsyncMeshInfo mi;

    mi.path = path;
    std::vector<StaticMeshVertex>& vertices = mi.vertices;
    std::vector<unsigned>& indices = mi.indices;

    glm::vec3 meshCenter{ 0.f, 0.f, 0.f };

    for (auto i = 0; i < mesh->mNumVertices; i++)
    {
        StaticMeshVertex vertex;

        {	//Position and normals
            for (auto j = 0; j < 3; j++)
            {
                vertex.pos[j] = mesh->mVertices[i][j];
                meshCenter[j] += vertex.pos[j];
            }

            if (mesh->HasNormals())
                for (auto j = 0; j < 3; j++)
                    vertex.normal[j] = mesh->mNormals[i][j];
        }
        {	//Texture coords
            if (mesh->mTextureCoords[0])
            {
                for (auto j = 0; j < 2; j++)
                {
                    vertex.texCord[j] = mesh->mTextureCoords[0][i][j];
                }
            }
        }
        {	//Tangents
            if (mesh->HasTangentsAndBitangents())
                for (auto j = 0; j < 3; j++)
                {
                    vertex.tangent[j] = mesh->mTangents[i][j];
                    vertex.bitTangent[j] = mesh->mBitangents[i][j];
                }
        }
        vertices.emplace_back(vertex);
    }
    {	//Indices
        for (auto i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (auto j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
    }
    {	//Bonedata
        if (mesh->HasBones())
        {
            std::cerr << "Static mesh has bones, use skeletal mesh manager if you need bones! current path: " << path << std::endl;
        }
    }
    bool bIsTransparent{ false };
    {	//loading materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        for (auto i = 1; i < (int)aiTextureType::aiTextureType_UNKNOWN; i++)
        {
            aiTextureType type = aiTextureType(i);
            for (auto j = 0; j < material->GetTextureCount(type); j++)
            {

                std::cout << "Texture types " << Texture::GetTypeString((Texture::Types)type) << std::endl;
                aiString texturePath;

                material->GetTexture(type, j, &texturePath);
                std::filesystem::path p(texturePath.C_Str());
                const std::string fullPath = (outStaticMesh.pathDirectory + '/' + p.filename().string());
                mi.texturePaths.push_back({ fullPath, (Texture::Types)i });
            }
        }
    }

    asyncMeshInfo[threadIndex].emplace_back(mi);

    return true;
}

StaticMeshManager::StaticMeshManager()
{
    tp = new ThreadPoolx([this](int i) {this->WorkerThread(i); }, SMM_CONTEXTS);
}

StaticMeshManager::~StaticMeshManager()
{
    for (auto [name, staticMeshInstanced] : staticMeshMap)
    {
        for (auto& mesh : staticMeshInstanced.meshes)
        {
            glDeleteVertexArrays(1, &mesh.vao);
        }
    }
    delete tp;
}

bool StaticMeshManager::LoadStaticMesh(const std::string& path, StaticMesh& outStaticMesh, bool bCreateConvexHull, bool bSaveMeshData)
{
    if (Exists(path))
    {
        outStaticMesh = Get(path);
        return true;
    }

    const aiScene* scene;

    Assimp::Importer importer;
    Assimp::Exporter Exporter;

    uint binPathExtIndex = path.find_last_of('.');
    std::string pathWOExt = std::string(path.begin(), path.begin() + binPathExtIndex);
    std::string binPathExt = ".ass";
    std::string binPath = pathWOExt + binPathExt;

    if (std::filesystem::exists(binPath))
    {
        std::cout << "Loading asset binary file from path: " << binPath << std::endl;
        scene = importer.ReadFile(binPath,
            aiFlags);
    }
    else
    {
        std::cout << "Loading asset obj file from path: " << path << std::endl;
        scene = importer.ReadFile(path,
            aiFlags);

        std::cout << "Exporting asset to binary: " << binPath << std::endl;
        const aiExportFormatDesc* format = Exporter.GetExportFormatDescription(13);
        Exporter.Export(scene, format->id, binPath, aiFlags);
    }

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "Assimp failed to load " << importer.GetErrorString() << std::endl;
        return false;
    }
    std::string directory = path.substr(0, path.find_last_of('/'));

    outStaticMesh.globalInverseTransform = scene->mRootNode->mTransformation;
    outStaticMesh.globalInverseTransform.Inverse();
    outStaticMesh.pathDirectory = path.substr(0, path.find_last_of('/'));
    
    MeshData meshData;

    std::function<void(aiNode*, const aiScene*)> ProcessNode =
        [&ProcessNode, this, &path, &outStaticMesh, &meshData](aiNode* node, const aiScene* scene)
    {
        for (auto i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            if (!LoadMesh(mesh, scene, path, outStaticMesh, meshData))
                std::cout << "failed to load mesh: " << path << std::endl;
        }       
        for (auto i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }       
    };

    ProcessNode(scene->mRootNode, scene);

    Insert(path, outStaticMesh);

    if (bSaveMeshData) InsertMeshDataMap(path, meshData);
   
    if (bCreateConvexHull)
    { 
        ConvexHull hull;
        hull.BuildKDOP(meshData.verts.data(), meshData.verts.size(), KDOP::DOP26, KDOP_AXIS::Y);
        InsertHullMap(path, hull);       
    }

    return true;
}

bool StaticMeshManager::LoadStaticMeshesAsync(std::vector<std::string>& paths)
{
    std::cout << "Loading assets in async..." << std::endl;

    tpPaths = paths;
    tp->StartWork();  
    tp->WaitWork();

    std::cout << "Async assets data cached..." << std::endl;

    UploadStaticMeshAsync();
    ClearAsyncMeshInfo();

    std::cout << "Async assets finished!" << std::endl;
    return true;
}

void StaticMeshManager::WorkerThread(int index)
{
    while (tpIndex < tpPaths.size())
    {
        auto& path = tpPaths[tpIndex++];

        std::cout << "Loading asset async thread index: "<< index << " at path: " << path << std::endl;

        StaticMesh temp;
        LoadStaticMeshAsync(path, temp, index);
    }  
}

void StaticMeshManager::ClearAsyncMeshInfo()
{
    for (size_t i = 0; i < SMM_CONTEXTS; i++)
    {
        asyncMeshInfo[i].clear();
    }
    tpPaths.clear();
    tpIndex = 0;
}

bool StaticMeshManager::LoadStaticMeshAsync(const std::string& path, StaticMesh& outStaticMesh, int threadIndex)
{
    if (Exists(path))
    {
        outStaticMesh = Get(path);
        return true;
    }

    const aiScene* scene;

    uint binPathExtIndex = path.find_last_of('.');
    std::string pathWOExt = std::string(path.begin(), path.begin() + binPathExtIndex);
    std::string binPathExt = ".ass";
    std::string binPath = pathWOExt + binPathExt;

    if (std::filesystem::exists(binPath))
    {
        std::cout << "Loading asset binary file from path: " << binPath << std::endl;
        scene = importer[threadIndex].ReadFile(binPath,
            aiFlags);
    }
    else
    {
        std::cout << "Loading asset obj file from path: " << path << std::endl;
        scene = importer[threadIndex].ReadFile(path,
            aiFlags);

        std::cout << "Exporting asset to binary: " << binPath << std::endl;
        const aiExportFormatDesc* format = exporter[threadIndex].GetExportFormatDescription(13);
        exporter[threadIndex].Export(scene, format->id, binPath, aiFlags);
    }

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "Assimp failed to load " << importer[threadIndex].GetErrorString() << std::endl;
        return false;
    }
    std::string directory = path.substr(0, path.find_last_of('/'));

    outStaticMesh.globalInverseTransform = scene->mRootNode->mTransformation;
    outStaticMesh.globalInverseTransform.Inverse();
    outStaticMesh.pathDirectory = path.substr(0, path.find_last_of('/'));

    std::function<void(aiNode*, const aiScene*)> ProcessNode =
        [&ProcessNode, this, &path, &outStaticMesh, &threadIndex](aiNode* node, const aiScene* scene)
    {
        // nodeTransforms.name = node->mName.C_Str(); Might add per node transform later, if needed
        // nodeTransforms.transform = node->mTransformation;

        for (auto i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            if (!LoadMeshAsync(mesh, scene, path, outStaticMesh, threadIndex))
                std::cout << "failed to load mesh: " << path << std::endl;
        }
        for (auto i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    };

    ProcessNode(scene->mRootNode, scene);
}

void StaticMeshManager::UploadStaticMeshAsync()
{
    std::vector<std::pair<std::string, StaticMesh>> sms;

    for (size_t i = 0; i < SMM_CONTEXTS; i++)
    {
        auto& aVec = asyncMeshInfo[i];

        for (auto& mi : aVec)
        {
            static std::string lastPath{""};

            if (mi.path != lastPath)
                sms.push_back({mi.path, StaticMesh() });

            std::vector<Texture> textures; bool hasTrans = false;
            for (auto& [path, type] : mi.texturePaths)
            {             
                Texture texture;
                textureManager.LoadTexture(path, texture, (Texture::Types)type);

                if ((Texture::Types)type == Texture::Opacity)
                    hasTrans = true;

                textures.emplace_back(texture);
            }

            Mesh mesh;
            UploadStaticMesh(mi.path, mi.vertices, mi.indices, textures, mesh);    
            mesh.bIsTransparent = hasTrans;

            sms.back().second.AddMesh(mesh);

            lastPath = mi.path;
        }
    }

    for (auto& [path, sm] : sms)
    {
        Insert(path, sm);
    }
}
