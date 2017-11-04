/*******************************************************************************
* Copyright 2017 CLNST CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef __NLP_NODE_H__
#define __NLP_NODE_H__

#include<ros/ros.h>
#include<std_msgs/String.h>
#include<iostream>
#include<sstream>
#include<jsoncpp/json/json.h>
#include<curl/curl.h>
#include<string>
#include<exception>

using namespace std;

class Nlp{
public:
    Nlp(void);
    ~Nlp(void);

    bool init(void);
    bool update(void);

private:
    ros::NodeHandle nd;
    ros::Subscriber nlp_sub;
    ros::Publisher nlp_pub;

    std::string sub_topic;
    std::string pub_topic;
    std::string user_id;

    void nlp_cb(const std_msgs::String::ConstPtr &msg);

};

#endif // __NLP_NODE_H__