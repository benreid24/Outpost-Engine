target_sources(Core PRIVATE
    Game.cpp
    Properties.cpp
)

add_subdirectory(Commands)
add_subdirectory(Components)
add_subdirectory(Factions)
add_subdirectory(Input)
add_subdirectory(Player)
add_subdirectory(Systems)
add_subdirectory(Unit)
add_subdirectory(World)
