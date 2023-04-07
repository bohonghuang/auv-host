require "application.lua.utils"
require "math"

DoorBlock = {
    horizontal_theta = 0.0, --   {theta, {x0, y0}}
    motions = {

    }
}

function DoorBlock.update(raw_vision_line_results)
    DoorBlock.horizontal_theta = 0
    -- DoorBlock.horizontal = {}
    -- DoorBlock.vertical = {}

    local horizontal_theta = 0.0
    -- local horizontal_point = { x0 = 0, y0 = 0 }
    local count = 0
    for i = 1, #raw_vision_line_results do
        -- local point = raw_vision_line_results[i].point
        local theta = raw_vision_line_results[i].theta
        if math.abs(theta) < math.pi / 4 then
            count = count + 1
            horizontal_theta = theta + horizontal_theta
        end
    end

    if count ~= 0 then
        DoorBlock.horizontal_theta = horizontal_theta / count
        print(math.deg(DoorBlock.horizontal_theta))
    end

end
