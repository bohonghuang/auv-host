require "application.lua.utils"
require "math"

DoorBlock = {
    motions = {

    }
}

function DoorBlock.update(raw_vision_line_results)
    local count = 0
    for i = 1, #raw_vision_line_results do
       print(raw_vision_line_results[i].x, raw_vision_line_results[i].y)
    end
end
