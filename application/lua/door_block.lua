require "application.lua.utils"
require "math"

DoorBlock = {
    bars = {
        left = {}, -- x, y, width, height  中心点(x,y)
        right = {},
        bottom = {}
    },
    motions = function(deg, dev)
        return function(server)
            server.move { x = -dev, y = 0.3, z = 0.0, rot = -deg / 90.0 }
        end
    end
}

function DoorBlock.process()

end

function DoorBlock.update(raw_vision_line_results)
    DoorBlock.bars.left = {}
    DoorBlock.bars.right = {}
    DoorBlock.bars.bottom = {}
    for i = 1, #raw_vision_line_results do
        local x, y, width, height = raw_vision_line_results[i].x, raw_vision_line_results[i].y, raw_vision_line_results[i].width, raw_vision_line_results[i].height
        local area = width * height
        print(area)
        if area < 0.03 then
            goto continue
        end

        local cent_x = x + width / 2
        local cent_y = y + height / 2
        if raw_vision_line_results[i].width < raw_vision_line_results[i].height then
            -- 垂直的bar
            if cent_x > 0 then --右边垂直
                DoorBlock.bars.left = {cent_x, cent_y, width, height}
            else --左边垂直
                DoorBlock.bars.right = {cent_x, cent_y, width, height}
            end
        else
            -- 水平的bar
            DoorBlock.bars.bottom = {cent_x, cent_y, width, height}
        end
        :: continue ::
    end
end
