require("math")

function register_block(_register_block_list)
	local M = {}
	for i = 1, #_register_block_list do
		local compent_args = _register_block_list[i]
		local key = compent_args[1]
		local block = compent_args.block
		local block_name = block[1]
		if not _G[block_name] then
			print(block_name .. " is not exist.")
			goto continue
		end
		if type(block.args) == "function" then
			M[key] = _G[block[1]].new(block.args())
		else
			status, result, msg = pcall(_G[block[1]].new, unpack(block.args))
			if status then
				M[key] = result
			else
				print(key, msg)
			end
		end
		::continue::
	end
	return setmetatable(M, {
		__index = function(_, key)
			error("could not find the `" .. key .. "` block!!!!")
		end,
	})
end

function connect_block(block_list, sequence)
	local temp = {}
	for i, v in ipairs(sequence) do
		if type(v) == "table" then
			v = v[1]
		end
		local block = block_list[v]
		if not block then
			error("could not find the `" .. v .. "` block!!!!")
		end
		if i == #sequence then
			local input_key = sequence[i].input
			if not input_key then
				error("could not find input field in the block")
			end
			temp[i] = block:input_block(input_key)
		else
			temp[i] = block
		end
	end
	return connect(unpack(temp))
end

function create_task_group(task_list)
	local tasks = SchedulerList.new()
	for _, v in ipairs(task_list) do
		local task, fps = v[1], v.fps
		if not task then
			error("could not find the task")
		end
		if not fps then
			error("could not find the fps field in the table")
		end
		tasks:add(Scheduler.new(task:as_untyped(), 1.0 / fps))
	end
	return tasks
end

local function find_left_right(update_function, backward)
	local function try_motion(motion, duration)
		local delta = 0.1
		while true do
			server.move(motion)
			sleep(delta)
			local update_result = update_function()
			if type(update_result) == "table" then
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
		if
			try_motion({ x = 0.0, y = 0.0, z = 0.0, rot = rotation }, duration / 2.0)
			or try_motion({ x = 0.0, y = 0.0, z = 0.0, rot = -rotation }, duration)
			or try_motion({ x = 0.0, y = 0.0, z = 0.0, rot = rotation }, duration / 2.0)
		then
			return true
		end
	end
	return false
end

function point_dist(point_1, point_2)
	local x1, y1 = point_1.x, point_1.y
	local x2, y2 = point_2.x, point_2.y
	return ((x1 - x2) ^ 2 + (y1 - y2) ^ 2) ^ 0.5
end

function point_center(...)
	local points = { ... }
	local cx, cy = 0.0, 0.0
	for _, point in pairs(points) do
		cx = cx + point.x
		cy = cy + point.y
	end
	return cx / #points, cy / #points
end

function point_deg(point_1, point_2)
	local x1, y1 = point_1.x, point_1.y
	local x2, y2 = point_2.x, point_2.y
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

function is_in_range(value, min, max)
	if value > min and value < max then
		return true
	else
		return false
	end
end

function table_size(tbl)
	local cnt = 0
	for _, _ in pairs(tbl) do
		cnt = cnt + 1
	end
	return cnt
end

function make_fake_server()
	local tbl = {}
	local meta = {
		__index = function(self, key)
			return function(x)
				local function dump(o)
					if type(o) ~= "table" then
						return tostring(o)
					end
					local s = "{ "
					for k, v in pairs(o) do
						if type(k) ~= "number" then
							k = '"' .. k .. '"'
						end
						s = s .. "[" .. k .. "] = " .. dump(v) .. ","
					end
					return s .. "} "
				end
				print(key, dump(x))
			end
		end,
	}
	setmetatable(tbl, meta)
	return tbl
end

function print_mat(mat)
	local str = ""
	for i = 1, #mat do
		local row = mat[i]
		for j = 1, #row do
			local value = row[j]
			str = str .. (value - value % 0.01) .. ","
		end
		str = str .. "\n"
	end
	print(str)
end

function print_table(tbl)
	for k, v in pairs(tbl) do
		print(k, v)
	end
end

function print_bool_mat(mat, rows, cols)
	local str = ""
	for i = 1, rows do
		local row = mat[i]
		for j = 1, cols do
			local value = row[j]
			if value then
				str = str .. 1 .. ","
			else
				str = str .. 0 .. ","
			end
		end
		str = str .. "\n"
	end
	print(str)
end
