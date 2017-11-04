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

#include <nlp/nlp_node.h>

int flag = 0;
string result;


int writer(char *data, size_t size, size_t nmemb, string *writerData)
{
    unsigned long sizes = size * nmemb;
    if (writerData == NULL)
        return -1;

    writerData->append(data, sizes);

    return sizes;
}

int parseJsonResonse(string input)
{
   Json::Value root;
   Json::Reader reader;
   bool parsingSuccessful = reader.parse(input, root);
   if(!parsingSuccessful)
   {
       std::cout<<"!!! Failed to parse the response data"<< std::endl;
        return -1;
   }
   const Json::Value code = root["code"];
   const Json::Value text = root["text"];
   result = text.asString();
   flag = 1;

   std::cout<< "response code:" << code << std::endl;
   std::cout<< "response text:" << result << std::endl;

   return 0;
}

int HttpPostRequest(string input)
{
    string buffer;

    std::string strJson = "{";
    strJson += "\"key\" : \"f483d25950304b129bd7b1cd9c01fdbe\","; //双引号前加／防转仪
    strJson += "\"info\" : ";
    strJson += "\"";
    strJson += input;
    strJson += "\"";
    strJson += "}";

    std::cout<<"post json string: " << strJson << std::endl;

    try
    {
        CURL *pCurl = NULL;
        CURLcode res;
        // In windows, this will init the winsock stuff
        curl_global_init(CURL_GLOBAL_ALL);

        // get a curl handle
        pCurl = curl_easy_init();
        if (NULL != pCurl)
        {
            // 设置超时时间为10秒
            curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 10);

            // First set the URL that is about to receive our POST.
            // This URL can just as well be a
            // https:// URL if that is what should receive the data.
            curl_easy_setopt(pCurl, CURLOPT_URL, "http://www.tuling123.com/openapi/api");
            //curl_easy_setopt(pCurl, CURLOPT_URL, "http://192.168.0.2/posttest.cgi");

            // 设置http发送的内容类型为JSON
            curl_slist *plist = curl_slist_append(NULL,
                "Content-Type:application/json;charset=UTF-8");
            curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, plist);

            // 设置要POST的JSON数据
            curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, strJson.c_str());

            curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, writer);

            curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &buffer);

            // Perform the request, res will get the return code
            res = curl_easy_perform(pCurl);
            // Check for errors
            if (res != CURLE_OK)
            {
                printf("curl_easy_perform() failed:%s\n", curl_easy_strerror(res));
            }
            // always cleanup
            curl_easy_cleanup(pCurl);
        }
        curl_global_cleanup();
    }
    catch (std::exception &ex)
    {
        printf("curl exception %s.\n", ex.what());
    }
    if(buffer.empty())
    {
      std::cout<< "!!! ERROR The Tuling sever response NULL" << std::endl;
    }
    else
    {
        parseJsonResonse(buffer);
    }

    return 0;
}

void Nlp::nlp_cb(const std_msgs::String::ConstPtr &msg)
{
    std::cout<<"your quesion is: " << msg->data << std::endl;
    HttpPostRequest(msg->data);
}

Nlp::Nlp(void)
{
    //Init node
    ROS_ASSERT(init());
}

Nlp::~Nlp(void)
{

}

bool Nlp::init(void)
{
    ros::param::get("~sub_topic", sub_topic);
    ros::param::get("~pub_topic", pub_topic);

    nlp_sub = nd.subscribe(sub_topic, 10, &Nlp::nlp_cb, this);
    nlp_pub = nd.advertise<std_msgs::String>(pub_topic, 10);

    return true;
}

bool Nlp::update(void)
{
    if(flag)
    {
        std_msgs::String msg;
        msg.data = result;
        nlp_pub.publish(msg);
        flag = 0;
    }
    return true;
}

/*******************************************************************************
* Main function
*******************************************************************************/
int main(int argc, char* argv[])
{
    ros::init(argc, argv,"tl_nlp");
    Nlp nlp;

    ros::Rate loop_rate(10);
    while(ros::ok())
    {
        nlp.update();
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
