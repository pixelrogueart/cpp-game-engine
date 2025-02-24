class_name = "Root"

world = Object2D.new()
add_child(world)

player = Object2D.new()
world.add_child(player)
player.set_script("scripts/player.lua")
