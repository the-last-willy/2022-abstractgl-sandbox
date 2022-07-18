#pragma once

#include "material/material.hpp"
#include "mesh/mesh.hpp"
#include "mesh/vertex_array.hpp"
#include "scene_graph/scene_graph.hpp"

#include "common/dependency/abstractgl_api_opengl.hpp"
#include "common/dependency/glm.hpp"
#include "common/opengl/debug_message_callback.hpp"
#include "common/all.hpp"

#include <agl/standard/all.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <filesystem>
#include <optional>
#include <span>
#include <vector>

struct TextureResource
{
	std::filesystem::path file_path;

	std::optional<gl::TextureObject> gpu;
};

struct LittlestTokyo {
	float dt = 1.f / 60.f;

	gl::FramebufferObject render_framebuffer;

	glsl::DepthRenderer depth_renderer;
    glsl::SolidRenderer solid_renderer;

    SceneGraph scene;

	std::vector<Material> materials;
    std::vector<Mesh> meshes;
    std::vector<gl::VertexArrayObj> mesh_solid_renderer_vertex_arrays;

	std::vector<TextureResource> textures;

	gizmo::triangle::Quad quad;
	gl::VertexArrayObj quad_solid_renderer;
	glm::vec3 quad_position = glm::vec3(0.f);
	float quad_scale = 1.f;

	gizmo::Solid_UV_Sphere sphere = gizmo::Solid_UV_Sphere(30, 30);
	gl::VertexArrayObj sphere_depth_renderer_va;
	gl::VertexArrayObj sphere_solid_renderer_va;
	glm::vec3 sphere_position = glm::vec3(0.f);
	glm::vec3 sphere_scale = glm::vec3(1.f);

    // Camera.
	float camera_speed = 1.f;
    glm::mat4 view_to_clip = glm::mat4(1.f);
    glm::mat4 world_to_clip = glm::mat4(1.f);
    glm::mat4 world_to_view = glm::mat4(1.f);
    glm::vec2 yaw_pitch = glm::vec2(0.f, 0.f);
    glm::vec3 camera_position = glm::vec3(0.f);
};

void init(LittlestTokyo& _this) {
    auto path_to_scene = std::string(
		"D:/data/3d_model/sketchfab/sketchfab_3d_editor_challenge_littlest_tokyo/scene.gltf");
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
		path_to_scene,
		aiProcess_Triangulate
		| aiProcess_FlipUVs);
    if(scene == nullptr) {
		throw std::runtime_error(
			"Failed to open 2D scene.");
    }
    { // Meshes.
        for(unsigned mi = 0; mi < scene->mNumMeshes; ++mi) {
            auto& ai_mesh = *scene->mMeshes[mi];
            auto& gl_mesh = _this.meshes.emplace_back();
            gl_mesh.draw_mode = GL_TRIANGLES;
            gl_mesh.draw_type = GL_UNSIGNED_INT;
            if(ai_mesh.HasFaces()) {
                auto indices = std::vector<unsigned>();
                for(unsigned fi = 0; fi < ai_mesh.mNumFaces; ++fi) {
                    auto& face = ai_mesh.mFaces[fi];
                    for(unsigned ii = 0; ii < face.mNumIndices; ++ii) {
                        indices.push_back(face.mIndices[ii]);
                    }
                }
                gl_mesh.draw_count = GLsizei(size(indices));
                gl::NamedBufferStorage(gl_mesh.indices, indices, gl::NONE);
            }
            if(ai_mesh.HasNormals()) {
                gl::NamedBufferStorage(gl_mesh.normals,
                    std::span(ai_mesh.mNormals, ai_mesh.mNumVertices), gl::NONE);
                
            }
            if(ai_mesh.HasPositions()) {
                gl::NamedBufferStorage(gl_mesh.positions,
                    std::span(ai_mesh.mVertices, ai_mesh.mNumVertices), gl::NONE);
            }
			if(ai_mesh.HasTextureCoords(0)) {
				gl::NamedBufferStorage(gl_mesh.texcoords0,
					std::span(ai_mesh.mTextureCoords[0], ai_mesh.mNumVertices), gl::NONE);
			}
        }
    }
    { // Nodes.
        auto traversal = [&](SceneGraph& gl_node, const aiNode& ai_node, auto self) -> void {
            gl_node.transform[0][0] = ai_node.mTransformation.a1;
            gl_node.transform[0][1] = ai_node.mTransformation.a2;
            gl_node.transform[0][2] = ai_node.mTransformation.a3;
            gl_node.transform[0][3] = ai_node.mTransformation.a4;
            gl_node.transform[1][0] = ai_node.mTransformation.b1;
            gl_node.transform[1][1] = ai_node.mTransformation.b2;
            gl_node.transform[1][2] = ai_node.mTransformation.b3;
            gl_node.transform[1][3] = ai_node.mTransformation.b4;
            gl_node.transform[2][0] = ai_node.mTransformation.c1;
            gl_node.transform[2][1] = ai_node.mTransformation.c2;
            gl_node.transform[2][2] = ai_node.mTransformation.c3;
            gl_node.transform[2][3] = ai_node.mTransformation.c4;
            gl_node.transform[3][0] = ai_node.mTransformation.d1;
            gl_node.transform[3][1] = ai_node.mTransformation.d2;
            gl_node.transform[3][2] = ai_node.mTransformation.d3;
            gl_node.transform[3][3] = ai_node.mTransformation.d4;
            gl_node.transform = glm::transpose(gl_node.transform);
            for(unsigned mi = 0; mi < ai_node.mNumMeshes; ++mi) {
                gl_node.meshes.push_back(MeshId(ai_node.mMeshes[mi]));
            }
            for(unsigned ci = 0; ci < ai_node.mNumChildren; ++ci) {
                self(
                    *gl_node.children.emplace_back(std::make_unique<SceneGraph>()),
                    *ai_node.mChildren[ci],
                    self);
            }
        };
        traversal(_this.scene, *scene->mRootNode, traversal);
    }
	{// Materials.
		_this.materials.resize(scene->mNumMaterials);
		for(unsigned mi = 0; mi < scene->mNumMaterials; ++mi)
		{
			auto& ai_material = *scene->mMaterials[mi];
			auto& our_material = _this.materials.emplace_back();
			if(ai_material.GetTextureCount(aiTextureType_BASE_COLOR) > 0) {
				auto texture_path = aiString();
				ai_material.GetTexture(aiTextureType_BASE_COLOR, 0, &texture_path);

			}
		}
	}
	if constexpr(true) { // Materials.

		for(unsigned mi = 0; mi < scene->mNumMaterials; ++mi)
		{
			auto& ai_material = *scene->mMaterials[mi];
			std::cout << "Material: " << ai_material.GetName().C_Str() << '\n';
			for(unsigned pi = 0; pi < ai_material.mNumProperties; ++pi) {
				auto& ai_property = *ai_material.mProperties[pi];
				aiTextureType;
				aiPTI_Float;
				std::cout << ai_property.mKey.C_Str() << " " << ai_property.mSemantic << " " << ai_property.mIndex << " " << ai_property.mDataLength << " " << ai_property.mType << '\n';
				if(ai_property.mSemantic > 0) {
					std::cout << "  " << ai_property.mSemantic << " " << ai_property.mType << '\n';
				}
			}
			for(unsigned tt = 0; tt <= 30; ++tt) {
				for(unsigned ti = 0; ti < ai_material.GetTextureCount(aiTextureType(tt)); ++ti) {
					auto path = aiString();
					ai_material.GetTexture(
						aiTextureType(tt), ti,
						&path,
						NULL, NULL, NULL, NULL, NULL);
					std::cout << "  Texture: " << tt << " " << path.C_Str() << " " << ti << '\n';
				}
			}
		}
		//aiTextureType_AMBIENT;
	}
	{ // Textures.
		for(unsigned ti = 0; ti < scene->mNumTextures; ++ti) {
			auto& ai_texture = *scene->mTextures[ti];

		}
	}
    { // Solid renderer.
        _this.solid_renderer = glsl::solid_renderer();
    }
    { // Meshes / Solid renderer vertex arrays.
        _this.mesh_solid_renderer_vertex_arrays.resize(size(_this.meshes));
        for(std::size_t i = 0; i < size(_this.meshes); ++i) {
            _this.mesh_solid_renderer_vertex_arrays[i]
            = vertex_array(_this.meshes[i], _this.solid_renderer);
        }
    }
    { // Camera.
        _this.view_to_clip = glm::perspective(
            3.141593f / 2.f,
            16.f / 9.f,
            0.1f,
            1000.f);
    }
    if constexpr(false) {
        auto traverse = [&](const SceneGraph& sg, glm::mat4 parent_transform, auto self) -> void {
            parent_transform = parent_transform * sg.transform;
            std::cout << "parent_transform:\n";
            for(int j = 0; j < 4; ++j) {
                std::cout << "[";
                for(int i = 0; i < 4; ++i) {
                    std::cout << std::setw(16) << parent_transform[i][j] << " ";
                }
                std::cout << "]\n";
            }
            std::cout << "\n";
            std::cout << "sg.transform:\n";
            for(int j = 0; j < 4; ++j) {
                std::cout << "[";
                for(int i = 0; i < 4; ++i) {
                    std::cout << std::setw(16) << sg.transform[i][j] << " ";
                }
                std::cout << "]\n";
            }
            std::cout << "\n\n";
            for(auto& c : sg.children) {
                self(*c, parent_transform, self);
            }
        };
        // traverse(_this.scene, glm::mat4(1.f), traverse);
    }
	{ // Quad.
		_this.quad_solid_renderer = vertex_array(
			_this.quad,
			_this.solid_renderer);
	}
	{ // Sphere.
		_this.sphere_depth_renderer_va = vertex_array(
			_this.sphere,
			_this.depth_renderer);
		_this.sphere_solid_renderer_va = vertex_array(
			_this.sphere,
			_this.solid_renderer);
	}
	{ // Framebuffer.

	}
}

void update(LittlestTokyo& _this) {
    auto& io = ImGui::GetIO();
    if(not io.WantCaptureKeyboard) {
        auto forward = glm::vec3(inverse(_this.world_to_view) * glm::vec4(0.f, 0.f, -1.f, 0.f));
        auto right = glm::vec3(inverse(_this.world_to_view) * glm::vec4(+1.f, 0.f, 0.f, 0.f));
        if(ImGui::IsKeyDown('A')) {
            _this.camera_position += right * _this.camera_speed * _this.dt;
        }
        if(ImGui::IsKeyDown('D')) {
            _this.camera_position -= right * _this.camera_speed * _this.dt;
        }
        if(ImGui::IsKeyDown('S')) {
            _this.camera_position += forward * _this.camera_speed * _this.dt;
        }
        if(ImGui::IsKeyDown('W')) {
            _this.camera_position -= forward * _this.camera_speed * _this.dt;
        }
    }
    if(not io.WantCaptureMouse) {
        if(ImGui::IsMouseDown(0)) {
            _this.yaw_pitch += glm::vec2(io.MouseDelta[0], io.MouseDelta[1]) / 100.f;
        }
    }
    { // Camera.
        _this.world_to_view = glm::translate(
            glm::rotate(
                glm::rotate(
                    glm::mat4(1.f),
                    _this.yaw_pitch.y,
                    glm::vec3(1.f, 0.f, 0.f)),
                _this.yaw_pitch.x,
                glm::vec3(0.f, 1.f, 0.f)),
            _this.camera_position);
        _this.world_to_clip = _this.view_to_clip * _this.world_to_view;
    }
}

inline
void render_ui(LittlestTokyo& _this)
{
	if(ImGui::Begin("Settings")) {
		if(ImGui::TreeNode("Camera")) {
			ImGui::DragFloat("Speed",
				&_this.camera_speed,
				1.0f, 0.0f, 100.0f, "%.3f",
				ImGuiSliderFlags_Logarithmic);
			ImGui::TreePop();
		}
		if(ImGui::TreeNode("Plane")) {
			ImGui::DragFloat3("Position",
				&_this.quad_position[0],
				1.0f, 0.0f, 100.0f, "%.3f");
			ImGui::DragFloat("Scale",
				&_this.quad_scale,
				1.0f, 1.0f, 100.0f, "%.3f");
			ImGui::TreePop();
		}
		if(ImGui::TreeNode("Hole")) {
			ImGui::DragFloat3("Position",
				&_this.sphere_position[0],
				1.0f, 0.0f, 100.0f, "%.3f");
			ImGui::DragFloat3("Scale",
				&_this.sphere_scale[0],
				1.0f, 0.0f, 100.0f, "%.3f");
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

void render(LittlestTokyo& _this)
{
	gl::ClearNamedFramebuffer(gl::ZERO, gl::COLOR, 0, {0.f, 0.f, 0.f, 1.f});
	gl::ClearNamedFramebuffer(gl::ZERO, gl::DEPTH, 1.f);

	{ // Littlest tokyo.
		gl::UseProgram(_this.solid_renderer.program);

		glDepthFunc(GL_LESS);
		auto depth_cap = scoped(gl::Enable(GL_DEPTH_TEST));

		auto traverse_and_draw = [&](const SceneGraph &sg,
			glm::mat4 parent_transform, auto self) -> void {
			parent_transform = parent_transform * sg.transform;
			auto object_to_clip = _this.world_to_clip * parent_transform;

			glProgramUniformMatrix4fv(_this.solid_renderer.program,
				_this.solid_renderer.object_to_clip,
				1, GL_FALSE, &object_to_clip[0][0]);
			glProgramUniformMatrix4fv(_this.solid_renderer.program,
				_this.solid_renderer.object_to_world_position,
				1, GL_FALSE, &parent_transform[0][0]);

			for(auto mi: sg.meshes) {
				auto &mesh = _this.meshes[mi];
				auto &va = _this.mesh_solid_renderer_vertex_arrays[mi];

				gl::BindVertexArray(va);

				gl::DrawElements(mesh.draw_mode,
					mesh.draw_count,
					mesh.draw_type,
					0);
			}
			for(auto &c: sg.children) {
				self(*c, parent_transform, self);
			}
		};
		traverse_and_draw(_this.scene, glm::mat4(1.f), traverse_and_draw);
	}

	{ // Quad.
		gl::UseProgram(_this.solid_renderer.program);

		gl::BindVertexArray(_this.quad_solid_renderer);

		auto object_to_world = glm::translate(
			glm::scale(
				glm::identity<glm::mat4>(),
				glm::vec3(_this.quad_scale)),
			_this.quad_position);

		auto object_to_clip = _this.world_to_clip * object_to_world;

		glProgramUniformMatrix4fv(_this.solid_renderer.program,
			_this.solid_renderer.object_to_clip,
			1, GL_FALSE,
			&object_to_clip[0][0]);
		glProgramUniformMatrix4fv(_this.solid_renderer.program,
			_this.solid_renderer.object_to_world_position,
			1, GL_FALSE,
			&object_to_world[0][0]);

		gl::DrawElements(
			_this.quad.mode,
			_this.quad.count,
			_this.quad.type,
			0);
	}

	if constexpr(false) { // Sphere.
		gl::UseProgram(_this.solid_renderer.program);

		gl::BindVertexArray(_this.sphere_solid_renderer_va);

		auto object_to_world = glm::translate(
			glm::scale(
				glm::identity<glm::mat4>(),
				_this.sphere_scale),
			_this.sphere_position);

		auto object_to_clip = _this.world_to_clip * object_to_world;

		glProgramUniformMatrix4fv(_this.solid_renderer.program,
			_this.solid_renderer.object_to_clip,
			1, GL_FALSE,
			&object_to_clip[0][0]);
		glProgramUniformMatrix4fv(_this.solid_renderer.program,
			_this.solid_renderer.object_to_world_position,
			1, GL_FALSE,
			&object_to_world[0][0]);

		gl::DrawElements(
			_this.sphere.mode,
			_this.sphere.count,
			_this.sphere.type,
			0);
	}

	render_ui(_this);
}
