json = require('json')
require("json.rpc")
server = json.rpc.proxy("http://localhost:8888")

cam = CameraBlock.new(GetCapture("2"))

calibr_params = CameraParams.new()
calibr_params.fx = 588.4306598875787
calibr_params.cx = 322.7472860229715
calibr_params.fy = 592.781786987308
calibr_params.cy = 242.4471017083893
calibr_params.k1 = -0.1443039341764572
calibr_params.k2 = 0.91856728920134
calibr_params.k3 = 0.0
calibr_params.k4 = 0.0
calibr_params.k5 = -2.402839834767997
calibr = CameraCalibrateBlock.new(calibr_params)

cvtcolor = ConvertColorBlock.new(cv.COLOR_BGR2YCrCb)

inrange_params = InRangeParams.new()
inrange_params.low_1 = 33
inrange_params.high_1 = 177
inrange_params.low_2 = 146
inrange_params.high_2 = 255
inrange_params.low_3 = 65
inrange_params.high_3 = 130
inrange = InRangeBlock.new(inrange_params)

find_bar = FindBarBlock.new(false)

-- show = ImshowBlock.new()
bio = ObjectDetectBlock.new()

-- writer = UploadBlock.new("appsrc ! videoconvert ! nvvidconv ! nvv4l2h264enc ! rtph264pay ! udpsink host=192.168.31.100 port=5600", 640, 480)

mux_block = LuaMuxBlock.new("application/lua/main_block.lua")
input_find_bar = connect(cam, cvtcolor, inrange, find_bar, mux_block:input_block("find_bar"))
input_detect = connect(cam, bio, mux_block:input_block("detect"))

find_bar_task = Scheduler.new(input_find_bar:as_untyped(), 1.0 / 15.0)
detect_task = Scheduler.new(input_detect:as_untyped(), 1.0 / 15.0)
main_task = Scheduler.new(mux_block:as_untyped(), 1.0 / 15.0)

tasks = SchedulerList.new(find_bar_task, main_task)

server.move{x=0.0, y=0.0, z=0.0, rot=0.0}
server.set_depth_locked{false}

function start_all()
   server.move{x=0.0, y=0.0, z=0.0, rot=0.0}
   -- server.set_depth_locked(false)
   -- sleep(1.0)
   -- print("深度锁定 3")
   -- sleep(1.0)
   -- print("深度锁定 2")
   -- sleep(1.0)
   -- print("深度锁定 1")
   -- sleep(1.0)
   server.set_depth_locked{true}
   tasks:start()
end

function stop_all()
   server.move{x=0.0, y=0.0, z=0.0, rot=0.0}
   server.set_depth_locked{false}
   tasks:stop()
end

function pause_all()
   server.move{x=0.0, y=0.0, z=0.0, rot=0.0}
   server.set_depth_locked{false}
   tasks:pause()
end

function resume_all()
   server.move{x=0.0, y=0.0, z=0.0, rot=0.0}
   server.set_depth_locked{true}
   tasks:resume()
end
