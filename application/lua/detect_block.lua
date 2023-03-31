require "application.lua.utils"
require "math"

DetectBlock = {
    detects = {},
    detect_tables = { shark = false,
                      squid = false,
                      coralreef = true,
                      turtle = true,
                      dolphin = true, },
    motions = {
        Hit = function(self)
            return function(server)
                local track = self.detects[1]
                local x, y, w, h = track.x, track.y, track.width, track.height
                local cx, cy = point_center({ x = x, y = y }, { x = x + w, y = y + h })
                server.move { x = 0.0, y = 0.2, z = 0.0, rot = cx / 2.0 }
                sleep(3.0)
            end
        end,

        WillHit = function(self)
            return function(server)
                server.move { x = 0.0, y = 0.2, z = 0.0, rot = 0.0 }
            end
        end,

        Stop = function(self)
            return function(server)
                server.move { x = 0.0, y = 0.0, z = 0.0, rot = 0.0 }
            end
        end
    }
}

local prev_track_name
local timer = 0
local really_detected = false
function DetectBlock.process()
    if not next(DetectBlock.detects) then
        if really_detected then
            print("疑似撞到生物 " .. prev_track_name .. " ，已等待 " .. timer .. " 秒")
            timer = timer + 0.1

            if timer > 5.0 then
                print("已撞到生物 " .. prev_track_name)
                timer = 0.0
                really_detected = false
                DetectBlock.detect_tables[prev_track_name] = false
                return
            end
            return DetectBlock.motions.WillHit(self)
        end

        return
    end

    local track = self.detects[1]

    if not prev_track_name then
        prev_track_name = track.name
    end

    if prev_track_name ~= track.name then
        prev_track_name = track.name
        return
    end

    if not really_detected then
        if timer <= 0.5 then
            print("疑似检测到生物 " .. prev_track_name .. " ，已等待 " .. timer .. " 秒")
            timer = timer + 0.1
            return DetectBlock.motions.Stop(self)
        else
            print("开始撞向生物" .. prev_track_name)
            really_detected = true
            timer = 0.0
        end
    end

    if really_detected then
        print("正在撞向生物" .. prev_track_name)
        timer = 0.0
        return DetectBlock.motions.Hit(self)
    end
end

function DetectBlock.update(raw_vision_detect_results)
    DetectBlock.detects = {}
    for i = 1, #raw_vision_detect_results do
        if DetectBlock.detect_tables[raw_vision_detect_results[i].name] then
            table.insert(DetectBlock.detects, raw_vision_detect_results[i])
        end
    end
    table.sort(DetectBlock.detects, function(a, b)
        return a.confidence > b.confidence
    end)
end
