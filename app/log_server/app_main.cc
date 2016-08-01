#include "app_worker.h"
#include "app_writer.h"
#include "net/net_conf.h"
#include "minicore.h"


static void dispatch_reader(void* priv)
{
    CAppWorker *worker = (CAppWorker*) priv;
    worker->DispatchReader();
}

int app_main(int argc, char **argv)
{
    if (argc <= 1) {
        printf("usage: %s conf_file\n", argv[0]);
        return -100;
    }

    //InitMiniCore();
    std::string conf_file = argv[1];
    uint32_t worker_num;

    CNetConf conf;
    conf.OpenIniFile(conf_file);
    conf.GetFiledValue("main", "worker_num", 1, &worker_num);

    CLogger *reader_logger = new CLogger("../log/tnf_reader.log");
    CLogger *worker_logger = new CLogger("../log/tnf_worker.log", true);
    reader_logger->SetLevel(LOG_LEVEL_DEBUG);
    worker_logger->SetLevel(LOG_LEVEL_DEBUG);


    CTNFReader *reader = new CTNFReader();
    reader->Init(conf_file, reader_logger);

    CAppWriter *writer = new CAppWriter();
    CNetQueue* writer_queue = new CNetQueue(true, true);
    writer->SetQueue(writer_queue);

    for (uint32_t i = 0; i < worker_num; i++) {
        CNetQueue* queue1 = new CNetQueue(false, true);
        CNetQueue* queue2 = new CNetQueue(true,false);
        CAppWorker *worker = new CAppWorker();
        reader->AddQueue(queue1, queue2);
        worker->SetReaderQueue(queue2, queue1);
        worker->AddMQDispatch(queue2, dispatch_reader);
        worker->SetWriterQueue(writer_queue);

        if (0 != worker->Init(conf_file, worker_logger)) {
            return -2;
        }
        worker->Start();
    }
    writer->Start();
    reader->Start();

    while(1) {
        sleep(1);
    }
    return 0;
}

extern "C" {
    int app_run(int argc, char **argv) {
        return app_main(argc, argv);
    }

    int net_check(char *data_buf, uint32_t data_len) {
        return 0;
    }

}
