require "application.lua.utils"
require "math"

DoorBlock = {
    bars = {
        left = {}, -- {cent_x, cent_y, points}
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
    local bars = DoorBlock.bars
    if next(bars.bottom) then
        local points = bars.bottom[3]
        local dist_p12 = point_dist(points[1], points[2])
        local dist_p23 = point_dist(points[2], points[3])
        if dist_p12 > dist_p23 then
            p1, p2 = points[1], points[2]
        else
            p1, p2 = points[2], points[3]
        end
        local deg = point_deg(p1, p2) - 90.0
        print(deg)
    end
end

function DoorBlock.update(raw_vision_line_results)
    DoorBlock.bars.left = {}
    DoorBlock.bars.right = {}
    DoorBlock.bars.bottom = {}
    for index = 1, #raw_vision_line_results do
        local points = raw_vision_line_results[index] --  bottomLeft, topLeft, topRight, bottomRight
        local dist_p12 = point_dist(points[1], points[2])
        local dist_p23 = point_dist(points[2], points[3])
        local length, width = math.max(dist_p12, dist_p23) / 2.0, math.min(dist_p12, dist_p23) / 2.0
        local area = width * length
        print(area)
        if area < 0.03 then
            goto continue
        end

        local cent_x = 0.0
        local cent_y = 0.0
        for i = 1, #points do
            cent_x = cent_x + points[i].x
            cent_y = cent_y + points[i].y
        end
        cent_x = cent_x / 4.0
        cent_y = cent_y / 4.0

        if points[4].x - points[1].x < points[2].y - points[1].y then
            -- 垂直的bar
            if cent_x > 0 then --右边垂直
                DoorBlock.bars.left = {cent_x, cent_y, points}
            else --左边垂直
                DoorBlock.bars.right = {cent_x, cent_y, points}
            end
            goto continue
        elseif points[1].y + points[2].y + points[3].y + points[4].y < 0 then
            -- 水平的bar
            DoorBlock.bars.bottom = {cent_x, cent_y, width, points}
        end

        :: continue ::
    end
end
