
function point_dist(p1, p2)
    local x1, y1 = p1.x, p1.y
    local x2, y2 = p2.x, p2.y
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

function point_deg(p1, p2)
    local x1, y1 = p1.x, p1.y
    local x2, y2 = p2.x, p2.y
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
