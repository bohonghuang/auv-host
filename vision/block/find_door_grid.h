#ifndef AUV_HOST_VISION_BLOCK_FIND_DOOR_GRID_H_
#define AUV_HOST_VISION_BLOCK_FIND_DOOR_GRID_H_

#include "block.h"
#include "find_door.h"
#include <opencv2/opencv.hpp>

namespace auv::vision {

struct FindDoorGridResults {

};

class FindDoorGrid : public auv::Block<cv::Mat, FindDoorGridResults> {
public:
  FindDoorGrid(int row_count, int col_count);
  Out process(In frame) override;

  AUV_BLOCK;
private:
  const int m_row_count;
  const int m_col_count;
};

}// namespace auv::vision

#endif//AUV_HOST_VISION_BLOCK_FIND_DOOR_GRID_H_
