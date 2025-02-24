sprite = SpriteComponent.new("assets/enemy.png")
add_component(sprite)


physics_component = PhysicsComponent.new(1, true)
add_component(physics_component)
local box_collider = ColliderComponent.new(ColliderShape.Box, Vector2.new(256, 32), Vector2.new(0, 0), false)
box_collider:enable()   
add_component(box_collider)

function process(delta)
    set_global_position(0,100)
end