#pragma once

#include <ituGL/core/DeviceGL.h>
#include <ituGL/renderer/RenderPass.h>
#include <ituGL/geometry/Drawcall.h>
#include <ituGL/geometry/Mesh.h>
#include <ituGL/shader/Material.h>
#include <glm/mat4x4.hpp>
#include <vector>
#include <unordered_map>
#include <memory>
#include <span>
#include <functional>

class Camera;
class Light;
class ShaderProgram;
class Material;
class VertexArrayObject;
class Drawcall;
class Model;
class FramebufferObject;

class Renderer
{
public:
    class DrawcallInfo
    {
    public:
        DrawcallInfo(const Material& material, unsigned int worldMatrixIndex, const VertexArrayObject& vao, const Drawcall& drawcall);

        const Material& GetMaterial() const { return m_material; }
        unsigned int GetWorldMatrixIndex() const { return m_worldMatrixIndex; }
        const VertexArrayObject& GetVAO() const { return m_vao; }
        const Drawcall& GetDrawcall() const { return m_drawcall; }

    private:
        std::reference_wrapper<const Material> m_material;
        unsigned int m_worldMatrixIndex;
        std::reference_wrapper<const VertexArrayObject> m_vao;
        std::reference_wrapper<const Drawcall> m_drawcall;
    };

    using DrawcallSupportedFunction = std::function<bool(const DrawcallInfo& drawcallInfo)>;
    class DrawcallCollection
    {
    public:
        DrawcallCollection(const DrawcallSupportedFunction &isSupported = nullptr);

        bool IsSupported(const DrawcallInfo& drawcallInfo) const;
        void SetSupportedFunction(const DrawcallSupportedFunction& isSupported);

        std::span<DrawcallInfo> GetDrawcalls() { return m_drawcallInfos; }
        std::span<const DrawcallInfo> GetDrawcalls() const { return m_drawcallInfos; }

        void AddDrawcall(const DrawcallInfo& drawcallInfo);
        void Clear();

    private:
        DrawcallSupportedFunction m_isSupported;
        std::vector<DrawcallInfo> m_drawcallInfos;
    };

    using DrawcallSortFunction = std::function<bool(const DrawcallInfo&, const DrawcallInfo&)>;

    using UpdateTransformsFunction = std::function<void(const ShaderProgram&, const glm::mat4&, const Camera&, bool)>;
    using UpdateLightsFunction = std::function<bool(const ShaderProgram&, std::span<const Light* const>, unsigned int&)>;

public:
    Renderer(DeviceGL& device);

    const DeviceGL& GetDevice() const { return m_device; }
    DeviceGL& GetDevice() { return m_device; }

    int AddRenderPass(std::unique_ptr<RenderPass> renderPass);

    bool HasCamera() const;
    const Camera& GetCurrentCamera() const;
    void SetCurrentCamera(const Camera& camera);

    std::shared_ptr<const FramebufferObject> GetDefaultFramebuffer() const;
    std::shared_ptr<const FramebufferObject> GetCurrentFramebuffer() const;
    void SetCurrentFramebuffer(std::shared_ptr<const FramebufferObject> framebuffer);

    std::span<const Light* const> GetLights() const;
    void AddLight(const Light& light);

    std::span<const DrawcallInfo> GetDrawcalls(unsigned int collectionIndex) const;
    void AddModel(const Model& model, const glm::mat4& worldMatrix);

    unsigned int AddDrawcallCollection(const DrawcallSupportedFunction &drawcallSupportedFunction);
    void SetDrawcallCollectionSupportedFunction(unsigned int index, const DrawcallSupportedFunction& drawcallSupportedFunction);

    void SortDrawcallCollection(unsigned int index, const DrawcallSortFunction& drawcallSortFunction);
    bool IsBackToFront(const DrawcallInfo& a, const DrawcallInfo& b) const;
    bool IsFrontToBack(const DrawcallInfo& a, const DrawcallInfo& b) const;

    const Mesh& GetFullscreenMesh() const;

    void RegisterShaderProgram(std::shared_ptr<const ShaderProgram> shaderProgramPtr,
        const UpdateTransformsFunction& updateTransformFunction,
        const UpdateLightsFunction& updateLightsFunction);

    void UpdateTransforms(std::shared_ptr<const ShaderProgram> shaderProgramPtr, const glm::mat4& worldMatrix, bool cameraChanged = true) const;
    void UpdateTransforms(std::shared_ptr<const ShaderProgram> shaderProgramPtr, unsigned int worldMatrixIndex, bool cameraChanged = true) const;

    UpdateLightsFunction GetDefaultUpdateLightsFunction(const ShaderProgram& shaderProgram);
    bool UpdateLights(std::shared_ptr<const ShaderProgram> shaderProgramPtr, std::span<const Light* const> lights, unsigned int& lightIndex) const;

    void PrepareDrawcall(const DrawcallInfo& drawcallInfo, Material::OverrideFlags materialOverride = Material::NoOverride);

    void SetLightingRenderStates(bool firstPass);

    void Render();

private:
    void Reset();

    void InitializeFullscreenMesh();

    const glm::mat4& GetWorldMatrix(const DrawcallInfo& drawcallInfo) const;

private:
    DeviceGL& m_device;

    const Camera *m_currentCamera;

    std::shared_ptr<const Material> m_currentMaterial;

    std::shared_ptr<const FramebufferObject> m_defaultFramebuffer;
    std::shared_ptr<const FramebufferObject> m_currentFramebuffer;

    std::vector<const Light*> m_lights;

    std::vector<glm::mat4> m_worldMatrices;

    std::vector<DrawcallCollection> m_drawcallCollections;

    std::unordered_map<std::shared_ptr<const ShaderProgram>, UpdateTransformsFunction> m_updateTransformsFunctions;
    std::unordered_map<std::shared_ptr<const ShaderProgram>, UpdateLightsFunction> m_updateLightsFunctions;

    Mesh m_fullscreenMesh;

    std::vector<std::unique_ptr<RenderPass>> m_passes;
};
