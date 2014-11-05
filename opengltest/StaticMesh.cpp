//StaticMesh.cpp - implementation of loading meshes, initialising vertex buffer data etc
// http://www.lighthouse3d.com/cg-topics/code-samples/importing-3d-models-with-assimp/ was a great reference in building 
//this

//last update 05/11/2014 - made non OOP


#include "StaticMesh.h"
#include "VertexTypes.h"
#include "VertexArray.h"
#include <aiScene.h>
#include <aiPostProcess.h>
#include <aiMesh.h>
#include <assimp.hpp>
#include <assimp.h>
#include <fstream>
#include <iostream>


bool InitStaticMesh(StaticMesh& mesh,char* fileName){
	
	//check if file exists
	char buf[40];
	std::ifstream fin(fileName);

	if(!fin.fail()){
		fin.close();
	}else{
		std::cout << "Couldn't open file "<< fileName <<" it doesn't exist!" << std::endl;
		return false;
	}

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(fileName,aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	if(!scene){
		std::cout <<"couldn't load scene! Reason: "<< importer.GetErrorString() <<std::endl;
		return false;
	}

	GetBoundingBox(mesh.m_boundingBoxMin,mesh.m_boundingBoxMax,scene);

	float biggestDifferenceAxis = 0.0f;
	biggestDifferenceAxis = mesh.m_boundingBoxMax.x - mesh.m_boundingBoxMin.x; 

	if(mesh.m_boundingBoxMax.y - mesh.m_boundingBoxMin.y > biggestDifferenceAxis ){
		biggestDifferenceAxis = mesh.m_boundingBoxMax.y - mesh.m_boundingBoxMin.y;
	}

	if (mesh.m_boundingBoxMax.z - mesh.m_boundingBoxMin.z > biggestDifferenceAxis){
		biggestDifferenceAxis = mesh.m_boundingBoxMax.z - mesh.m_boundingBoxMin.z;
	}

	std::cout<<"number of meshes in Init: "<< scene->mNumMeshes << std::endl;

	mesh.m_numMeshes = scene->mNumMeshes;
	mesh.m_meshData.reserve(mesh.m_numMeshes);

	//now init vaos and buffers...
		
	//find out if this mesh has positions, normals, textures
	for (unsigned int meshNum = 0; meshNum < mesh.m_numMeshes; meshNum++){
		int numAttribs = 0;

		const aiMesh* thisMesh = scene->mMeshes[meshNum];

		unsigned int numFaces = thisMesh->mNumFaces;

		//copy the mesh's face data

		unsigned int *indices = new unsigned int[numFaces * 3];


		for(unsigned int face = 0, vertexIndex = 0; face < numFaces; face++, vertexIndex += 3){
			const struct aiFace* meshFace = &thisMesh->mFaces[face];
			memcpy(&indices[vertexIndex],meshFace->mIndices,sizeof(unsigned int) * 3);
		}

		//copy the mesh's vertex data

		//create a new objectdata to store the mesh data on the gfx card
		MeshComponent* newComp = new MeshComponent;
		newComp->m_numFaces = numFaces;
			
		//here is a slightly messy way of allocating object data....
		//generate vertex array for this mesh using objectData


		if(thisMesh->HasPositions() && thisMesh->HasNormals() && thisMesh->HasTextureCoords(0)){
			//create a new objectdata to store the mesh data on the gfx card
			unsigned int numVerts = thisMesh->mNumVertices;
			CustomVertexNormUV *vertices = new CustomVertexNormUV[numVerts];
			
			int vecSize = sizeof(float) * 3;
			int texVecSize = sizeof(float) * 2;
			for (unsigned int vertex = 0; vertex < numVerts; vertex++){
				memcpy(&vertices[vertex].vertexPoint, &thisMesh->mVertices[vertex], vecSize);
				memcpy(&vertices[vertex].normal, &thisMesh->mNormals[vertex], vecSize);
				memcpy(&vertices[vertex].uv, &thisMesh->mTextureCoords[0][vertex], texVecSize);
			}

			newComp->m_vertexBuffer = CreateVertexNormUVArray(vertices, numVerts, indices, numFaces);

			delete [] vertices;

		}else if(thisMesh->HasPositions() && thisMesh->HasNormals())
		{
			//create a new objectdata to store the mesh data on the gfx card
			unsigned int numVerts = thisMesh->mNumVertices;
			CustomVertexNorm *vertices = new CustomVertexNorm[numVerts];

			int vecSize = sizeof(float) * 3;
			for (unsigned int vertex = 0; vertex < numVerts; vertex++){
				memcpy(&vertices[vertex].vertexPoint, &thisMesh->mVertices[vertex], vecSize);
				memcpy(&vertices[vertex].normal, &thisMesh->mNormals[vertex], vecSize);
			}

			newComp->m_vertexBuffer = CreateVertexNormArray(vertices, numVerts, indices, numFaces);

			delete[] vertices;
		}else {
			//create a new objectdata to store the mesh data on the gfx card
			glm::vec3 *vertices = new glm::vec3[thisMesh->mNumVertices];

			unsigned int numVerts = thisMesh->mNumVertices;
			unsigned int vecSize = sizeof(float) * 3;
			for (unsigned int vertex = 0; vertex < numVerts; vertex++){
				memcpy(&vertices[vertex], &thisMesh->mVertices[vertex], vecSize);
			}

			newComp->m_vertexBuffer = CreateSimpleVertexArray(vertices, numVerts, indices, numFaces);

			delete[] vertices;
		}

		Material* newMat = NULL;
		GLuint	  newTex = NULL;

		//load materials
		newMat = LoadMaterials(scene,scene->mMaterials[thisMesh->mMaterialIndex]);
		newTex = LoadTextures(scene,scene->mMaterials[thisMesh->mMaterialIndex]);
		
		
		newComp->m_material = newMat;

		if(newTex != 0){
			newComp->m_hasTexture = true;
			newComp->m_texture = newTex;
		}

		
		mesh.m_meshData.push_back(newComp);
	}
	return true;
}



void GetBoundingBox(glm::vec3& min,glm::vec3& max,const aiScene* scene){
	aiMatrix4x4 trafo;
    aiIdentityMatrix4(&trafo);

    min.x = min.y = min.z =  1e10f;
    max.x = max.y = max.z = -1e10f;
    GetBoundingBoxForNode(scene->mRootNode,min,max,trafo,scene);
}

void GetBoundingBoxForNode(const aiNode* node,glm::vec3& min,glm::vec3& max,aiMatrix4x4& trafo,
				                        const aiScene* scene){
	aiMatrix4x4 prev;

	prev = trafo;
	aiMultiplyMatrix4(&trafo,&node->mTransformation);

	for (unsigned int n = 0; n < node->mNumMeshes; ++n) {
        const struct aiMesh* mesh = scene->mMeshes[node->mMeshes[n]];
            
		for (unsigned int t = 0; t < mesh->mNumVertices; ++t) {

                struct aiVector3D tmp = mesh->mVertices[t];
                aiTransformVecByMatrix4(&tmp,&trafo);

                min.x = glm::min(min.x,tmp.x);
                min.y = glm::min(min.y,tmp.y);
                min.z = glm::min(min.z,tmp.z);

                max.x = glm::max(max.x,tmp.x);
                max.y = glm::max(max.y,tmp.y);
                max.z = glm::max(max.z,tmp.z);
        }
    }

    for (unsigned int n = 0; n < node->mNumChildren; n++) {
        GetBoundingBoxForNode(node->mChildren[n],min,max,trafo,scene);
    }

    trafo = prev;
}

Material* LoadMaterials(const aiScene* scene,aiMaterial* material)
{
	Material* mat = new Material;

	float c[4] = {0.8f, 0.8f, 0.8f, 1.0f};

    aiColor4D diffuse;
    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse)){
		memcpy(&mat->diffuse,&diffuse, sizeof(diffuse));
	}else{
		memcpy(&mat->diffuse,c, sizeof(c));
	}
 
	c[0] = 0.2f;
	c[1] = 0.2f; 
	c[2] = 0.2f;
	c[3] = 1.0f;

    aiColor4D ambient;
    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient)){
        memcpy(&mat->ambient, &ambient, sizeof(ambient));
	}else{
		memcpy(&mat->ambient,c, sizeof(c));
	}
 
	c[0] = 0.0f;
	c[1] = 0.0f; 
	c[2] = 0.0f;
	c[3] = 1.0f;
      
    aiColor4D specular;
    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular)){
            memcpy(&mat->specular, &specular, sizeof(specular));
	}else{
		memcpy(&mat->specular, c, sizeof(c));
	}
 
    float shininess = 0.0;
    unsigned int max;
    aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max);
    mat->shininess = shininess;

	return mat;
}

GLuint LoadTextures(const aiScene* scene,aiMaterial* material){
		
	int texIndex = 0;
	aiString path;  // filename

	std::vector<char*> texNames;

	aiReturn texFound = material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path, NULL, NULL, NULL,NULL,NULL);

	while (texFound == AI_SUCCESS) {
		//fill map with textures, OpenGL image ids set to 0
		char buf[50];
		sprintf_s(buf,"%s",path.data);
		texNames.push_back(buf);

		// more textures?
		texIndex++;
		texFound = material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
	}


	int numTextures = texNames.size();

	if (numTextures == 0) return 0;
	
	GLuint nextTex = CreateTexture(texNames[0], GL_RGB, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	
	return nextTex;
}

void RenderStaticMesh(const StaticMesh& mesh)
{
	for (unsigned int meshNum = 0; meshNum < mesh.m_numMeshes; meshNum++){

		if (mesh.m_meshData[meshNum]->m_hasTexture){
			//glBindTexture(GL_TEXTURE_2D, mesh.m_meshData[meshNum]->m_texture);
		}

		glBindVertexArray(mesh.m_meshData[meshNum]->m_vertexBuffer);
		glDrawElements(GL_TRIANGLES, mesh.m_meshData[meshNum]->m_numFaces * 3, GL_UNSIGNED_INT, 0);
	}

}