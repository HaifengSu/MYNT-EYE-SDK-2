// Copyright 2018 Slightech Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <opencv2/highgui/highgui.hpp>

#include <glog/logging.h>

#include "mynteye/api.h"

#include "util/cv_painter.h"

MYNTEYE_USE_NAMESPACE

int main(int argc, char *argv[]) {
  auto &&api = API::Create(argc, argv);
  if (!api)
    return 1;

  // Enable this will cache the motion datas until you get them.
  api->EnableMotionDatas();

  api->Start(Source::ALL);

  CVPainter painter;

  cv::namedWindow("frame");

  while (true) {
    api->WaitForStreams();

    auto &&left_data = api->GetStreamData(Stream::LEFT);
    auto &&right_data = api->GetStreamData(Stream::RIGHT);

    cv::Mat img;
    cv::hconcat(left_data.frame, right_data.frame, img);

    auto &&motion_datas = api->GetMotionDatas();
    /*
    for (auto &&data : motion_datas) {
      LOG(INFO) << "Imu frame_id: " << data.imu->frame_id
                << ", timestamp: " << data.imu->timestamp
                << ", accel_x: " << data.imu->accel[0]
                << ", accel_y: " << data.imu->accel[1]
                << ", accel_z: " << data.imu->accel[2]
                << ", gyro_x: " << data.imu->gyro[0]
                << ", gyro_y: " << data.imu->gyro[1]
                << ", gyro_z: " << data.imu->gyro[2]
                << ", temperature: " << data.imu->temperature;
    }
    */

    painter.DrawImgData(img, *left_data.img);
    if (!motion_datas.empty()) {
      painter.DrawImuData(img, *motion_datas[0].imu);
    }

    cv::imshow("frame", img);

    char key = static_cast<char>(cv::waitKey(1));
    if (key == 27 || key == 'q' || key == 'Q') {  // ESC/Q
      break;
    }
  }

  api->Stop(Source::ALL);
  return 0;
}
