#include "WorldSerializer.h"
#include <iostream>
#include "World.h"
#include "Json.h"
#include "Components.h"
#include "Entity.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"

WorldSerializer::WorldSerializer(World* world)
	:world(world)
{
}

void WorldSerializer::Serialize(const std::string& path)
{
	/*
	Json json(Json::Object);
	
	json.Set("World", Json::Object);
	json["World"].Set("Entities", Json::Array);

	auto inc = 0;
	world->entRegistry.each([&](auto entityId)
	{
			const entt::entity entity = entityId;
		
			json["World"]["Entities"].Add(Json::Object);
		
			json["World"]["Entities"][inc].Set("EntityId", (unsigned)entityId);	
			json["World"]["Entities"][inc].Set("EntityName", world->entRegistry.get<TagComponent>(entity).tag);

			json["World"]["Entities"][inc].Set("Components", Json::Object);
			auto& jComponents = json["World"]["Entities"][inc]["Components"];

			{//Transform component
				auto& transformComponent = world->entRegistry.get<TransformComponent>(entity).GetTransform();			
				jComponents.Set("TransformComponent", Json::Array);
				
				jComponents["TransformComponent"].Add(Json::Array);
				jComponents["TransformComponent"].Add(Json::Array);
				jComponents["TransformComponent"].Add(Json::Array);
				jComponents["TransformComponent"].Add(Json::Array);

				for (auto i = 0; i < 4; i++)
					for (auto j = 0; j < 4; j++)
					{
						jComponents["TransformComponent"][i].Add(transformComponent[i][j]);
					}
			}
			{//Mesh Component
				if (world->entRegistry.any_of<MeshComponent>(entity))
				{
					auto& meshComponent = world->entRegistry.get<MeshComponent>(entity);
					jComponents.Set("MeshComponent", Json::Object);
					jComponents["MeshComponent"].Set("MeshVAO", meshComponent.mesh.vao);
					jComponents["MeshComponent"].Set("MeshIndices", meshComponent.mesh.indices);
					jComponents["MeshComponent"].Set("ShaderId", meshComponent.shader.shaderId);
					//jComponents["MeshComponent"].Set("TextureId", meshComponent.texture.textureID);
					//jComponents["MeshComponent"].Set("TextureWidth", meshComponent.texture.width);
					//jComponents["MeshComponent"].Set("TextureHeight", meshComponent.texture.height);
					//jComponents["MeshComponent"].Set("TextureDimensions", meshComponent.texture.dimensions);
					
					jComponents["MeshComponent"].Set("MaterialAmbient", Json::Array);
					jComponents["MeshComponent"].Set("MaterialDiffuse", Json::Array);
					jComponents["MeshComponent"].Set("MaterialSpecular", Json::Array);
					//jComponents["MeshComponent"].Set("MaterialShine", meshComponent.material.shine);

					for(auto i = 0; i<3; i++)
					{
						//jComponents["MeshComponent"]["MaterialAmbient"].Add(meshComponent.material.ambient[i]);
						//jComponents["MeshComponent"]["MaterialDiffuse"].Add(meshComponent.material.diffuse[i]);
						//jComponents["MeshComponent"]["MaterialSpecular"].Add(meshComponent.material.specular[i]);
					}				
				}
			}
			{//Box Collision Component
				if (world->entRegistry.any_of<AABBCollisionComponent>(entity))
				{
					auto& boxColl = world->entRegistry.get<AABBCollisionComponent>(entity);
					jComponents.Set("AABBCollisionComponent", Json::Object);
					auto& jBoxCol = jComponents["AABBCollisionComponent"];
					
					jBoxCol.Set("Type", (unsigned)boxColl.collisionVolume.GetType());
					jBoxCol.Set("ParentId", (unsigned)boxColl.collisionVolume.parentId);
					jBoxCol.Set("ResolveCollision", boxColl.bCollisionResolve);
					jBoxCol.Set("DrawVolume", boxColl.bDrawVolume);
					jBoxCol.Set("Static", boxColl.collisionVolume.bStatic);

					jBoxCol.Set("c", Json::Array);
					jBoxCol.Set("e", Json::Array);

					for(auto i= 0; i<3; i++)
					{
						jBoxCol["c"].Add(boxColl.collisionVolume.C[i]);
						jBoxCol["e"].Add(boxColl.collisionVolume.e[i]);
					}
									
				}
			}
			{//OBBCollisionComponent
				if (world->entRegistry.any_of<OBBCollisionComponent>(entity))
				{
					auto& boxColl = world->entRegistry.get<OBBCollisionComponent>(entity);
					jComponents.Set("OBBCollisionComponent", Json::Object);
					auto& jBoxCol = jComponents["OBBCollisionComponent"];

					jBoxCol.Set("Type", (unsigned)boxColl.collisionVolume.GetType());
					jBoxCol.Set("ParentId", (unsigned)boxColl.collisionVolume.parentId);
					jBoxCol.Set("ResolveCollision", boxColl.bCollisionResolve);
					jBoxCol.Set("DrawVolume", boxColl.bDrawVolume);
					jBoxCol.Set("Static", boxColl.collisionVolume.bStatic);

					jBoxCol.Set("c", Json::Array);
					jBoxCol.Set("e", Json::Array);
					jBoxCol.Set("orient", Json::Array);

					for (auto i = 0; i < 3; i++)
					{
						jBoxCol["c"].Add(boxColl.collisionVolume.C[i]);
						jBoxCol["e"].Add(boxColl.collisionVolume.e[i]);
						jBoxCol["orient"].Add(boxColl.collisionVolume.orient[i]);
					}
					jBoxCol["orient"].Add(boxColl.collisionVolume.orient.x);
					jBoxCol["orient"].Add(boxColl.collisionVolume.orient.y);
					jBoxCol["orient"].Add(boxColl.collisionVolume.orient.z);
					jBoxCol["orient"].Add(boxColl.collisionVolume.orient.w);
				}
			}
			{//physics component
				if (world->entRegistry.any_of<PhysicsComponent>(entity))
				{
					jComponents.Set("PhysicsComponent", Json::Object);
					auto& physics = world->entRegistry.get<PhysicsComponent>(entity);
					auto& jPhysics = jComponents["PhysicsComponent"];
					auto& rigidBody = physics.rigidBody;
					
					jPhysics.Set("Acceleration", Json::Array);
					jPhysics.Set("AngularVelocity", Json::Array);
					jPhysics.Set("ForceAccum", Json::Array);
					jPhysics.Set("LastFrameAcceleration", Json::Array);
					jPhysics.Set("LinearVelocity", Json::Array);
					jPhysics.Set("Position", Json::Array);
					
					jPhysics.Set("AngularDamping", rigidBody.angularDamping);
					jPhysics.Set("IsActive", rigidBody.bIsActive);
					jPhysics.Set("InverseMass", rigidBody.inverseMass);
					jPhysics.Set("LinearDamping", rigidBody.linearDamping);
					jPhysics.Set("Motion", rigidBody.motion);
					
					for(int i= 0; i<3; i++)
					{
						jPhysics["Acceleration"].Add(rigidBody.acceleration[i]);
						jPhysics["AngularVelocity"].Add(rigidBody.angularVelocity[i]);
						jPhysics["ForceAccum"].Add(rigidBody.forceAccum[i]);
						jPhysics["LastFrameAcceleration"].Add(rigidBody.lastFrameAcceleration[i]);
						jPhysics["LinearVelocity"].Add(rigidBody.velocity[i]);
						jPhysics["Position"].Add(rigidBody.position[i]);
					}
					
					jPhysics.Set("InverseInteria", Json::Array);
					
					jPhysics["InverseInteria"].Add(Json::Array);
					jPhysics["InverseInteria"].Add(Json::Array);
					jPhysics["InverseInteria"].Add(Json::Array);

					for (int i = 0; i < 3; i++)
						for (int j = 0; j < 3; j++)
							jPhysics["InverseInteria"][i].Add(rigidBody.inverseInteriaTensorWorld[i][j]);
											
				}
			}
		
			inc++;
	});
	json.Print();
	json.Save(path);
	*/
}

void WorldSerializer::Deserialize(const std::string& path)
{
	/*
	Json json = Json::Load(path);
	json.Print();
	world->entRegistry.clear<>();

	for (auto entityPair : json["World"]["Entities"])
	{
		Json entity = entityPair.Value();		

		const entt::entity entityId = (entt::entity)(unsigned)entity["EntityId"];
		std::string entityName = entity["EntityName"];		

		Entity ent = world->CreateEntity(entityName);

		{//Transform component
			TransformComponent transformComponent;
			for (auto i = 0; i < 4; i++)
				for (auto j = 0; j < 4; j++)
				{
					transformComponent.GetTransform()[i][j] = entity["Components"]["TransformComponent"][i][j];
				}
			
			auto& entTransform = world->entRegistry.get<TransformComponent>(ent.id);
			entTransform = transformComponent;
		}

		{//MeshComponent
			if (entity["Components"].Contains("MeshComponent"))
			{
				MeshComponent meshComponent;
				Json jsonMesh = entity["Components"]["MeshComponent"];
				meshComponent.mesh.vao = jsonMesh["MeshVAO"];
				meshComponent.mesh.indices = jsonMesh["MeshIndices"];
				meshComponent.shader.shaderId = jsonMesh["ShaderId"];
				//meshComponent.texture.textureID = jsonMesh["TextureId"];
				//meshComponent.texture.width = jsonMesh["TextureWidth"];
				//meshComponent.texture.height = jsonMesh["TextureHeight"];
				//meshComponent.texture.dimensions = jsonMesh["TextureDimensions"];
				//meshComponent.material.shine = jsonMesh["MaterialShine"];
				
				for (auto i = 0; i < 3; i++)
				{
					//meshComponent.material.ambient[i] = jsonMesh["MaterialAmbient"];
					//meshComponent.material.diffuse[i] = jsonMesh["MaterialDiffuse"];
					//meshComponent.material.specular[i] = jsonMesh["MaterialSpecular"];
				}
				auto& entMesh = ent.AddComponent<MeshComponent>();
				entMesh = meshComponent;
			}	
		}
		{//AABBCollisionComponent
			if (entity["Components"].Contains("AABBCollisionComponent"))
			{
				Json jBoxCol = entity["Components"]["AABBCollisionComponent"];
				glm::vec3 c, e;
				for (auto i = 0; i < 3; i++)
				{
					c[i] = jBoxCol["c"][i];
					e[i] = jBoxCol["e"][i];
				}

				auto& entBoxCol = ent.AddComponent<AABBCollisionComponent>(c, e, (std::uint32_t)ent.id);
				entBoxCol.bDrawVolume = jBoxCol["DrawVolume"];
				entBoxCol.bCollisionResolve = jBoxCol["ResolveCollision"];
				entBoxCol.collisionVolume.bStatic = jBoxCol["Static"];
			}
		}
		{//OBBCollisionComponent
			if (entity["Components"].Contains("OBBCollisionComponent"))
			{
				Json jBoxCol = entity["Components"]["OBBCollisionComponent"];
				glm::vec3 c, e;
				glm::quat orient;
				for (auto i = 0; i < 3; i++)
				{
					c[i] = jBoxCol["c"][i];
					e[i] = jBoxCol["e"][i];					
				}
				orient.x = jBoxCol["orient"][0];
				orient.y = jBoxCol["orient"][1];
				orient.z = jBoxCol["orient"][2];
				orient.w = jBoxCol["orient"][3];
								
				auto& entBoxCol = ent.AddComponent<OBBCollisionComponent>((std::uint32_t)ent.id);
				entBoxCol.bDrawVolume = jBoxCol["DrawVolume"];
				entBoxCol.bCollisionResolve = jBoxCol["ResolveCollision"];
				entBoxCol.collisionVolume.bStatic = jBoxCol["Static"];
				entBoxCol.collisionVolume.C = c;
				entBoxCol.collisionVolume.e = e;
				entBoxCol.collisionVolume.orient = orient;
			}
		}
		{//physics component
			if (entity["Components"].Contains("PhysicsComponent"))
			{
				auto& jPhysics = entity["Components"]["PhysicsComponent"];
				PhysicsComponent physicsComp;
				auto& rigidBody = physicsComp.rigidBody;
				
				rigidBody.angularDamping = jPhysics["AngularDamping"];
				rigidBody.bIsActive = jPhysics["IsActive"];
				rigidBody.inverseMass = jPhysics["InverseMass"];
				rigidBody.linearDamping = jPhysics["LinearDamping"];
				rigidBody.motion = jPhysics["Motion"];
					
				for (int i = 0; i < 3; i++)
				{
					rigidBody.acceleration[i] = jPhysics["Acceleration"][i];
					rigidBody.angularVelocity[i] = jPhysics["AngularVelocity"][i];
					rigidBody.forceAccum[i] = jPhysics["ForceAccum"][i];
					rigidBody.lastFrameAcceleration[i] = jPhysics["LastFrameAcceleration"][i];
					rigidBody.velocity[i] = jPhysics["LinearVelocity"][i];
					rigidBody.position[i] = jPhysics["Position"][i];
				}

				for (int i = 0; i < 3; i++)
					for (int j = 0; j < 3; j++)
						rigidBody.inverseInteriaTensorWorld[i][j] = jPhysics["InverseInteria"][i][j];

				auto& entPhysics = ent.AddComponent<PhysicsComponent>();
				entPhysics = physicsComp;
			}
		}
	}
	*/
}


