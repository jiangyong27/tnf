#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "reader.h"
CReader::CReader()
{
    m_pack = new LongconnHelper();
    m_session_mgr = new CSessionMgr();
    m_epoll = new CEpoll(10);
    m_buffer_size = 1024 * 1024 * 10;
    m_buffer = NULL;
    m_timeout_num = 0;
    m_send_num = 0;
    m_recv_num = 0;
    m_threshold1 = 100;
    m_threshold2 = 200;
    m_threshold3 = 500;
    m_threshold1_num = 0;
    m_threshold2_num = 0;
    m_threshold3_num = 0;
    m_total_cost = 0;
}

CReader::~CReader()
{
    delete m_epoll;
    delete m_session_mgr;
    delete m_pack;
    if (m_buffer) delete [] m_buffer;
}

void CReader::Init(const reader_conf_t& conf)
{
    m_input_dir = conf.input_dir;
    m_send_queue = conf.send_queue;
    m_recv_queue = conf.recv_queue;
    m_max_qps = conf.max_qps;
    m_buffer = new char[m_buffer_size];
    m_logger = conf.logger;

    m_epoll->Add(m_recv_queue->Fd(), true, false);
}

void CReader::Send()
{
    static uint32_t idx = 0;
    std::string& query = m_query[idx % m_query.size()];
    idx++;
    if (m_send_queue->Push(query.c_str(),query.size())) {
        LOG_ERROR(m_logger, "push client send queue failed!");
        return;
    }
    m_send_num++;
}

void CReader::Recv()
{
    uint32_t data_len;
    for (uint32_t i = 0; i < 10; i++) {
        int ret = m_recv_queue->Pop(m_buffer, m_buffer_size, &data_len);
        if (ret != 0 ){
            break;
        }

        server_result_t *result = (server_result_t*) m_buffer;
        m_total_cost += result->cost;
        if (result->result_num == 0) {
            m_zero_num++;
        }
        if (result->retcode == 2) {
            m_timeout_num++;
        } else {
            m_recv_num++;
        }
        if (result->cost > m_threshold3) {
            m_threshold3_num++;
        } else if (result->cost > m_threshold2) {
            m_threshold2_num++;
        } else if (result->cost > m_threshold1) {
            m_threshold1_num++;
        }
    }
}

void CReader::Run()
{
    FileUtility::LoadLines(m_input_dir, &m_query);
    LOG_P("reader read num[%d]", m_query.size());

    time_t pre = 0;
    time_t now = time(NULL);
    uint32_t pre_recv = 0;
	char szTmpStr1[512];
	char szTmpStr2[512];
	char szTmpStr3[512];
	std::string result;

	snprintf(szTmpStr1, 512, ">%.3fms", (float)m_threshold3);
	snprintf(szTmpStr2, 512, ">%.3fms", (float)m_threshold2);
	snprintf(szTmpStr3, 512, ">%.3fms", (float)m_threshold1);
    CNetGrid flow_grids(100, 10, m_max_qps);

    std::vector<void*> rfds;
    while( 1 ) {

        if (flow_grids.check_load() == 0) {
            Send();
        }

        rfds.clear();
        m_epoll->Wait(&rfds, NULL, 0);
        if (rfds.size() != 0)
            Recv();

        //usleep(10);
        now = time(NULL);
        if (now - pre >= 1) {
            if (now % 10 == 0 || pre == 0) {
                printf("------------------------------------------------------");
                printf("-------------------------------------------------------\n");
                printf("%-20s|%10s|%10s|%10s|%10s|%10s|%10s|%10s|%10s|%10s\n",
                                      "TIME","SEND(N)","RECV(N)","QPS(N)","ZERO(N)","TIMEOUT(N)","AVG(ms)",szTmpStr1,szTmpStr2,szTmpStr3);
            }
            DateTime now_time;
            float avg_cost = m_total_cost / (float)(m_recv_num + m_timeout_num + 1);
            printf("%-20s|%10d|%10d|%10d|%10d|%10d|%10.2f|%10d|%10d|%10d\n",
                  now_time.ToString().c_str() ,m_send_num, m_recv_num, m_recv_num - pre_recv, m_zero_num, m_timeout_num, avg_cost,
                  m_threshold3_num, m_threshold2_num,  m_threshold1_num);

            pre = now;
            m_send_num = 0;
            m_recv_num = 0;
            m_timeout_num = 0;
            m_total_cost = 0;
            m_zero_num = 0;
            m_threshold1_num = 0;
            m_threshold2_num = 0;
            m_threshold3_num = 0;
        }
    }
}


