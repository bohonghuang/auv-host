local json = require('json')
require("json.rpc")
require("application.lua.utils")

local time = current_time()
local server = json.rpc.proxy("http://localhost:8888")
local writer = UploadBlock.new("appsrc ! videoconvert ! nvvidconv ! nvv4l2h264enc ! rtph264pay ! udpsink host=192.168.31.100 port=5600", 640, 480)
-- local writer = UploadBlock.new("appsrc ! videoconvert ! nvh264enc ! rtph264pay ! udpsink host=192.168.31.200 port=5600", 640, 480)

--local function cpp_sequence_table(seq)
--    local results = {}
--    for i = 1, #seq do
--        table.insert(results, seq[i])
--    end
--    return results
--end

local function ball_filter(bars)
    local results = {}
    for i = 1, #bars do
        local points = bars[i].points
        local dist_p12 = point_dist(points[1], points[2])
        local dist_p23 = point_dist(points[2], points[3])

        if dist_p12 < 0.05 or dist_p23 < 0.05 then
            goto continue
        end

        local cx, cy = point_center(points[1], points[2], points[3], points[4])
        local area = dist_p12 * dist_p23
        local p1 = { x = 0.0, y = -1.0 }
        local p2 = { x = cx, y = cy }
        local deg = point_deg(p1, p2) - 90.0

        table.insert(results, { cx = cx, cy = cy, deg = deg, area = area })
        :: continue ::
    end
    table.sort(results, function(a, b)
        return a.area > b.area
    end)
    return results
end

function main(input)
    local front_vision_result, bottom_vision_result = { result = {} }, { result = {} }
    local function update()
        input = coroutine.yield()
        local front_ball_any, bottom_ball_any = input["front"], input["bottom"]
        if front_ball_any then
            front_vision_result = FindBallResults.from_any(front_ball_any)
            writer:process(front_vision_result.frame)
        end
        if bottom_ball_any then
            bottom_vision_result = FindBallResults.from_any(bottom_ball_any)
        end
    end
    local function sleep(duration)
        time = current_time() + duration
        update()
    end

    -- server.move({x = 0.0, y = 0.0, z = -0.5, rot = 0.0 })
    -- sleep(3)
    server.catch { 1.0 }
    local sink_acc = -0.45
    while true do
        local function find_left_right(update_function)
            local function try_motion(motion, duration)
                local delta = 0.1
                while true do
                    server.move(motion)
                    sleep(delta)
                    local update_result = update_function()
                    if type(update_result) == 'table' then
                        if table_size(update_result) > 0 then
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
            try_motion({ x = 0.0, y = 0.5, z = sink_acc, rot = 0.0 }, 3.0)
            local rotation = 0.5
            for duration = 1, 3 do
                if try_motion({ x = 0.0, y = 0.0, z = sink_acc, rot = rotation }, duration / 2.0) or
                        try_motion({ x = 0.0, y = 0.0, z = sink_acc, rot = -rotation }, duration) or
                        try_motion({ x = 0.0, y = 0.0, z = sink_acc, rot = rotation }, duration / 2.0) then
                    return true
                end
            end
            return false
        end
        local function find_ball()
            local front_balls, bottom_balls = ball_filter(front_vision_result.result), ball_filter(bottom_vision_result.result)
            --if #bottom_balls > 0 then
            --    if bottom_balls[1].cy < 0.6 then
            --        print("下摄像头：直行......")
            --        server.move{ x = 0.0, y = 1.0, z = sink_acc, rot = 0.0 }
            --        sleep(1.5)
            --        server.catch{-1.0}
            --        server.move{ x = 0.0, y = 0.0, z = 0.0, rot = 0.0 }
            --        error("break while(true)")
            --    end
            --end

            if #front_balls > 0 then
                if front_balls[1].cy > -0.3 then
                    if front_balls[1].deg < 85 then
                        print("正在直行 微调")
                        print(front_balls[1].deg)
                        return { x = 0.0, y = 0.36, z = sink_acc, rot = front_balls[1].deg / 90.0 / 2.0 }
                    else
                        print("偏离角度过大，减小速度")
                        print(front_balls[1].deg)
                        return { x = 0.0, y = 0.15, z = sink_acc, rot = front_balls[1].deg / 90.0 / 2.0 }
                    end
                else
                    print("前摄像头：直行......")
                    return { x = 0.0, y = 0.36, z = sink_acc, rot = 0.0 }
                end
            end
            return {}
        end

        local function block_main()
            local motion = find_ball()
            while true do
                local res = find_left_right(function()
                    motion = find_ball()
                    return motion
                end)
                if res then
                    break
                end
            end
            server.move(motion)
            sleep(0.1)
        end

        local success, _ = pcall(block_main)
        if not success then
            break
        end
    end
    sleep(999999)
end

co = coroutine.create(main)

function process(input)
    if time <= current_time() then
        local result, err = coroutine.resume(co, input)
        if not result then
            print(err)
            local tasks = SchedulerList.from_any(input["scheduler"])
            tasks:stop()
        end
    end
    return void:to_any()
end
