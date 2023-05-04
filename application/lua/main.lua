local json = require('json')
require("json.rpc")
require("application.lua.utils")
local server = json.rpc.proxy("http://localhost:8888")

-- local cam_front = CameraBlock.new(GetCapture("door6.tmp"))
local cam_front = CameraBlock.new(GetCapture("0", 640, 360))
local cam_bottom = CameraBlock.new(GetCapture("0", 640, 360))

local camera_calibr_params = {
    fx = 354.9862468599325,
    cx = 324.5669837164503,
    fy = 354.8966706449905,
    cy = 192.7918851368834,
    k1 = -0.4797210886746362,
    k2 = 0.2124696026438813,
    k3 = 0.001906119577026161,
    k4 = 0.001051512267817575,
    k5 = -0.03890132615729874,
}

local cam_front_calibr = CameraCalibrateBlock.new(camera_calibr_params)
local cvtcolor_ycrcb = ConvertColorBlock.new(cv.COLOR_BGR2YCrCb)

local find_bar_inrange_params = {
    low_1 = 17,
    low_2 = 145,
    low_3 = 92,
    high_1 = 107,
    high_2 = 188,
    high_3 = 134

}
local find_bar_inrange = InRangeBlock.new(find_bar_inrange_params)
local find_bar = FindBarBlock.new(true)
local find_bar_block = LuaMuxBlock.new("application/lua/main_block.lua")

local find_door = FindLineBlock.new(1, math.pi / 180, 200)
local find_door_grid = FindDoorGrid.new(6, 8)

local show = ImshowBlock.new()
-- local bio = ObjectDetectBlock.new()

local input_find_bar = connect(cam_bottom, cvtcolor_ycrcb, find_bar_inrange, find_bar,
    find_bar_block:input_block("find_bar"))
local input_find_door = connect(cam_front, cam_front_calibr, cvtcolor_ycrcb, find_bar_inrange, find_door,
    find_bar_block:input_block("find_door"))
local input_find_door_grid = connect(cam_front, cam_front_calibr, cvtcolor_ycrcb, find_bar_inrange, find_door_grid,
    find_bar_block:input_block("find_door_grid"))
-- local input_detect = connect(cam_front, bio, find_bar_block:input_block("detect"))

local find_bar_task = SchedulerList.new(
    Scheduler.new(find_bar_block:as_untyped(), 1.0 / 15.0),
    Scheduler.new(input_find_bar:as_untyped(), 1.0 / 15.0),
    Scheduler.new(input_find_door:as_untyped(), 1.0 / 15.0),
    Scheduler.new(input_find_door_grid:as_untyped(), 1.0 / 15.0)
)
-- find_bar_task:add(Scheduler.new(input_detect:as_untyped(), 1.0 / 15.0))

-- local find_ball_inrange_params = {
--     low_1 = 33,
--     high_1 = 177,
--     low_2 = 146,
--     high_2 = 255,
--     low_3 = 65,
--     high_3 = 130
-- }

-- local cvtcolor_hsv = ConvertColorBlock.new(cv.COLOR_BGR2HSV)
-- local find_ball_inrange = InRangeBlock.new(find_ball_inrange_params)
-- local find_ball = FindBallBlock.new(true)
-- local find_ball_block = LuaMuxBlock.new("application/lua/ball_block.lua")

-- local input_find_ball_front = connect(cam_bottom, cvtcolor_hsv, find_ball_inrange, find_ball, find_ball_block:input_block("front"))
-- local input_find_ball_bottom = connect(cam_front, cvtcolor_hsv, find_ball_inrange, find_ball, find_ball_block:input_block("bottom"))

-- local find_ball_task = SchedulerList.new(
--         Scheduler.new(find_ball_block:as_untyped(), 1.0 / 15.0),
--         Scheduler.new(input_find_ball_front:as_untyped(), 1.0 / 15.0),
--         Scheduler.new(input_find_ball_bottom:as_untyped(), 1.0 / 15.0)
-- )

local tasks = find_bar_task

server.move { x = 0.0, y = 0.0, z = 0.0, rot = 0.0 }
server.set_depth_locked { false }
server.set_direction_locked { false }

function start_all()
    server.move { x = 0.0, y = 0.0, z = 0.0, rot = 0.0 }
    server.set_depth_locked { true }
    server.set_direction_locked { true }
    tasks:start()
end

function stop_all()
    server.move { x = 0.0, y = 0.0, z = 0.0, rot = 0.0 }
    server.set_depth_locked { false }
    server.set_direction_locked { false }
    tasks:stop()
end

function pause_all()
    server.move { x = 0.0, y = 0.0, z = 0.0, rot = 0.0 }
    server.set_depth_locked { false }
    server.set_direction_locked { false }
    tasks:pause()
end

function resume_all()
    server.move { x = 0.0, y = 0.0, z = 0.0, rot = 0.0 }
    server.set_depth_locked { true }
    server.set_direction_locked { true }
    tasks:resume()
end
