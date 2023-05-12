require "application.lua.utils"
require "math"

local find_count = 1

local function bottom_x(cx, cy, deg)
    local bx = cx - math.tan(math.rad(limit_value(deg, -85, 85))) * (cy - (-1.0))
    bx = limit_value(bx, -1.0, 1.0)
    return bx
end

BarBlock = {
    bars = {},           --{{cx, cy, deg, fill_rate, area}, ... }
    motions = {
        function(server) --FindMid 1
            print("往前走")
            server.move { x = 0.0, y = 0.1, z = 0.0, rot = 0.0 }
            sleep(0.1)
        end,
        function(server) --FindLeft 2
            print("向左寻找")
            server.move { x = 0.0, y = 0.0, z = 0.0, rot = -0.5 }
            sleep(0.1)
        end,
        function(server) --FindLeftBack 3
            print("归位")
            server.move { x = 0.0, y = 0.0, z = 0.0, rot = 0.5 }
            sleep(0.1)
        end,
        function(server) --FindRight 4
            print("向右寻找")
            server.move { x = 0.0, y = 0.0, z = 0.0, rot = 0.5 }
            sleep(0.1)
        end,
        function(server) --FindRightBack 5
            print("归位")
            server.move { x = 0.0, y = 0.0, z = 0.0, rot = -0.5 }
            sleep(0.1)
        end,
        function(bars) --Found 6
            if #bars >= 2 then
                if bars[1][2] > bars[2][2] then
                    bars[1], bars[2] = bars[2], bars[1]
                end
                local p1, p2 = cv.Point2d.new(), cv.Point2d.new()
                local deg2
                p1.x, p1.y = unpack(bars[1])
                p2.x, p2.y, deg2 = unpack(bars[2])
                if p1.y > 0.25 then
                    -- +-----------+
                    -- |       --- |
                    -- |     |     |
                    -- |     |     |
                    -- +-----------+
                    goto find_bar_1
                end
                -- +-----------+
                -- |           |
                -- |       --- |
                -- |     |     |
                -- +-----------+
                local deg = point_deg(p1, p2) - 90.0
                local bx = p1.x
                print("找到两个引导线 x  rot =", bx, deg)
                return function(server)
                    server.move { x = bx, y = 0.2, z = 0.0, rot = (deg / 90.0) / 2.0 }
                    sleep(0.1)
                end
            end

            :: find_bar_1 ::
            local cx, cy, deg = unpack(bars[1])
            local bx = bottom_x(cx, cy, deg)
            local by = 0.2
            if math.abs(deg) > 75.0 then
                -- +-----------+
                -- |           |
                -- |      ,_.-^|
                -- |     `     |
                -- +-----------+
                deg = 90.0 * cx
            elseif math.abs(cx) > 0.5 and math.abs(deg) > 30.0 and cx * deg < 0.0 then
                -- +-----------+
                -- |       \   |
                -- |        \  |
                -- |         \ |
                -- +-----------+
                deg = deg / (math.abs(cx) * 4.0)
                by = by / (math.abs(cx) * 4.0)
            end
            print("找到一个引导线  x  rot =", bx, deg)
            return function(server)
                server.move { x = bx, y = by, z = 0.0, rot = (deg / 90.0) / 2.0 }
                sleep(0.1)
            end
        end,
    }
}

--- 调用这个函数前先调用 update
--- @return function
local index = 1
local duration = 0.5
local time = 0.0
local motion = BarBlock.motions[1]
function BarBlock.process()
    if #BarBlock.bars > 0 then
        index = 0
        find_count = 1
        return BarBlock.motions[6](BarBlock.bars)
    end

    if index >= 5 then
        find_count = find_count + 1
        index = 0
    end

    if time >= duration then
        time = 0.0
        duration = 0.5 * find_count
        index, motion = next(BarBlock.motions, index)
    end
    time = time + 0.1
    return motion
end

function BarBlock.has_result()
    if next(BarBlock.bars) then
        return true
    else
        return false
    end
end

--- 更新表中bars的结果  有结果返回true  没有结果返回false
function BarBlock.update(raw_vision_bar_results)
    BarBlock.bars = {}
    for i = 1, #raw_vision_bar_results do
        local points = raw_vision_bar_results[i].points
        local area = raw_vision_bar_results[i].area
        local dist_p12 = point_dist(points[1], points[2])
        local dist_p23 = point_dist(points[2], points[3])
        local length, width = math.max(dist_p12, dist_p23) / 2.0, math.min(dist_p12, dist_p23) / 2.0
        if width < 1 / 15 then
            -- print("跳过过小的宽度")
            goto continue
        elseif length < 1 / 6 then
            -- print("跳过过小的长度")
            goto continue
        elseif length / width > 6 then
            -- print("跳过错误的长宽比")
            goto continue
        end
        local fill_rate = area / (length * width)
        local p1, p2
        if dist_p12 > dist_p23 then
            p1, p2 = points[1], points[2]
        else
            p1, p2 = points[2], points[3]
        end
        local deg = point_deg(p1, p2) - 90.0
        if math.abs(deg) > 89.0 then
            goto continue
        end
        local cx, cy = point_center(points[1], points[2], points[3], points[4])
        table.insert(BarBlock.bars, { cx, cy, deg, fill_rate, area })
        :: continue ::
    end

    table.sort(BarBlock.bars, function(a, b)
        return a[4] * a[5] > b[4] * b[5]
    end)

    if next(BarBlock.bars) then
        return true
    else
        return false
    end
end
