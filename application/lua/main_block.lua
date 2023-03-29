local json = require('json')
require("json.rpc")
require("application.lua.utils")
require("application.lua.bar_block")

local time = current_time()
local server = json.rpc.proxy("http://localhost:8888")
--local writer = UploadBlock.new("appsrc ! videoconvert ! nvvidconv ! nvv4l2h264enc ! rtph264pay ! udpsink host=192.168.31.143 port=5600", 640, 480)
--local server = make_fake_server()
local writer = ImshowBlock.new()

bar_block = BarBlock.new()

local function update(input)
    input = coroutine.yield()

    local bar_any = input["find_bar"]
    if bar_any then
        find_bar = FindBarResults.from_any(bar_any)
        bar_block:update(find_bar.result)
        writer:process(find_bar.frame)
    end

    --local detect_any = input["detect"]
    --if detect_any then
    --    object_detect = ObjectDetectResults.from_any(detect_any)
    --    detect = object_detect.result
    --    --writer:process(object_detect.frame)
    --end
end

local function sleep(duration)
    time = current_time() + duration
    update()
end

function main(input)
    update(input)

    local detectables = { shark = false,
                          squid = false,
                          coralreef = true,
                          turtle = true,
                          dolphin = true, }
    local tracks = {}
    while true do
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

        local function update_tracks()
            tracks = {}
            for i = 1, #detect do
                if detectables[detect[i].name] then
                    table.insert(tracks, detect[i])
                end
            end
        end
        local function update_bars()
            bars = bars_filter(boxes)
        end
        local function track_object()
            local prev_track_name = tracks[1].name
            local really_detected = false
            local really_hit = false
            local timer = 0
            while really_detected and not really_hit or tracks[1] and tracks[1].name == prev_track_name do
                if really_detected then
                    if tracks[1] and tracks[1].name == prev_track_name then
                        print("正在撞向生物" .. prev_track_name)
                        timer = 0.0
                        local track = tracks[1]
                        local rect = track
                        local x, y, w, h = rect.x, rect.y, rect.width, rect.height
                        cx, cy = point_center({ x = x, y = y }, { x = x + w, y = y + h })
                        server.move { x = 0.0, y = 0.2, z = 0.0, rot = cx / 2.0 }
                    else
                        print("疑似撞到生物 " .. prev_track_name .. " ，已等待 " .. timer .. " 秒")
                        timer = timer + 0.1
                        if timer > 5.0 then
                            print("已撞到生物 " .. prev_track_name)
                            really_hit = true
                            timer = 0.0
                            detectables[prev_track_name] = false
                            find_left_right(update_results, true)
                            return
                        end
                        server.move { x = 0.0, y = 0.2, z = 0.0, rot = 0.0 }
                    end
                else
                    print("疑似检测到生物 " .. prev_track_name .. " ，已等待 " .. timer .. " 秒")
                    server.move { x = 0.0, y = 0.0, z = 0.0, rot = 0.0 }
                    timer = timer + 0.1
                    if timer > 0.5 then
                        print("开始撞向生物" .. prev_track_name)
                        really_detected = true
                        timer = 0.0
                    end
                end
                update_tracks()
                sleep(0.1)
            end
        end
        update_results = function()
            local results = {}
            update_tracks()
            if table_size(tracks) > 0 then
                table.insert(results, track_object) -- 检测优先
            end
            table.sort(tracks, function(a, b)
                return a.confidence > b.confidence
            end)

            update_bars()
            if #bars > 0 then
                table.insert(results, find_bar)
            end
            return results
        end
        local results = update_results()
        if #results == 0 then
            find_left_right(update_results)
        else
            for _, result in pairs(results) do
                result()
                break
            end
        end
        sleep(0.1)
    end
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