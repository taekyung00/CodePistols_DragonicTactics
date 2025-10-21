# author Rudy Castan
# date 2025 Fall
# CS200 Computer Graphics I
# copyright DigiPen Institute of Technology

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(SOURCE_CODE 

    CS200/Image.hpp CS200/Image.cpp
    CS200/ImGuiHelper.hpp CS200/ImGuiHelper.cpp
    CS200/ImmediateRenderer2D.hpp CS200/ImmediateRenderer2D.cpp
    CS200/IRenderer2D.hpp
    CS200/NDC.hpp
    CS200/Renderer2DUtils.hpp CS200/Renderer2DUtils.cpp
    CS200/RenderingAPI.hpp CS200/RenderingAPI.cpp
    CS200/RGBA.hpp

    Demo/DemoShapes.hpp Demo/DemoShapes.cpp
    Demo/DemoFramebuffer.hpp Demo/DemoFramebuffer.cpp
    Demo/DemoText.hpp Demo/DemoText.cpp
    Demo/DemoVAO.hpp Demo/DemoVAO.cpp
    Demo/DemoBufferTypes.hpp Demo/DemoBufferTypes.cpp
    Demo/DemoTexturing.hpp Demo/DemoTexturing.cpp
    Demo/DemoCS230Textures.hpp Demo/DemoCS230Textures.cpp


    Game/Background.h Game/Background.cpp
    Game/GameObjectTypes.h
    Game/MainMenu.h Game/MainMenu.cpp
    Game/Particles.h
    Game/Score.h Game/Score.cpp
    Game/Splash.h Game/Splash.cpp
    Game/States.h

    Game/CS230_Final/Components/Bounce.h Game/CS230_Final/Components/Bounce.cpp
    Game/CS230_Final/Components/Grid.h Game/CS230_Final/Components/Grid.cpp
    Game/CS230_Final/Components/Random.h Game/CS230_Final/Components/Random.cpp
    Game/CS230_Final/Components/Ziggle.h Game/CS230_Final/Components/Ziggle.cpp

    Game/CS230_Final/Objects/Bus.h Game/CS230_Final/Objects/Bus.cpp
    Game/CS230_Final/Objects/BusLine.h
    Game/CS230_Final/Objects/Constants.h
    Game/CS230_Final/Objects/Bus.h Game/CS230_Final/Objects/Bus.cpp
    Game/CS230_Final/Objects/Food.h Game/CS230_Final/Objects/Food.cpp
    Game/CS230_Final/Objects/Obstacle.h Game/CS230_Final/Objects/Obstacle.cpp
    Game/CS230_Final/Objects/Passenger.h Game/CS230_Final/Objects/Passenger.cpp
    Game/CS230_Final/Objects/Player.h Game/CS230_Final/Objects/Player.cpp

    Game/CS230_Final/States/BadSandwich.h Game/CS230_Final/States/BadSandwich.cpp
    Game/CS230_Final/States/GoodSandwich.h Game/CS230_Final/States/GoodSandwich.cpp
    Game/CS230_Final/States/NormalSandwich.h Game/CS230_Final/States/NormalSandwich.cpp
    Game/CS230_Final/States/Project.h Game/CS230_Final/States/Project.cpp

    Game/DragonicTactics/Objects/Actions/Action.h Game/DragonicTactics/Objects/Actions/Action.cpp
    Game/DragonicTactics/Objects/Actions/ActionAttack.h Game/DragonicTactics/Objects/Actions/ActionAttack.cpp

    Game/DragonicTactics/Objects/Components/ActionPoints.h Game/DragonicTactics/Objects/Components/ActionPoints.cpp
    Game/DragonicTactics/Objects/Components/GridPosition.h Game/DragonicTactics/Objects/Components/GridPosition.cpp
    Game/DragonicTactics/Objects/Components/SpellSlots.h Game/DragonicTactics/Objects/Components/SpellSlots.cpp
    Game/DragonicTactics/Objects/Components/StatsComponent.h Game/DragonicTactics/Objects/Components/StatsComponent.cpp

    Game/DragonicTactics/Objects/Character.h Game/DragonicTactics/Objects/Character.cpp
    Game/DragonicTactics/Objects/Dragon.h Game/DragonicTactics/Objects/Dragon.cpp
    Game/DragonicTactics/Objects/Fighter.h Game/DragonicTactics/Objects/Fighter.cpp

    Game/DragonicTactics/Singletons/EventBus.h Game/DragonicTactics/Singletons/EventBus.cpp
    Game/DragonicTactics/Singletons/DiceManager.h Game/DragonicTactics/Singletons/DiceManager.cpp
    Game/DragonicTactics/Singletons/DataRegistry.cpp Game/DragonicTactics/Singletons/DataRegistry.h

    Game/DragonicTactics/StateComponents/GridSystem.h Game/DragonicTactics/StateComponents/GridSystem.cpp
    Game/DragonicTactics/StateComponents/TurnManager.h Game/DragonicTactics/StateComponents/TurnManager.cpp

    Game/DragonicTactics/States/Test.h 
    Game/DragonicTactics/States/Test.cpp Game/DragonicTactics/States/Test_Taekyung.cpp
    Game/DragonicTactics/States/Test_Junyoung.cpp Game/DragonicTactics/States/Test_Ginam.cpp

    Game/DragonicTactics/Test/Week1TestMocks.h Game/DragonicTactics/Test/Week1TestMocks.cpp Game/DragonicTactics/Test/Week1TestMocks.ini

    Game/DragonicTactics/Types/CharacterTypes.h
    Game/DragonicTactics/Types/Events.h
    Game/DragonicTactics/Types/GameObjectTypes.h
    Game/DragonicTactics/Types/GameTypes.h



    Engine/Engine.hpp Engine/Engine.cpp
    Engine/Error.hpp
    Engine/Font.hpp Engine/Font.cpp
    Engine/Fonts.h
    Engine/FPS.hpp
    Engine/GameState.hpp
    Engine/GameStateManager.hpp Engine/GameStateManager.cpp
    Engine/Input.hpp Engine/Input.cpp
    Engine/Logger.hpp Engine/Logger.cpp
    Engine/Matrix.hpp Engine/Matrix.cpp
    Engine/Path.hpp Engine/Path.cpp
    Engine/Random.hpp Engine/Random.cpp
    Engine/Rect.hpp
    Engine/Texture.hpp Engine/Texture.cpp
    Engine/TextureManager.hpp Engine/TextureManager.cpp
    Engine/TextManager.hpp Engine/TextManager.cpp
    Engine/Timer.hpp
    Engine/Vec2.hpp Engine/Vec2.cpp
    Engine/Window.hpp Engine/Window.cpp
    Engine/Animation.cpp Engine/Animation.h
    Engine/Camera.cpp Engine/Camera.h
    Engine/Collision.cpp Engine/Collision.h
    Engine/Component.h
    Engine/ComponentManager.h
    Engine/GameObject.cpp Engine/GameObject.h
    Engine/GameObjectManager.cpp Engine/GameObjectManager.h
    Engine/Particle.cpp Engine/Particle.h
    Engine/ShowCollision.cpp Engine/ShowCollision.h
    Engine/Sprite.cpp Engine/Sprite.h

    OpenGL/Buffer.hpp OpenGL/Buffer.cpp
    OpenGL/Environment.hpp
    OpenGL/Framebuffer.hpp OpenGL/Framebuffer.cpp
    OpenGL/GL.cpp OpenGL/GL.hpp
    OpenGL/GLConstants.hpp
    OpenGL/GLTypes.hpp
    OpenGL/Handle.hpp
    OpenGL/Shader.cpp OpenGL/Shader.hpp
    OpenGL/Texture.hpp OpenGL/Texture.cpp
    OpenGL/VertexArray.cpp OpenGL/VertexArray.hpp

    main.cpp
)

add_executable(dragonic_tactics ${SOURCE_CODE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_CODE})

target_link_libraries(dragonic_tactics PRIVATE project_options dependencies)
target_include_directories(dragonic_tactics PRIVATE .)

# Check the IS_DEVELOPER_VERSION cache variable
# This is set by the cmake configure preset
if (IS_DEVELOPER_VERSION)
    target_compile_definitions(dragonic_tactics PRIVATE DEVELOPER_VERSION _DEBUG)
endif()

if(EMSCRIPTEN)

    # https://emscripten.org/docs/tools_reference/settings_reference.html
    # ASSERTIONS=1                  - we want asserts to work
    # WASM=1                        - we want web assembly generated rather than just javascript
    # DISABLE_EXCEPTION_CATCHING=0  - we are using exceptions so let's not disable them
    # ALLOW_MEMORY_GROWTH=1         - we don't know exactly how much heap space we'll need so allow it to grow
    # EXIT_RUNTIME=1                - have exiting actually stop the program
    # SINGLE_FILE=1                 - generate everything into one html file
    # --embed-file                  - https://emscripten.org/docs/tools_reference/emcc.html#emcc-embed-file
    # --use-preload-cache           - help with faster reloads : https://emscripten.org/docs/compiling/Deploying-Pages.html#providing-a-quick-second-time-load
    # -lembind                      - to call c++ from javascript https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html
    # --shell-file                  - to customize the webpage https://emscripten.org/docs/compiling/Deploying-Pages.html#build-files-and-custom-shell
    target_link_options(dragonic_tactics PRIVATE 
    -sASSERTIONS=$<$<BOOL:${IS_DEVELOPER_VERSION}>:1>$<$<NOT:$<BOOL:${IS_DEVELOPER_VERSION}>>:0>
    -sWASM=1 
    -sDISABLE_EXCEPTION_CATCHING=0 
    -sALLOW_MEMORY_GROWTH=1 
    -sEXIT_RUNTIME=1 
    -sSINGLE_FILE=1 
    --embed-file ${CMAKE_SOURCE_DIR}/Assets@/Assets
    --use-preload-cache
    -lembind
    --shell-file ${CMAKE_SOURCE_DIR}/app_resources/web/index_shell.html
    )

    set_target_properties(dragonic_tactics PROPERTIES SUFFIX ".html")
elseif(WIN32)

    target_link_options(dragonic_tactics PRIVATE 
        $<IF:$<BOOL:${IS_DEVELOPER_VERSION}>,/SUBSYSTEM:CONSOLE,/SUBSYSTEM:WINDOWS>
        /ENTRY:mainCRTStartup)

    set(ICON_RC ${CMAKE_CURRENT_BINARY_DIR}/app.rc)
    if(NOT EXISTS ${ICON_RC})
        set(ICON_FILE ${CMAKE_SOURCE_DIR}/app_resources/windows/icon.ico)
        file(WRITE ${ICON_RC} "IDR_MAINFRAME ICON \"${ICON_FILE}\"\nIDI_ICON1 ICON DISCARDABLE \"${ICON_FILE}\"")
    endif()

    target_sources(dragonic_tactics PRIVATE ${ICON_RC})

endif()
