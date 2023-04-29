#include "find_door_grid.h"

#include <array>

#include "utils.h"

namespace auv::vision {

FindDoorGrid::FindDoorGrid(int row_count, int col_count)
    : m_row_count(row_count), m_col_count(col_count) {}


static size_t count_max_continuous(cv::MatIterator_<uint8_t> begin, const cv::MatIterator_<uint8_t> end) {
  size_t i = 0, j = 0;
  for (; begin != end; ++begin) {
    if (*begin) {
      i++;
    } else {
      j = std::max(i, j);
      i = 0;
    }
  }
  j = std::max(i, j);
  return j;
}


FindDoorGridResults FindDoorGrid::process(cv::Mat frame) {
  const int width = frame.size().width;
  const int height = frame.size().height;
  static const cv::Mat kernel1 =
      cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                cv::Size(height / 96, height / 96));
  static const cv::Mat kernel2 =
      cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                cv::Size(height / 48, height / 48));
  cv::morphologyEx(frame, frame, cv::MORPH_OPEN, kernel1);
  cv::morphologyEx(frame, frame, cv::MORPH_CLOSE, kernel2);

  int cell_width = width / m_col_count;
  int cell_height = height / m_row_count;
  int cell_area = cell_width * cell_height;

  std::vector<std::vector<cv::Mat>> cells = [&]() {
    std::vector<std::vector<cv::Mat>> result(m_row_count);
    for (int i = 0; i < m_row_count; ++i) {
      std::vector<cv::Mat> cell_per_row(m_col_count);
      auto row_range = cv::Range(i * cell_height, (i + 1) * cell_height);
      for (int j = 0; j < m_col_count; ++j)
        cell_per_row[j] = frame(row_range, cv::Range(j * cell_width, (j + 1) * cell_width));

      result[i] = std::move(cell_per_row);
    }
    return result;
  }();

  cv::Mat cell_mat(m_row_count, m_col_count, CV_8UC1);
  for (int i = 0; i < m_row_count; ++i) {
    for (int j = 0; j < m_col_count; ++j) {
      const auto &cell = cells[i][j];
      if (cv::countNonZero(cell) > cell_area / 20)
        cell_mat.at<uint8_t>(i, j) = 255;
    }
  }

  std::vector<std::pair<int, int>> edge_heights;// (index, height)
  for (int i = 0; i < m_col_count; ++i) {
    cv::Mat col = cell_mat.col(i);
    size_t edge_height = count_max_continuous(col.begin<uint8_t>(), col.end<uint8_t>());
    if (edge_height >= (m_row_count + 1) / 2)
      edge_heights.emplace_back(i, edge_height);
  }

  float roi_dev = 0.0;
  float roi_deg = 0.0;
  float confidence = 1.0;
  if (edge_heights.size() == 2) {// 大于2怎么办？
    // +---+---+---+---+---+---+---+---+
    // | # |   |   |   | # |   |   |   |
    // +-#-+---+---+---+-#-+---+---+---+
    // | # |   |   |   | # |   |   |   |
    // +-#-+---+---+---+-#-+---+---+---+
    // | # |   |   |   | # |   |   |   |
    // +-#-+---+---+---+-#-+---+---+---+
    // | ################# |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    auto col_left = edge_heights[0];// heights={(index,height),...}
    auto col_right = edge_heights[1];
    int door_width = col_right.first - col_left.first;
    if (door_width > 1) {
      auto center_std = (1 + m_col_count) / 2 + 1;
      auto center_lr = (col_left.first + col_right.first) / 2 + 1;
      roi_dev = (float) (center_lr - center_std) / (float) (center_std - 1);

      auto min_height = std::min(col_left.second, col_right.second);
      if (door_width < min_height && col_left.second != col_right.second) {
        roi_deg = 1.0f - (float) door_width / (float) min_height;
        roi_deg *= 45.0;
        if (col_right.second - col_left.second < 0)
          roi_deg *= -1;
      }
    } else {
      if (col_left.second < col_right.second)
        edge_heights.erase(edge_heights.begin());
      else
        edge_heights.erase(edge_heights.begin() + 1);
    }
  }

  if (edge_heights.size() == 1) {
    auto [col_index, col_height] = edge_heights[0];
    auto index_limit = [this](int index) {
      if (index > m_col_count - 1) {
        index = m_col_count - 1;
      }
      if (index < 0) {
        index = 0;
      }
      return index;
    };

    uint8_t hoffset_left = index_limit(cell_mat.at<uint8_t>(col_height - 1, col_index - 1));
    uint8_t hoffset_right = index_limit(cell_mat.at<uint8_t>(col_height - 1, col_index + 1));
    uint8_t hoffset_bottom_left = index_limit(cell_mat.at<uint8_t>(col_height, col_index - 1));
    uint8_t hoffset_bottom_right = index_limit(cell_mat.at<uint8_t>(col_height, col_index + 1));
    uint8_t hoffset_top_left = index_limit(cell_mat.at<uint8_t>(col_height - 2, col_index - 1));
    uint8_t hoffset_top_right = index_limit(cell_mat.at<uint8_t>(col_height - 2, col_index + 1));
    if (hoffset_right || hoffset_bottom_right || hoffset_top_right) {
      // |   |   |   |
      // +---+---+---+
      // |   | # |   |
      // +---+-#-+---+
      // |   | ######|
      confidence /= 2;
      roi_dev = (float) col_index / m_col_count;
    }
    if (hoffset_left || hoffset_bottom_left || hoffset_top_left) {
      // |   |   |   |
      // +---+---+---+
      // |   | # |   |
      // +---+-#-+---+
      // | ##### |   |
      confidence /= 2;
      roi_dev = -(float) (m_col_count - col_index) / m_col_count;
    }

    if (hoffset_top_left || hoffset_bottom_right) {
      // +---+-#-+---+
      // |   | # |   |
      // +---+-#-+---+
      // |   | ##|   |
      // +---+---##--+
      // |   |   | # |
      // +---+---+---+
      roi_deg = 0.5;
      goto finish_detect;
    }
    if (hoffset_top_right || hoffset_bottom_left) {
      // +-#-+---+---+
      // | # |   |   |
      // +-#-+--#+---+
      // | # | # |   |
      // +-#-##--+---+
      // | ##|   |   |
      // +---+---+---+
      roi_deg = -0.5;
      goto finish_detect;
    }

    // +---+-#-+---+
    // |   | # |   |
    // +---+-#-+---+
    // |   | # |   |
    // +---+-#-+---+
    // |   | # |   |
    // +---+---+---+
    confidence /= 2;
    int mid_index = (m_col_count + 1) / 2;
    if (col_index < mid_index) {
      roi_dev = (float) col_index / m_col_count;
    }
    if (col_index > mid_index) {
      roi_dev = -(float) (m_col_count - col_index) / m_col_count;
    }
  }
finish_detect:
  std::cout << cell_mat << std::endl;
  FindDoorGridResults results{frame,
                              confidence,
                              roi_deg,
                              roi_dev};
  return results;
}


}// namespace auv::vision
