time = current_time()
rov = RovController.new("localhost", 8888)

function main(input)
    local bar, bars, detect
    bars = {}
    bar = FindBarResult.new()
    function update()
        input = coroutine.yield()
        local bar_any = input["find_bar"]
        if bar_any then
            bars = FindBarResults.from_any(bar_any).result
        end
        local detect_any = input["detect"]
        if detect_any then
            detect = ObjectDetectResults.from_any(detect_any).result
        end
    end
    function sleep(duration)
        time = current_time() + duration
        update()
    end
    while true do
        --if #bars > 0 then
        --    bar = FindBarResult.new()
        --    bar.points.x = 0.0
        --    bar.points.y = 0.0
        --    for i = 1, #bars do
        --        local bar_i = bars[i]
        --        print(bar_i.area)
        --        print("points_x:", bar_i.points.x)
        --        print("points_y:", bar_i.points.y)
        --        --bar.points.x = bar.points.x + bar_i.points.x
        --        --bar.points.y = bar.points.y + bar_i.points.y
        --    end
        --    bar.points.x = bar.points.x / #bars
        --    bar.points.y = bar.points.y / #bars
        --end
        if detect then
            for i = 1, #detect do
                print(detect[i].name)
            end
        end
        sleep(0.5)
    end
end

co = coroutine.create(main)

function process(input)
    if time <= current_time() then
        local result, err = coroutine.resume(co, input)
        if not result then
            print(err)
            tasks = SchedulerList.from_any(input["scheduler"])
            tasks:stop()
        end
    end
    return void:to_any()
end
