require("math")

local function find_left_right(update_function, backward)
local function try_motion(motion, duration)
    local delta = 0.1
    while true do
        server.move(motion)
        sleep(delta)
        local update_result = update_function()
        if type(update_result) == 'table' then
            if #update_result > 0 then
                return true
            end
        elseif update_result then
            return true
        end
        duration = duration - delta
        if duration <= 0.0 then
            return false
        end
    end
end
try_motion({ x = 0.0, y = backward and -0.5 or 0.5, z = 0.0, rot = 0.0 }, 10.0)
local rotation = 0.5
for duration = 1, 3 do
    if try_motion({ x = 0.0, y = 0.0, z = 0.0, rot = rotation }, duration / 2.0) or
            try_motion({ x = 0.0, y = 0.0, z = 0.0, rot = -rotation }, duration) or
            try_motion({ x = 0.0, y = 0.0, z = 0.0, rot = rotation }, duration / 2.0) then
        return true
    end
end
return false
end

function point_dist(point_1, point_2)
    local x1, y1 = point_1.x, point_1.y
    local x2, y2 = point_2.x, point_2.y
    return ((x1 - x2) ^ 2 + (y1 - y2) ^ 2) ^ 0.5;
end

function point_center(...)
    local points = { ... };
    local cx, cy = 0.0, 0.0
    for _, point in pairs(points) do
        cx = cx + point.x
        cy = cy + point.y
    end
    return cx / #points, cy / #points
end

function point_deg(point_1, point_2)
    local x1, y1 = point_1.x, point_1.y
    local x2, y2 = point_2.x, point_2.y
    local dx, dy = x2 - x1, y2 - y1
    if dx == 0.0 then
        return 90.0
    else
        local deg = math.deg(math.atan(dy / dx))
        if deg <= 0.0 then
            return -deg
        else
            return 180.0 - deg
        end
    end
end

function limit_value(value, min, max)
    return math.min(math.max(min, value), max)
end

function is_in_range(value, min, max)
    if value > min and value < max then
        return true
    else
        return false
    end
end

function table_size(tbl)
    local cnt = 0
    for _, _ in pairs(tbl) do
        cnt = cnt + 1
    end
    return cnt
end

function make_fake_server()
    local tbl = {}
    local meta = {
        __index = function(self, key)
            return function(x)
                local function dump(o)
                    if type(o) ~= 'table' then
                        return tostring(o)
                    end
                    local s = '{ '
                    for k, v in pairs(o) do
                        if type(k) ~= 'number' then
                            k = '"' .. k .. '"'
                        end
                        s = s .. '[' .. k .. '] = ' .. dump(v) .. ','
                    end
                    return s .. '} '
                end
                print(key, dump(x))
            end
        end,
    }
    setmetatable(tbl, meta)
    return tbl
end