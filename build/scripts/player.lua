
--SPRITE
sprite = SpriteComponent.new("assets/player.png")
add_component(sprite)

--INPUT
input = InputComponent.new()
add_component(input)

local move_up = input.register_action("move_up", "Up", "pressed")

local move_left = input.register_action("move_left", "Left", "pressed")

local move_right = input.register_action("move_right", "Right", "pressed")

local move_down = input.register_action("move_down", "Down", "pressed")

--MOVEMENT
input_dir = Vector2.new(0, 0)
velocity = Vector2.new(0, 0)
local move_speed = 100


--STATE MACHINE
script_component = ScriptComponent.new()
add_component(script_component)
state_manager = script_component.add_script('scripts/state_machine.lua')
player_states = script_component.add_script('scripts/player_states.lua')
state_manager.add_state("idle", player_states.idle_state)
state_manager.add_state("walk", player_states.walking_state)
state_manager.initialize(self)

--CAMERA
camera = CameraComponent.new(true)
add_component(camera)
camera.set_smooth(0.9)

--PROCESSES
function process(delta)
    state_manager.process(delta)
end

function process_input(event)
    state_manager.process_input(event)
end


function update_input_direction(event)
    input_dir.x = input.get_action_strength("move_right") - input.get_action_strength("move_left")
    input_dir.y = input.get_action_strength("move_down") - input.get_action_strength("move_up")
    return input_dir
end