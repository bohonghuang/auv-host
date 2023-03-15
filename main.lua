cam = Camera.new(0, 588.4306598875787, 322.7472860229715, 592.781786987308, 242.4471017083893, -0.1443039341764572, 0.91856728920134, 0.0, 0.0, -2.402839834767997)
red_bar = VisionRedBar.new()

red_bar:start(cam)
while true do
    imshow(red_bar:get_result().frame)
end
