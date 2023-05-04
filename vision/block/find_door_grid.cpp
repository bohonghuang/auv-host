#include "find_door_grid.h"

#include <array>
#include <cstddef>
#include <functional>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <tuple>

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

  std::vector<std::vector<cv::Mat>> cells = std::invoke([&]() {
    std::vector<std::vector<cv::Mat>> result(m_row_count);
    for (int i = 0; i < m_row_count; ++i) {
      std::vector<cv::Mat> cell_per_row(m_col_count);
      auto row_range = cv::Range(i * cell_height, (i + 1) * cell_height);
      for (int j = 0; j < m_col_count; ++j) {
        cell_per_row[j] = frame(row_range, cv::Range(j * cell_width, (j + 1) * cell_width));
      }
      result[i] = std::move(cell_per_row);
    }
    return result;
  });

  std::vector<std::vector<float>> cell_mat = std::invoke([this] {
    std::vector<float> tmp(m_col_count, 0);
    return std::vector<std::vector<float>>(m_row_count, tmp);
  });

  for (int i = 0; i < m_row_count; ++i) {
    for (int j = 0; j < m_col_count; ++j) {
      const auto &cell = cells[i][j];
      cell_mat[i][j] = (float) cv::countNonZero(cell) / cell_area;
    }
  }

  for (size_t i = 1; i < m_row_count; ++i) {
    int line_hegin = i * cell_height;
    cv::line(frame, cv::Point(0, line_hegin), cv::Point(width, line_hegin),
             cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
  }

  for (size_t i = 1; i < m_col_count; ++i) {
    int line_width = i * cell_width;
    cv::line(frame, cv::Point(line_width, 0), cv::Point(line_width, height),
             cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
  }

  FindDoorGridResults results{frame,
                              cell_mat};
  return results;
}


}// namespace auv::vision
