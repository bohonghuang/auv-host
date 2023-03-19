#include "yolo_fast_v2.h"

#include <fstream>

#include "utils.h"
#include "model_lib.h"

namespace auv::vision::network {
    
static cv::Mat generic_grid(int nx = 20, int ny = 20) {
    cv::Mat result(cv::Size(2, nx*ny), CV_32FC1);
    int tmp = 0;
    for(int i = 0; i < nx; ++i) {
        for(int j = 0; j < ny; ++j) {
            result.at<float>(tmp++) = j;
            result.at<float>(tmp++) = i;
        }
    }
    return result;
}

static cv::Mat get_grid(int nx = 20, int ny = 20) {
    cv::Mat result = generic_grid(nx, ny);
    return result.clone();
}

YoloFastV2::YoloFastV2(const std::string& module_config_file_path, float obj_threshold,
                       float conf_threshold, float nms_threshold) 
    : m_obj_threshold(obj_threshold),
      m_conf_threshold(conf_threshold),
      m_nms_threshold(nms_threshold) {
    this->load_data_file(module_config_file_path);

    std::ifstream ifs(m_model_config.names_path);
    assert(ifs.is_open());

    for(std::string line; std::getline(ifs, line); ) {
        if(char tmp = line[0]; tmp == '\0' || tmp ==' ')
            continue;
        m_classes.push_back(line);
    }

    m_anchors = cv::Mat::zeros(cv::Size(2, 3), CV_32FC2);
    for(int i = 0; i < 2*3*2; ++i) {
        m_anchors.at<float>(i) = m_model_config.anchors[i];
    }

    m_net = ModelLibs::GetInstance().get_net("marine"); //"./marine.onnx"
}

void YoloFastV2::load_data_file(const std::string& file_path) {
    std::ifstream ifs(file_path);
    assert(ifs.is_open());

    std::map<std::string, std::string> key_value;
    std::vector<std::string> temp_key_value;
    for(std::string line; std::getline(ifs, line); ) {
        if(char tmp = line[0];tmp  == '\n' || tmp == '\r' || tmp == '[' || line.empty())
            continue;
        temp_key_value = utils::split(line, "=");
        temp_key_value[1] = *(temp_key_value[1].cend()-1) == '\r' ? 
                                temp_key_value[1].substr(0, temp_key_value[1].length()-1) :
                                temp_key_value[1];
        key_value[temp_key_value[0]] = temp_key_value[1];
    }
    ifs.close();

    for(const auto& i : key_value) {
        if(i.first == "anchors") {
            auto temp_vector = utils::split(i.second, ",");
            for(const auto& i : temp_vector) {
                m_model_config.anchors.push_back(std::atof(i.c_str()));
            }
            continue;
        }
        if(i.first == "steps") {
            auto temp_vector = utils::split(i.second, ",");
            for(const auto& i : temp_vector) {
                m_model_config.steps.push_back(std::atof(i.c_str()));
            }
            continue;
        }
        if(i.first == "model_name") {
            m_model_config.model_name = i.second;
            continue;
        }
        if(i.first == "val") {
            m_model_config.val_path = i.second;
            continue;
        }
        if(i.first == "train") {
            m_model_config.train_path = i.second;
            continue;
        }
        if(i.first == "names") {
            m_model_config.names_path = i.second;
            continue;
        }
        if(i.first ==  "pre_weights") {
            m_model_config.pre_weights = i.second;
            continue;
        }
        if(i.first == "epochs") {
            m_model_config.epochs = std::atoi(i.second.c_str());
            continue;
        }
        if(i.first == "batch_size") {
            m_model_config.batch_size = std::atoi(i.second.c_str());
            continue;
        }
        if(i.first == "classes") {
            m_model_config.classes = std::atoi(i.second.c_str());
            continue;
        }
        if(i.first == "width") {
            m_model_config.width = std::atoi(i.second.c_str());
            continue;
        }
        if(i.first == "height") {
            m_model_config.height = std::atoi(i.second.c_str());
            continue;
        }
        if(i.first == "anchor_num") {
            m_model_config.anchor_num = std::atoi(i.second.c_str());
            continue;
        }
        if(i.first ==  "subdivisions") {
            m_model_config.subdivisions = std::atoi(i.second.c_str());
            continue;
        }
        if(i.first == "learning_rate") {
            m_model_config.learning_rate = std::atof(i.second.c_str());
            continue;
        }
        std::cout << "配置文件有错误的配置项  名称为：" << i.first << std::endl;
    }
}

cv::Mat YoloFastV2::forward(const cv::Mat& src_img) {
    auto blob = cv::dnn::blobFromImage(src_img, 1.0/255.0, cv::Size(m_model_config.width, m_model_config.height));
    m_net.setInput(blob);

    std::vector<cv::Mat> net_work_result_vec;
    m_net.forward(net_work_result_vec, m_net.getUnconnectedOutLayersNames());

    cv::Mat net_work_result = net_work_result_vec[0];

    cv::Mat result = cv::Mat::zeros(cv::Size(5 + m_classes.size(), net_work_result.rows*m_model_config.anchor_num), CV_32FC1);
    for(int i = 0, row_index = 0; i < m_stride.size(); ++i) {
        int h = m_model_config.height/m_stride[i];
        int w = m_model_config.width/m_stride[i];
        int length = h * w;
        int anchor_repeat_times = h*w/m_anchors.row(0).col(0).cols;

        cv::Mat grid = get_grid(w, h);
        for(int j = 0; j < m_model_config.anchor_num; ++j) {
            int y = row_index + j * length;
            int x = 4 * j;

            cv::Range row_range(y, y+length);
            result(row_range, cv::Range(0, 2)) = (net_work_result(cv::Range(row_index, row_index + length), cv::Range(x, x+2))*2.0-0.5+grid) * m_stride[i];
            cv::Mat temp;
            cv::pow((net_work_result(cv::Range(row_index, row_index + length), cv::Range(x+2, x+4)) * 2), 2, temp);
            cv::Mat temp_anchor(m_anchors.rows, m_anchors.cols, CV_32FC1, ((float*)m_anchors.data + i*m_anchors.rows*m_anchors.cols));
            result(row_range, cv::Range(2, 4)) = temp.mul(cv::repeat(temp_anchor.row(j), anchor_repeat_times, 1));
            net_work_result.rowRange(row_index, row_index + length).col(4*m_model_config.anchor_num+j).copyTo(result.rowRange(row_range).col(4));
            net_work_result(cv::Range(row_index, row_index + length), cv::Range(5*m_model_config.anchor_num, net_work_result.cols)).copyTo(result(row_range, cv::Range(5, result.cols)));
        }
        row_index += length;
    }
    return result;
}

std::map<std::string, std::vector<YoloFastV2Result>> YoloFastV2::process(const cv::Mat& img, const cv::Mat& detect_result) {
    int img_height = img.size().height;
    int img_width = img.size().width;
    static float ratioh = (float)img_height / m_model_config.height;
    static float ratiow = (float)img_width / m_model_config.width;

    static std::vector<int> class_ids;
    static std::vector<float> confidences;
    static std::vector<cv::Rect> boxes;
    class_ids.clear();
    confidences.clear();
    boxes.clear();

    // 后面363行全是0
    for(int i = 0; i < detect_result.rows-363; ++i) {
        cv::Mat tmp = detect_result.row(i);

        cv::Mat scores = tmp.colRange(5, tmp.cols);
        int class_id = 0;
        for(int j = 1; j < tmp.cols - 5; ++j) {
            if(scores.at<float>(0, class_id) < scores.at<float>(0, j))
                class_id = j;
        }
        float confidence = scores.at<float>(0, class_id);
        if(confidence > m_conf_threshold && tmp.at<float>(0, 4) > m_obj_threshold) {
            int center_x = tmp.at<float>(0, 0) * ratiow;
            int center_y = tmp.at<float>(0, 1) * ratioh;

            cv::Rect rect;
            rect.width  = tmp.at<float>(0, 2) * ratiow;
            rect.height = tmp.at<float>(0, 3) * ratioh;
            rect.x = center_x - rect.width / 2;
            rect.y = center_y - rect.height / 2;

            boxes.push_back(rect);
            confidences.push_back(confidence * tmp.at<float>(0, 4));
            class_ids.push_back(class_id);
        }
    }

    std::map<std::string, std::vector<YoloFastV2Result>> results;

    if(boxes.empty())
        goto end_up;

    static std::vector<int> indices;
    indices.clear();
    cv::dnn::NMSBoxes(boxes, confidences, m_conf_threshold, m_nms_threshold, indices);
    for(auto i : indices) {
        YoloFastV2Result result;
        cv::Rect rect = boxes[i];
        result.rect = rect;
        result.confidences = confidences[i];
        result.name = m_classes[class_ids[i]];

        results[result.name].push_back(result);
        // this->draw_pred(img, result.confidences, class_ids[i], rect);
    }

end_up:
    return results;
}

void YoloFastV2::draw_pred(cv::Mat& img, float confidence, const std::string& name, const cv::Rect& rect_range) {
        cv::rectangle(img, rect_range, cv::Scalar(0, 0, 255), 2);

        static std::string label;
        label = name;
        label += std::to_string(confidence);

        auto label_size = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, nullptr);
        auto y = std::max(rect_range.y, label_size.height);
        cv::putText(img, label, cv::Point(rect_range.x, y), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 0));
}

} // namespace auv::vision::network

