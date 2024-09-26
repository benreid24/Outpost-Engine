target_sources(Core PRIVATE
    Game.cpp
    Properties.cpp
)

add_subdirectory(Components)
add_subdirectory(Input)
add_subdirectory(Systems)
