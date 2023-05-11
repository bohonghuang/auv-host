local json = require('json')
require("json.rpc")
require("application.lua.utils")
require("application.lua.bar_block")
require("application.lua.door_block")
require("application.lua.door_grid_block")
require("application.lua.detect_block")

-- local server = make_fake_server()
local server = json.rpc.proxy("http://localhost:8888")

local writer = UploadBlock.new(
    "appsrc ! videoconvert ! nvvidconv ! nvv4l2h264enc ! rtph264pay ! udpsink host=192.168.31.100 port=5600", 640, 360)
-- local writer = ImshowBlock.new()

local function update()
    local input = coroutine.yield()

    local bar_any = input["find_bar"]
    if bar_any then
        local find_bar = FindBarResults.from_any(bar_any)
        BarBlock.update(find_bar.result)
        -- writer:process(find_bar.frame)
    end

    local door_any = input["find_door"]
    if door_any then
        local find_line = FindDoorResults.from_any(door_any)
        DoorBlock.update(find_line.result)
        writer:process(find_line.frame)
    end

    -- local detect_any = input["detect"]
    -- if detect_any then
    --     object_detect = ObjectDetectResults.from_any(detect_any)
    --     --DetectBlock.update(object_detect.rkesult)
    --     --writer:process(object_detect.frame)
    -- end

    -- local door_grid_any = input["find_door_grid"]
    -- if door_grid_any then
    --     local find_bar_grid = FindDoorGridResults.from_any(door_grid_any)
    --     DoorGridBlock.update(find_bar_grid.mat)
    --     --print_mat(find_bar_grid.mat)
    --     -- writer:process(find_bar_grid.frame)
    -- end
end

local time = current_time()
function sleep(duration)
    time = current_time() + duration
    update()
end

local rush_time = 0.0
function main(input)
    while true do
        ::while_begin::
        update()

        local door_state, door_motion_fun = DoorBlock.process()
        if door_state == DoorState.None or door_state == DoorState.Rush then
            print("None   Rush")
            -- local bar_motion_fun = BarBlock.process()
            -- if bar_motion_fun then
            --     bar_motion_fun(server)
            --     DoorBlock.reset()
            --     rush_time = 0.0
            --     goto while_begin
            -- end
            if door_state == DoorState.Rush then
                print("Rush")
                if rush_time > 3 then
                    print("超时  重置!--------------")
                    DoorBlock.reset()
                    rush_time = 0.0
                    goto while_begin
                else
                    rush_time = rush_time + 0.1
                end
            end
            door_motion_fun(server)
        elseif door_state == DoorState.Lost then
            print("Lost")
            DoorBlock.reset()
            -- local bar_motion_fun = BarBlock.process()
            -- if bar_motion_fun then
            --     bar_motion_fun(server)
            --     DoorBlock.reset()
            --     goto while_begin
            -- else
            -- 
            -- end
        elseif door_state == DoorState.FindDoor then
            print("FindDoor")
            rush_time = 0.0
            door_motion_fun(server)
        end

        --detect_motion_fun = detect_block:process()
        --if detect_motion_fun then
        --    detect_motion_fun(server)
        --    goto while_begin
        --end
        sleep(0.1)
    end
end

local co = coroutine.create(main)
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
