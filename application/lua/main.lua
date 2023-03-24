json = require('json')
require("json.rpc")
server = json.rpc.proxy("http://localhost:8888")

cam_front = CameraBlock.new(GetCapture("0"))
cam_bottom = CameraBlock.new(GetCapture("0"))

cvtcolor_ycrcb = ConvertColorBlock.new(cv.COLOR_BGR2YCrCb)

find_bar_inrange_params = {
    low_1 = 33,
    high_1 = 177,
    low_2 = 146,
    high_2 = 255,
    low_3 = 65,
    high_3 = 130
}
find_bar_inrange = InRangeBlock.new(find_bar_inrange_params)
find_bar = FindBarBlock.new(false)
find_bar_block = LuaMuxBlock.new("application/lua/main_block.lua")

show = ImshowBlock.new()
bio = ObjectDetectBlock.new()

input_find_bar = connect(cam_bottom, cvtcolor_ycrcb, find_bar_inrange, find_bar, find_bar_block:input_block("find_bar"))
input_detect = connect(cam_front, bio, find_bar_block:input_block("detect"))

find_bar_task = SchedulerList.new(
        Scheduler.new(find_bar_block:as_untyped(), 1.0 / 15.0),
        Scheduler.new(input_find_bar:as_untyped(), 1.0 / 15.0),
        Scheduler.new(input_detect:as_untyped(), 1.0 / 15.0)
)


--find_bar_task:add(Scheduler.new(input_detect:as_untyped(), 1.0 / 15.0))

find_ball_inrange_params = {
    low_1 = 33,
    high_1 = 177,
    low_2 = 146,
    high_2 = 255,
    low_3 = 65,
    high_3 = 130
}

cvtcolor_hsv = ConvertColorBlock.new(cv.COLOR_BGR2HSV)
find_ball_inrange = InRangeBlock.new(find_ball_inrange_params)
find_ball = FindBallBlock.new(true)
find_ball_block = LuaMuxBlock.new("application/lua/ball_block.lua")

input_find_ball_front = connect(cam_bottom, cvtcolor_hsv, find_ball_inrange, find_ball, find_ball_block:input_block("front"))
input_find_ball_bottom = connect(cam_front, cvtcolor_hsv, find_ball_inrange, find_ball, find_ball_block:input_block("bottom"))

find_ball_task = SchedulerList.new(
        Scheduler.new(find_ball_block:as_untyped(), 1.0 / 15.0),
        Scheduler.new(input_find_ball_front:as_untyped(), 1.0 / 15.0),
        Scheduler.new(input_find_ball_bottom:as_untyped(), 1.0 / 15.0)
)

tasks = find_bar_task

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
