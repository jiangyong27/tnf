#include <stdlib.h>
#include <stdio.h>
#include "reader.h"
#include "worker.h"

int main(int argc, char ** argv)
{
    if (argc <= 3) {
        printf("usage: %s ip:port query_file max_qps\n", argv[0]);
        exit(-1);
    }
    uint32_t worker_num = 1;
    CLogger* logger = new CLogger("/tmp/presscall.log", true);
    logger->SetLevel(LOG_LEVEL_DEBUG);

    CNetQueue *reader_send_queue = new CNetQueue(true, false);
    CNetQueue *reader_recv_queue = new CNetQueue(false, true);

    worker_conf_t worker_conf;
    reader_conf_t reader_conf;

    worker_conf.addr = argv[1];
    worker_conf.reader_send_queue = reader_send_queue;
    worker_conf.reader_recv_queue = reader_recv_queue;
    worker_conf.logger = logger;

    reader_conf.input_dir = argv[2];
    reader_conf.logger = logger;
    reader_conf.send_queue = reader_send_queue;
    reader_conf.recv_queue = reader_recv_queue;
    reader_conf.max_qps = atoi(argv[3]);

    for (uint32_t i = 0; i < worker_num; i++) {
        CWorker *worker = new CWorker();
        worker->Init(worker_conf);
        worker->Start();
    }

    CReader *reader = new CReader();
    reader->Init(reader_conf);
    reader->Start();
    reader->Join();

    return 0;
}
