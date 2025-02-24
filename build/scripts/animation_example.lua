local sprite = SpriteComponent("path/to/texture.png")

local animation = AnimationComponent(sprite)
animation:add_animation("walk", 0, 3, 10, true)

animation:play("walk")