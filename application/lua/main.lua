local json = require("json")
require("json.rpc")
require("application.lua.utils")
local server = json.rpc.proxy("http://localhost:8888")

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

local find_bar_inrange_params = {
	low_1 = 53,
	low_2 = 132,
	low_3 = 103,
	high_1 = 151,
	high_2 = 189,
	high_3 = 146,
}

local find_door_inrange_params = {
	low_1 = 38,
	low_2 = 132,
	low_3 = 95,
	high_1 = 136,
	high_2 = 193,
	high_3 = 134,
}

local register_bar_and_door_block_list = {
	-- { "cam_front", block = {
	-- 	"CameraBlock",
	-- 	args = function()
	-- 		return GetCapture("door6.tmp")
	-- 	end,
	-- } },
	{
		"cam_front",
		block = {
			"CameraBlock",
			args = function()
				return GetCapture("0", 640, 360)
			end,
		},
	},
	{
		"cam_bottom",
		block = {
			"CameraBlock",
			args = function()
				-- return GetCapture("0", 640, 480)
				return GetCapture("0", 640, 360)
			end,
		},
	},
	{ "cam_front_calibr", block = { "CameraCalibrateBlock", args = { camera_calibr_params } } },
	{ "cvtcolor_ycrcb", block = { "ConvertColorBlock", args = { cv.COLOR_BGR2YCrCb } } },
	--- 引导线相关视觉算法
	{ "find_bar_inrange", block = { "InRangeBlock", args = { find_bar_inrange_params } } },
	{ "find_bar", block = { "FindBarBlock", args = { true } } },
	--- 门相关视觉算法
	{ "find_door_inrange", block = { "InRangeBlock", args = { find_door_inrange_params } } },
	{ "find_door", block = { "FindLineBlock", args = { 4, math.pi / 60, 200, true } } },
	--- 逻辑代码循环
	{ "main_block", block = { "LuaMuxBlock", args = { "application/lua/main_block.lua" } } },
}
local bar_and_door_block_list = register_block(register_bar_and_door_block_list)

local find_bar_sequence = {
	"cam_bottom",
	"cam_front_calibr",
	"cvtcolor_ycrcb",
	"find_bar_inrange",
	"find_bar",
	{ "main_block", input = "find_bar" },
}
local find_door_sequence = {
	"cam_front",
	"cam_front_calibr",
	"cvtcolor_ycrcb",
	"find_door_inrange",
	"find_door",
	{ "main_block", input = "find_door" },
}
local find_bar = connect_block(bar_and_door_block_list, find_bar_sequence)
local find_door = connect_block(bar_and_door_block_list, find_door_sequence)

local main_task_group = create_task_group({
	{ bar_and_door_block_list.main_block, fps = 15.0 },
	{ find_bar, fps = 15.0 },
	{ find_door, fps = 15.0 },
})

local tasks = main_task_group

server.move({ x = 0.0, y = 0.0, z = 0.0, rot = 0.0 })
server.set_depth_locked({ false })
server.set_direction_locked({ false })

function start_all()
	server.move({ x = 0.0, y = 0.0, z = 0.0, rot = 0.0 })
	-- server.set_depth_locked { true } ------------------------------------------------------------------------------------------------------
	-- server.set_direction_locked { true }
	tasks:start()
end

function stop_all()
	server.move({ x = 0.0, y = 0.0, z = 0.0, rot = 0.0 })
	server.set_depth_locked({ false })
	server.set_direction_locked({ false })
	tasks:stop()
end

function pause_all()
	server.move({ x = 0.0, y = 0.0, z = 0.0, rot = 0.0 })
	server.set_depth_locked({ false })
	server.set_direction_locked({ false })
	tasks:pause()
end

function resume_all()
	server.move({ x = 0.0, y = 0.0, z = 0.0, rot = 0.0 })
	server.set_depth_locked({ true })
	server.set_direction_locked({ true })
	tasks:resume()
end
