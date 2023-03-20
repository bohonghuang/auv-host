time = current_time()

function main(input)
   local bar, detect
   function update()
      input = coroutine.yield()
      local bar_any = input["find_bar"]
      if bar_any then
         bar = FindBarResults.from_any(bar_any).result
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
      update()
      if bar then
         for i=1,#bar do
            box = bar[i]
            print(box.point.x, box.point.y, box.angle)
         end
      end
      if detect then
         print(detect)
      end
   end
end

co = coroutine.create(main)

function process(input)
   if time <= current_time() then
      coroutine.resume(co, input)
   end
   return void:to_any()
end
