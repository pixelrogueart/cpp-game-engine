class_name = "Root"
tree.set_global_gravity(0,10)
tree.set_global_drag(0.1)

tree.set_gravity_scale(4)
tree.set_drag_scale(0.1)

world = Object2D.new()
add_child(world)



for i=1, 10 do
    player = Object2D.new()
    world.add_child(player)
    player.set_script("scripts/player.lua")
    player.set_global_position(25*i, 0)
end

enemy = Object2D.new()
world.add_child(enemy)
enemy.set_script("scripts/enemy.lua")

camera = CameraComponent.new(true)
add_component(camera)
camera.set_smooth(0.9)