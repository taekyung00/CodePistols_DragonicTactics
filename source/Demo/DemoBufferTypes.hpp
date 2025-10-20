/**
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "CS200/NDC.hpp"
#include "CS200/RGBA.hpp"
#include "Engine/GameState.hpp"
#include "OpenGL/Buffer.hpp"
#include "OpenGL/Shader.hpp"
#include "OpenGL/VertexArray.hpp"
#include <array>
#include <vector>

class DemoBufferTypes : public CS230::GameState
{
public:
    void          Load() override;
    void          Unload() override;
    void          Update(double dt) override;
    void          Draw()  override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;

private:
    OpenGL::CompiledShader backgroundShader{};
    OpenGL::CompiledShader villagerShader{};
    OpenGL::BufferHandle   uniformBlock{};
    std::array<float, 12>  toNDC{};

    struct object
    {
        OpenGL::BufferHandle      vertexBufferHandle{};
        OpenGL::BufferHandle      indexBufferHandle{};
        GLsizei                   indicesCount{};
        OpenGL::VertexArrayHandle modelHandle{};
    } background{}, villager{};

    static constexpr std::size_t MAX_NUMBER_VILLAGERS = 16384;

    struct VillagerData
    {
        Math::vec2 position{};
        Math::vec2 targetPosition{};
        Math::vec2 scale{ 16.0, 16.0 };
        Math::vec2 targetScale{};
        double     rotation{};
        double     targetRotation{};

        // Animation parameters
        Math::vec2 positionOffset{};
        Math::vec2 scaleOffset{};
        double     rotationSpeed{};
        Math::vec2 positionSpeed{};
        Math::vec2 scaleSpeed{};
        double     timeOffset{};
        bool       alive{ true };
        bool       shouldDraw{ true };
    };

    std::vector<VillagerData> villagers;
    Math::vec2                currentDisplaySize{};

    constexpr static int QuadSize = 64;

    // Noise generation constants
    constexpr static float NoiseFrequency  = 1.0f / (static_cast<float>(QuadSize) * 4.0f);
    constexpr static float NoiseLacunarity = 2.0f;
    constexpr static float NoiseGain       = 0.5f;
    constexpr static int   NoiseLayers     = 5;

    // Animation wave constants
    constexpr static float WaveFrequency1 = 1.0f;
    constexpr static float WaveFrequency2 = 2.3f;
    constexpr static float WaveFrequency3 = 0.7f;
    constexpr static float WaveMix1       = 0.5f;
    constexpr static float WaveMix2       = 0.3f;
    constexpr static float WaveMix3       = 0.2f;

    struct Vertex
    {
        float                        x, y;
        std::array<unsigned char, 4> color;
    };

    std::vector<Vertex>         backgroundVertices;
    std::vector<unsigned short> backgroundIndices;

private:
    void        loadShaders();
    void        createVillagerModel();
    void        bounceVillager();
    void        updateNDCUniformValues();
    void        updateBackgroundModel();
    static void drawObject(const object& object);

    // Helper functions for updateBackgroundModel()
    struct AnimationParams
    {
        float elapsedTime;
        float quadTiltSize;
    };

    struct GridParams
    {
        int numQuadsWide;
        int numQuadsTall;
    };

    static AnimationParams calculateAnimationParameters();
    bool                   ensureBackgroundBuffersCapacity(const GridParams& grid_params);
    void                   generateQuadVertices(int row, int column, const GridParams& grid_params, const AnimationParams& anim_params);
    void                   updateBackgroundBuffersOnGPU(int num_required_vertices, bool indices_need_update) const;

    void increase_number_of_villagers(std::size_t current_size);
    void decrease_number_of_villagers(std::size_t current_size);
    void regenerate_all_villagers();
    void generateRandomVillager(VillagerData& villagerData, const Math::vec2& screenSize) const;
    void updateVillagers();
};
