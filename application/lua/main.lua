cam = CameraBlock.new(GetCapture(0))

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
input_find_bar = connect(cam, calibr, cvtcolor, inrange, find_bar, mux_block:input_block("find_bar"))
input_detect = connect(cam, calibr, bio, mux_block:input_block("detect"))

find_bar_task = Scheduler.new(input_find_bar:as_untyped(), 1.0 / 15.0)
detect_task = Scheduler.new(input_detect:as_untyped(), 1.0 / 15.0)
main_task = Scheduler.new(mux_block:as_untyped(), 1.0 / 15.0)

tasks = SchedulerList.new(find_bar_task, main_task, detect_task)

function start_all()
   tasks:start()
end

function stop_all()
   tasks:stop()
end
