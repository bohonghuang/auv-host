//
// Created by Qff on 2023/3/5.
//

#include "../red_door.h"
#include "../red_door.h"
#include "../utils.h"

int main() {
  //auv::vision::RedDoorBlock rdb;
  auv::vision::RedDoorBlock rdb;
  auv::vision::regulate_threshold_params("./door.mkv", rdb);
  return 0;
}
