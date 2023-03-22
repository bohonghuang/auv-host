json = require('json')
require("json.rpc")
server = json.rpc.proxy("http://localhost:8888")

cam_front = CameraBlock.new(GetCapture("0"))
cam_bottom = CameraBlock.new(GetCapture("0"))

--calibr_params = CameraParams.new()
--calibr_params.fx = 588.4306598875787
--calibr_params.cx = 322.7472860229715
--calibr_params.fy = 592.781786987308
--calibr_params.cy = 242.4471017083893
--calibr_params.k1 = -0.1443039341764572
--calibr_params.k2 = 0.91856728920134
--calibr_params.k3 = 0.0
--calibr_params.k4 = 0.0
--calibr_params.k5 = -2.402839834767997
--calibr = CameraCalibrateBlock.new(calibr_params)

cvtcolor_ycrcb = ConvertColorBlock.new(cv.COLOR_BGR2YCrCb)
find_bar_inrange_params = InRangeParams.new()
find_bar_inrange_params.low_1 = 33
find_bar_inrange_params.high_1 = 177
find_bar_inrange_params.low_2 = 146
find_bar_inrange_params.high_2 = 255
find_bar_inrange_params.low_3 = 65
find_bar_inrange_params.high_3 = 130
find_bar_inrange = InRangeBlock.new(find_bar_inrange_params)
find_bar = FindBarBlock.new(false)
find_bar_block = LuaMuxBlock.new("application/lua/main_block.lua")

input_find_bar = connect(cam_bottom, cvtcolor_ycrcb, find_bar_inrange, find_bar, find_bar_block:input_block("find_bar"))
find_bar_task = SchedulerList.new(
        Scheduler.new(find_bar_block:as_untyped(), 1.0 / 15.0),
        Scheduler.new(input_find_bar:as_untyped(), 1.0 / 15.0)
)

bio = ObjectDetectBlock.new()
input_detect = connect(cam_front, bio, find_bar_block:input_block("detect"))
--find_bar_task:add(Scheduler.new(input_detect:as_untyped(), 1.0 / 15.0))

find_ball_inrange_params = InRangeParams.new()
find_ball_inrange_params.low_1 = 20
find_ball_inrange_params.low_2 = 20
find_ball_inrange_params.low_3 = 20
find_ball_inrange_params.high_1 = 199
find_ball_inrange_params.high_2 = 166
find_ball_inrange_params.high_3 = 178

cvtcolor_hsv   = ConvertColorBlock.new(cv.COLOR_BGR2HSV)
find_ball_inrange = InRangeBlock.new(find_ball_inrange_params)
find_ball = FindBallBlock.new(true)
find_ball_block = LuaMuxBlock.new("application/lua/ball_block.lua")

show = ImshowBlock.new()

input_find_ball_front  = connect(cam_bottom, cvtcolor_hsv, find_ball_inrange, find_ball, find_ball_block:input_block("front"))
input_find_ball_bottom = connect(cam_front, cvtcolor_hsv, find_ball_inrange, find_ball, find_ball_block:input_block("bottom"))

find_ball_task = SchedulerList.new(
        Scheduler.new(find_ball_block:as_untyped(), 1.0 / 15.0),
        Scheduler.new(input_find_ball_front:as_untyped(), 1.0 / 15.0),
        Scheduler.new(input_find_ball_bottom:as_untyped(), 1.0 / 15.0)
)

tasks = find_ball_task

server.move { x = 0.0, y = 0.0, z = 0.0, rot = 0.0 }
server.set_depth_locked { false }

function start_all()
    server.move { x = 0.0, y = 0.0, z = 0.0, rot = 0.0 }
    server.set_depth_locked { true }
    tasks:start()
end

function stop_all()
    server.move { x = 0.0, y = 0.0, z = 0.0, rot = 0.0 }
    server.set_depth_locked { false }
    tasks:stop()
end

function pause_all()
    server.move { x = 0.0, y = 0.0, z = 0.0, rot = 0.0 }
    server.set_depth_locked { false }
    tasks:pause()
end

function resume_all()
    server.move { x = 0.0, y = 0.0, z = 0.0, rot = 0.0 }
    server.set_depth_locked { true }
    tasks:resume()
end
