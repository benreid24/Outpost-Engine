target_sources(Core PUBLIC
    Constants.hpp
    DemoEngineState.hpp
    Game.hpp
    Properties.hpp
)

add_subdirectory(Components)
add_subdirectory(Input)
add_subdirectory(Player)
add_subdirectory(Systems)
add_subdirectory(World)
