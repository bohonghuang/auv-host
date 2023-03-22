time = current_time()
json = require('json')
require("json.rpc")
server = json.rpc.proxy("http://localhost:8888")
writer = UploadBlock.new("appsrc ! videoconvert ! nvvidconv ! nvv4l2h264enc ! rtph264pay ! udpsink host=192.168.31.200 port=5600", 640, 480)
-- writer = UploadBlock.new("appsrc ! videoconvert ! nvh264enc ! rtph264pay ! udpsink host=192.168.31.200 port=5600", 640, 480)

local function point_dist(p1, p2)
   local x1, y1 = p1.x, p1.y
   local x2, y2 = p2.x, p2.y
   return ((x1 - x2) ^ 2 + (y1 - y2) ^ 2) ^ 0.5;
end

local function point_center(...)
   local points = { ... };
   local cx, cy = 0.0, 0.0
   for _, point in pairs(points) do
      cx = cx + point.x
      cy = cy + point.y
   end
   return cx / #points, cy / #points
end

local function point_deg(p1, p2)
   local x1, y1 = p1.x, p1.y
   local x2, y2 = p2.x, p2.y
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

local function bottom_x(cx, cy, deg)
   local bx = cx - math.tan(math.rad(math.min(math.max(-85.0, deg), 85))) * (cy - (-1.0))
   bx = math.min(math.max(-1.0, bx), 1.0)
   return bx
end

local function boxes_bars(bars)
   local results = {}
   for i = 1, #bars do
      local points = bars[i].points
      local area = bars[i].area
      local dist_p12 = point_dist(points[1], points[2])
      local dist_p23 = point_dist(points[2], points[3])
      local length, width = math.max(dist_p12, dist_p23) / 2.0, math.min(dist_p12, dist_p23) / 2.0
      if width < 1 / 15 then
         print("跳过过小的宽度")
         goto continue
      elseif length < 1 / 6 then
         print("跳过过小的长度")
         goto continue
      elseif length / width > 6 then
         print("跳过错误的长宽比")
         goto continue
      end
      local fill_rate = area / (length * width)
      local p1, p2
      if dist_p12 > dist_p23 then
         p1, p2 = points[1], points[2]
      else
         p1, p2 = points[2], points[3]
      end
      local deg = point_deg(p1, p2) - 90.0
      if math.abs(deg) > 89.0 then
         goto continue
      end
      local cx, cy = point_center(points[1], points[2], points[3], points[4])
      table.insert(results, { cx, cy, deg, fill_rate, area })
      ::continue::
   end
   table.sort(results, function(a, b) return a[4] * a[5] > b[4] * b[5] end)
   return results
end

function main(input)
   local boxes, detect = {}, {}
   local function update()
      input = coroutine.yield()
      local bar_any = input["find_bar"]
      if bar_any then
         boxes = FindBarResults.from_any(bar_any).result
         writer:process(FindBarResults.from_any(bar_any).frame)
      else
         boxes = {}
      end
      local detect_any = input["detect"]
      if detect_any then
         detect = ObjectDetectResults.from_any(detect_any).result
      end
   end

   local function sleep(duration)
      time = current_time() + duration
      update()
   end

   while true do
      local bars
      local function update_results()
         bars = boxes_bars(boxes)
         return #bars > 0
      end
      update_results()
      ::find_bar_start::
      ::find_bar_2::
      if #bars >= 2 then
         if bars[1][2] > bars[2][2] then
            bars[1], bars[2] = bars[2], bars[1]
         end
         local p1, p2 = cv.Point2d.new(), cv.Point2d.new()
         local deg2
         p1.x, p1.y = unpack(bars[1])
         p2.x, p2.y, deg2 = unpack(bars[2])
         if p1.y > 0.25 then
            -- +-----------+
            -- |       --- |
            -- |     |     |
            -- |     |     |
            -- +-----------+
            goto find_bar_1
         end
         -- +-----------+
         -- |           |
         -- |       --- |
         -- |     |     |
         -- +-----------+
         local deg = point_deg(p1, p2) - 90.0
         local bx = p1.x
         print(2, bx, deg)
         server.move{x=bx, y=0.2, z=0.0, rot=(deg / 90.0) / 2.0}
         goto find_bar_finish
      end
      ::find_bar_1::
      if #bars >= 1 then
         local cx, cy, deg = unpack(bars[1])
         local bx = bottom_x(cx, cy, deg)
         local by = 0.2
         if math.abs(deg) > 75.0 then
            -- +-----------+
            -- |           |
            -- |      ,_.-^|
            -- |     `     |
            -- +-----------+
            deg = 90.0 * cx
         elseif math.abs(cx) > 0.5 and math.abs(deg) > 30.0 and cx * deg < 0.0 then
            -- +-----------+
            -- |       \   |
            -- |        \  |
            -- |         \ |
            -- +-----------+
            deg = deg / (math.abs(cx) * 4.0)
            by = by / (math.abs(cx) * 4.0)
         end
         print(1, bx, deg)
         server.move{x=bx, y=by, z=0.0, rot=(deg / 90.0) / 2.0}
         goto find_bar_finish
      end
      ::find_bar_0::
      if #bars >= 0 then
         print(0)
         local function try_motion(motion, duration)
            local delta = 0.1
            while true do
               server.move(motion)
               sleep(delta)
               if update_results() then
                  return true
               end
               duration = duration - delta
               if duration <= 0.0 then
                  return false
               end
            end
         end
         try_motion({x=0.0, y=0.5, z=0.0, rot=0.0}, 3.0)
         local rotation = 0.5
         for duration=1,3 do
            if try_motion({x=0.0, y=0.0, z=0.0, rot=rotation}, duration / 2.0) or
               try_motion({x=0.0, y=0.0, z=0.0, rot=-rotation}, duration) or
               try_motion({x=0.0, y=0.0, z=0.0, rot=rotation}, duration / 2.0) then
               print("重新巡线")
               goto find_bar_start
            end
         end
         goto find_bar_finish
      end
      ::find_bar_finish::
      if #detect > 0 then
         for i = 1, #detect do
            print(detect[i].name)
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
