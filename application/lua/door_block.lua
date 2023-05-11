require("application.lua.utils")
require("math")

DoorState = {
    None = 0,
    FindDoor = 1,
    Rush = 2,
    Lost = 3,
}

DoorBlock = {
    left = {},
    right = {},
    bottom = {},

    state = DoorState.None
}

function DoorBlock.reset()
    DoorBlock.state = DoorState.None
end

local function is_point_val(tbl)
    if tbl.p1 and table_size(tbl.p1) ~= 0 and table_size(tbl.p2) ~= 0 then
        return true
    end
    return false
end

function DoorBlock.process()
    local rot = 0.0
    local x, y = 0.0, 0.0

    local left, right, bottom = DoorBlock.left, DoorBlock.right, DoorBlock.bottom
    if DoorBlock.state == DoorState.None or DoorBlock.state == DoorState.FindDoor or DoorBlock.state == DoorState.Lost then
        if is_point_val(left) and is_point_val(right) then
            DoorBlock.state = DoorState.FindDoor
        end

        if is_point_val(bottom) then
            local k = (bottom.p1.y - bottom.p2.y) / (bottom.p1.x - bottom.p2.x)
            rot = -k * 1.0
        end

        if is_point_val(left) and is_point_val(right) then
            local left_x = (left.p1.x + left.p2.x) / 2
            local right_x = (right.p1.x + right.p2.x) / 2
            x = left_x + right_x
        else
            DoorBlock.state = DoorState.Lost
            -- if is_point_val(left) then
            --     x = 1.0
            -- end
            -- if is_point_val(right) then
            --     x = -1.0
            -- end
        end

        if DoorBlock.state == DoorState.FindDoor and math.abs(x) < 0.06 and math.abs(rot) < 0.20 then
            y = 0.5
            DoorBlock.state = DoorState.Rush
        end
    elseif DoorBlock.state == DoorState.Lost then

    elseif DoorBlock.state == DoorState.Rush then
        if not is_point_val(left) or not is_point_val(right) then
            DoorBlock.state = DoorState.Lost
        else
            if is_point_val(bottom) then
                local k = (bottom.p1.y - bottom.p2.y) / (bottom.p1.x - bottom.p2.x)
                rot = -k * 0.8
            end
            if is_point_val(left) and is_point_val(right) then
                local left_x = (left.p1.x + left.p2.x) / 2
                local right_x = (right.p1.x + right.p2.x) / 2
                x = (left_x + right_x) * 0.8
            end
            y = 0.2
        end
    end

    local result_func = function(server)
        server.move { x = x * 0.3, y = y, z = 0, rot = rot * 0.5 }
        print("x,y,rot:", x, y, rot)
        sleep(0.1)
    end
    return DoorBlock.state, result_func
end

local function raw22point(raw_point_val)
    local point1, point2 = {}, {}
    if raw_point_val[1] ~= 0 and raw_point_val[2] ~= 0 then
        point1.x = raw_point_val[1]
        point1.y = raw_point_val[2]
    end

    if raw_point_val[3] ~= 0 and raw_point_val[4] ~= 0 then
        point2.x = raw_point_val[3]
        point2.y = raw_point_val[4]
    end
    return { p1 = point1, p2 = point2 }
end

function DoorBlock.update(raw_vision_line_results)
    DoorBlock.left = raw22point(raw_vision_line_results.left)
    DoorBlock.right = raw22point(raw_vision_line_results.right)
    DoorBlock.bottom = raw22point(raw_vision_line_results.bottom)
    -- if is_point_val(DoorBlock.bottom) then
    --     print("left.point1", DoorBlock.bottom.p1.x, DoorBlock.bottom.p1.y)
    --     print("left.point2", DoorBlock.bottom.p2.x, DoorBlock.bottom.p2.y)
    -- end
    -- print("left.point1", left.p1.x, left.p1.y)
    -- print("left.point2", left.p2.x, left.p2.y)
    -- print("right.point1", right.p1.x, right.p1.y)
    -- print("right.point2", right.p2.x, right.p2.y)
end
