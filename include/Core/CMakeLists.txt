target_sources(Core PUBLIC
    Constants.hpp
    DemoEngineState.hpp
    Game.hpp
    Properties.hpp
)

add_subdirectory(Commands)
add_subdirectory(Components)
add_subdirectory(Input)
add_subdirectory(Player)
add_subdirectory(Systems)
add_subdirectory(Unit)
add_subdirectory(World)
