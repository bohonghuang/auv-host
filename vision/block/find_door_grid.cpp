#include "find_door_grid.h"

#include <array>

#include "utils.h"

namespace auv::vision {

FindDoorGrid::FindDoorGrid(int row_count, int col_count)
    : m_row_count(row_count), m_col_count(col_count) {}


static size_t count_max_continuous(cv::MatIterator_<uint8_t> begin, const cv::MatIterator_<uint8_t> end) {
  size_t i = 0, j =0;
  for(; begin != end; ++begin) {
    if(*begin) {
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

  std::vector<std::pair<int, int>> heights;// (index, height)
  for (int i = 0; i < m_col_count; ++i) {
    cv::Mat col = cell_mat.col(i);
    size_t height = count_max_continuous(col.begin<uint8_t>(), col.end<uint8_t>());
    if(height >= (m_row_count + 1)/2)
      heights.emplace_back(i, height);
  }



  FindDoorGridResults results;
  return results;
}


}// namespace auv::vision
