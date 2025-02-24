
idle_state = {
    player = nil,
    initialize = function(context)
        idle_state.player = context
    end,
    enter = function()
        print("Entering Idle State")
    end,
    exit = function()
    end,
    process = function(delta)
    end,
    process_input = function(event)
        local input_dir = idle_state.player.update_input_direction(event)
        if input_dir.y ~= 0 or input_dir.x ~= 0 then
            idle_state.player.state_manager.change_state("walk")
        end
    end
}

walking_state = {
    player = nil,
    move_speed = 100,   
    input_dir = nil,
    initialize = function(context)
        walking_state.player = context
        walking_state.input_dir = walking_state.player.input_dir
    end,
    enter = function()
        print("Entering Walking State")
    end,
    exit = function()
    end,
    process = function(delta)
        local input_dir = walking_state.input_dir
        local position = walking_state.player.global_position
        local velocity = walking_state.player.velocity
        local move_speed = walking_state.move_speed
        velocity.x = input_dir.x
        velocity.y = input_dir.y
        position.x = position.x + input_dir.x * move_speed * delta
        position.y = position.y + input_dir.y * move_speed * delta
    end,

    process_input = function(event)
        walking_state.input_dir = walking_state.player.update_input_direction(event)
        local input_dir = walking_state.input_dir
        if input_dir.y == 0 and input_dir.x == 0 then
            walking_state.player.state_manager.change_state("idle")
            return
        end
    end
}
