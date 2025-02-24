initial_state = "idle"
current_state = "idle"
states = {}
context = nil

-- Initialize the state machine
function initialize(_context)
    context = _context
    if not context then
        print("Context not provided")
        return
    end

    print("State machine initialized")
    initial_state = initial_state or initial_state
    current_state = initial_state

    -- Loop through all states and call an `initialize` function if it exists
    for state_name, state in pairs(states) do
        if state.initialize then
            print("Initializing state: " .. state_name)
            state.initialize(context)
        else
            print("State '" .. state_name .. "' has no initialize function")
        end
    end

    -- Enter the initial state if it has an `enter` function
    if states[current_state] and states[current_state].enter then
        states[current_state].enter()
    end
end


-- Add a new state
function add_state(name, state)
    states[name] = state
end

-- Change the current state
function change_state(new_state)
    if states[current_state] and states[current_state].exit then
        states[current_state].exit()
    end
    current_state = new_state
    if states[current_state] and states[current_state].enter then
        states[current_state].enter()
    end
end

-- Process the current state
function process(delta)
    if states[current_state] and states[current_state].process then
        states[current_state].process(delta)
    end
end

function process_input(event)
    if states[current_state] and states[current_state].process_input then
        states[current_state].process_input(event)
    end
end
