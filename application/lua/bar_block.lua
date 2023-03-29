require "utils"
require "math"

BarBlock = {
    bars = {}, --{{cx, cy, deg, fill_rate, area}, ... }
    motions = {
        FindLeft = function(this)
            return { x = 0.0, y = 0.0, z = 0.0, rot = -0.5 }
        end,
        FindLeftBack = function(this)
            return { x = 0.0, y = 0.0, z = 0.0, rot = 0.5 }
        end,
        FindRight = function(this)
            return { x = 0.0, y = 0.0, z = 0.0, rot = 0.5 }
        end,
        FindRightBack = function(this)
            return { x = 0.0, y = 0.0, z = 0.0, rot = -0.5 }
        end,
        Found = function(this)
            if #this.bars >= 2 then
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
                print(2, bx, deg)
                return { x = bx, y = 0.2, z = 0.0, rot = (deg / 90.0) / 2.0 }
            end

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
            print(1, bx, deg)
            return { x = bx, y = by, z = 0.0, rot = (deg / 90.0) / 2.0 }
        end,
    }
}

--- 返回当前执行的运动类型和运动参数
function BarBlock:process(this)
    local status = "None"

    if #this.bars > 0 then
        return "Found", BarBlock.motions.Found(this)
    end


end

function BarBlock:new(this)
    temp_table = this or {}
    this.bars = {}
    this.status = BarBlock.status
    return temp_table
end

function BarBlock:has_result(this)
    if next(this.bars) then
        return true
    else
        return false
    end
end

local function bottom_x(cx, cy, deg)
    local bx = cx - math.tan(math.rad(limit_value(deg, -85, 85))) * (cy - (-1.0))
    bx = limit_value(bx, -1.0, 1.0)
    return bx
end

--- 更新表中bars的结果  有结果返回true  没有结果返回false
function BarBlock:update(this, raw_vision_bar_results)
    this.bars = {}
    for i = 1, #raw_vision_bar_results do
        local points = raw_vision_bar_results[i].points
        local area = raw_vision_bar_results[i].area
        local dist_p12 = point_dist(points[1], points[2])
        local dist_p23 = point_dist(points[2], points[3])
        local length, width = math.max(dist_p12, dist_p23) / 2.0, math.min(dist_p12, dist_p23) / 2.0
        if width < 1 / 15 then
            print("跳过过小的宽度")
            goto continue
        elseif length < 1 / 6 then
            print("跳过过小的长度")
            goto continue
        elseif length / width > 6 then
            print("跳过错误的长宽比")
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
        table.insert(this.bars, { cx, cy, deg, fill_rate, area })
        :: continue ::
    end

    table.sort(this.bars, function(a, b)
        return a[4] * a[5] > b[4] * b[5]
    end)

    if next(this.bars) then
        return true
    else
        return false
    end
end

