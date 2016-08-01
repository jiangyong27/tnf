// base_protocol.h: interface for the BaseProtocol class.
// wookin@tencent.com
// 2007-05-11
//
// 说明:通用协议封包, 接解包
//        1.使用key-value对模型
//
//        2.支持的数据类型
//                          1bytes(char/unsigned char),=
//                          2bytes(SHORT16/USHORT16),
//                          4bytes(INT32/UINT32),
//                          n bytes(BYTES)
//
//        3.字节序转换，如果设定BASEPROTOCOL_OPT_NETORDER，自动做字节序转换，
//          应用层不用转换, BYTES类型字节序需要用户自行处理.
//
//        4.用户数据类型，可以自己定义结构按BYTES类型数据处理
//
//        5.增加数据来源检查, 在服务端第一次在某连接上收数据的时候, 当接收数据
//          大于等于sizeof(BasProtocolHead), 调用IsValidPack()检测是否正确的
//          数据包。
//
//        package:
//        +----------+
//        | head     |
//        | reserved |
//        |----------|
//        | seg      |
//        | seg      |
//        | ...      |
//        +__________+
//
//        seg.
//        +------------------------------------+
//        | seg name(Key) 2B                   |
//        | data type     1B                   |
//        | *data len*    4B (type = Bytes存在)|
//        | data...[len = data len]            |
//        +------------------------------------+
// ////////////////////////////////////////////////////////////////////

//
// update list
// ___________________________
// data        by     detail
// 2010-07-28  wookin 支持自定义数据类型, GetKey时候检查双方数据类型大小是否一致
// 2011-02-17  lonely 支持双向数据传输，一条链路的一方可以发起请求也可以发送响应
// 2011-02-19  loney  包头添加用户数据字段

#ifndef LONGCONN_BASE_PROTOCOL_H_
#define LONGCONN_BASE_PROTOCOL_H_

#ifndef WIN32
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#endif //

// #include "thirdparty/gflags/gflags.h"
// #include "longconn/log/log.h"
// #include "longconn/utility/base_config.h"

//_START_XFS_BASE_NAMESPACE_
namespace xfs {
namespace base {

#ifndef  hton64
#define  hton64(x)  (((uint64_t)(htonl((uint32_t)((x) & 0xffffffff))) << 32) | \
		                htonl( (uint32_t)(((x) >> 32) & 0xffffffff)  ))

#define  ntoh64(x)  (((uint64_t)(ntohl( (uint32_t)((x) & 0xffffffff) )) << 32) | \
		                 ntohl( (uint32_t)(((x) >> 32) & 0xffffffff)  ))
#endif // hton64

#ifndef STRLEN
#define STRLEN(x) ((x) ? strlen(x):0)
#endif // STRLEN

//
//  Basic protocol package options
//
enum ENUM_PROTOCOL_OPT {
    BASEPROTOCOL_OPT_NETORDER           = 0x01,
    BASEPROTOCOL_OPT_CRC_CHECK          = 0x02,   // 如果需要CRC check, 则Option
    // 高位一个Byte保存CRC8结果,
    // 计算CRC8不取Head部分

    BASEPROTOCOL_OPT_KEEPALIVE          = 0x04,   // KeepAlive = 1, 长连接,
    // 可以完成多次请求

    BASEPROTOCOL_OPT_TRANS_PIPELINING   = 0x08,   // 如果有可能, 请将该数据包尽量
    // 以pipelining(流水线)方式转发

    BASEPROTOCOL_OPT_IS_PIPELINING_HEAD = 0x10,   // 和BASEPROTOCOL_OPT_TRANS_
    // PIPELINING复用,
    // 存在该选项则说明传输的数据是
    // pipelining head
    //
    // 下面3个option只有在server需要主动向client发送数据包是才需要由应用主动设置
    //
    BASEPROTOCOL_OPT_IS_REQ_PACKAGE    = 0x0100, // 请求包
    BASEPROTOCOL_OPT_IS_RSP_PACKAGE    = 0x0200, // 响应包
    BASEPROTOCOL_OPT_IS_DEFAULT_PACKAGE= 0x0400, // 缺省包类型,请求还是响应根据
    // accept与connect关系确定
};

enum ENUM_PROTOCOL_VER {
    //
    // ENUM_PROTOCOL_VER_1 = 1,
    //
    ENUM_PROTOCOL_VER_2 = 2,                    //  增加协议头部自校验,
    // 应对扫描策略
};

//
// Checksum算法初始化key,
// key不同相同的结果会得到不同的checksum
//
#define  BASEPROTO_HEAD_CHKSUM_KEY        0x12370B6

//
//  General protocol public head
//
#ifdef WIN32
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif //

// for safe_rand(),线程安全rand
static int32_t safe_rand() {
    unsigned seed = time(NULL);
    srand(seed);
    return rand_r(&seed);
}

struct BasProtocolHead {
    //  Total length, include first 4 bytes
    uint32_t        length;
    unsigned char   ver;
    unsigned char   TTL;
    uint32_t        option;
    uint32_t        sequence;
    uint16_t        service_type;
    uint64_t        user_data;
    uint16_t        first_seg_pos;

    //
    // 增加来源检查,
    // 应对别的程序扫描策略
    // 每次都不同的随机数, 避免某些可能的cache机制
    uint32_t        random_num;

    //
    // checksum of head
    // 本字段之前的所有字段的CRC32()结果
    //
    uint32_t        head_checksum;

    //
    // checksum of body
    // 包体的checksum
    //
    uint32_t        checksum;

    BasProtocolHead() {
        memset(this, 0, sizeof(BasProtocolHead));
        ver = ENUM_PROTOCOL_VER_2;
        TTL = 10;
        option = 0;
        first_seg_pos = sizeof(BasProtocolHead);
        random_num = (uint32_t)safe_rand();
        head_checksum = 0;
        checksum = 0;
        length = sizeof(BasProtocolHead);
    }

    void ToNetOrder() {
        length = htonl(length);
        option = htonl(option);
        sequence = htonl(sequence);
        service_type = htons(service_type);
        user_data = hton64(user_data);
        first_seg_pos = htons(first_seg_pos);
        random_num = htonl(random_num);
        head_checksum = htonl(head_checksum);
        checksum = htonl(checksum);
    }

    void ToHostOrder() {
        length = ntohl(length);
        option = ntohl(option);
        sequence = ntohl(sequence);
        service_type = ntohs(service_type);
        user_data = ntoh64(user_data);
        first_seg_pos = ntohs(first_seg_pos);
        random_num = ntohl(random_num);
        head_checksum = ntohl(head_checksum);
        checksum = ntohl(checksum);
    }
};

//
//  Seg head
//  数据类型为Bytes有效
//
struct SegHead {
    uint16_t        key;

    //  DTYPE_BYTES
    unsigned char   data_type;

    //  Real data length
    uint32_t        data_len;

    SegHead(){
        key = 0;
        data_type = 0;
        data_len = 0;
    }
};

#ifdef WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif //

//
//  Data type,
//  1 byte, 2 bytes, 4bytes, 8bytes, string or bytes data.
//
enum ENUM_DATA_TYPE_VAL {
    DTYPE_UCHAR = 0,
    DTYPE_USHORT16 = 1,
    DTYPE_UINT32 = 2,
    DTYPE_UINT64 = 3,
    DTYPE_BYTES = 4
};
typedef unsigned char  ENUM_DATA_TYPE;

//
//  Class CBaseProtocolPack
//  Version:1.1
//
template<typename T>
class CBaseProtocolPack_ {
public:
    CBaseProtocolPack_(const unsigned char* buff, const uint32_t buff_len);
    CBaseProtocolPack_();

    //
    //  设置buffer, 并标明已经打好包的长度
    //
    CBaseProtocolPack_(const unsigned char* buff, const uint32_t buff_len,
    const uint32_t valid_pack_len);
    ~CBaseProtocolPack_();

    void Init();
    void Uninit();
    void SetOption(ENUM_PROTOCOL_OPT opt, const bool enable);
    void SetTTL(const unsigned char TTL);
    void SetSeq(const uint32_t seq);
    bool SetReservedData(const unsigned char* data, const uint16_t len);
    void SetForceSkipCRC(const bool skip);

    void ResetContent();
    void SetServiceType(const uint16_t service);
    void SetUserData(const uint64_t user_data);
    bool SetKey(uint16_t key, const unsigned char val);
    bool SetKey(uint16_t key, const char val);
    bool SetKey(uint16_t key, const int16_t val);
    bool SetKey(uint16_t key, const uint16_t val);
    bool SetKey(uint16_t key, const int32_t val);
    bool SetKey(uint16_t key, const uint32_t val);
    bool SetKey(uint16_t key, const float val);
    bool SetKey(uint16_t key, const double val);
    bool SetKey(uint16_t key, const int64_t val);
    bool SetKey(uint16_t key, const uint64_t val);
    bool SetKey(uint16_t key, const char* val); // 字符串
    bool SetKey(uint16_t key, char* val);
    bool SetKey(uint16_t key, unsigned char* val);
    bool SetKey(uint16_t key, const unsigned char* data, // 1:当字符串插入, len + 1;
                                                         // 2:当Bytes*二进制, 长度不要 + 1
                const uint32_t len);

    //
    // 支持自定义类型, SetKey
    //
    template<typename T_ValType>
    bool SetKey(uint16_t key, const T_ValType* val);

    //
    // 在最后一个Key后面追加数据(key必须和上次SetKey的key相同)
    //
    bool AppendKeyData(uint16_t key, const unsigned char* data, const uint32_t len);

    //
    // 获取打包后结果
    //
    void GetPackage(unsigned char** package, uint32_t* len);

    //
    // 获取打包后结果
    // 和上面接口完全一致,使用这个接口更安全
    //
    void GetPackage(const char** package, uint32_t* len);

    //
    // 获取只对单个大的数据块打包后的头部数据
    //
    void GetPackedHeadOfBlock(uint16_t service, uint16_t key, uint32_t block_len,
                              unsigned char** head, uint32_t* head_len);

private:
    bool    CheckBufferLen(uint32_t new_data_len);
    bool    SetKeyVal(ENUM_DATA_TYPE type, uint16_t key,
                      const unsigned char* data, uint32_t len);
    void    GetPackageImpl(char** package, uint32_t* len);

    bool            m_use_internal_buff;
    unsigned char*  m_pack_buff;
    uint32_t        m_pack_buff_len;
    BasProtocolHead m_head;

    //
    // 最后一次设置数据的key和数据类型, 用于AppendKeyData
    //
    uint16_t        m_last_key;
    ENUM_DATA_TYPE  m_last_key_type;

    //
    // 最后一个Key数据seg开始位置
    //
    uint32_t        m_last_key_pos;

    //
    // 打包后的头部数据块, 用于打包单个大数据块
    // BasProtocolHead+key(ushort)+data type(uchar)+data len(uint32)
    //
    unsigned char   m_single_block_pack_head[sizeof(BasProtocolHead)+7];
};
typedef CBaseProtocolPack_<void> CBaseProtocolPack;

//
//  Class CBaseProtocolUnpack
//
template<typename T>
class CBaseProtocolUnpack_ {
public:
    CBaseProtocolUnpack_();
    virtual ~CBaseProtocolUnpack_();

    bool            Init();
    void            Uninit();
    void            AttachPackage(const unsigned char* pack, uint32_t len);
    void            SetForceSkipCRC(bool skip);
    bool            Unpack();
    uint32_t        GetTTL();
    uint32_t        GetSeq();
    uint16_t        GetServiceType();
    uint64_t        GetUserData();
    uint32_t        GetOption();
    bool            GetReservedData(unsigned char** reserved, uint32_t* len);
    ENUM_DATA_TYPE  GetDataType(uint16_t key, bool* in_use);
    bool            GetVal(uint16_t key, unsigned char* val);
    bool            GetVal(uint16_t key, char* val);
    bool            GetVal(uint16_t key, int16_t* val);
    bool            GetVal(uint16_t key, uint16_t* val);
    bool            GetVal(uint16_t key, int32_t* val);
    bool            GetVal(uint16_t key, uint32_t* val);
    bool            GetVal(uint16_t key, float* val);
    bool            GetVal(uint16_t key, double* val);
    bool            GetVal(uint16_t key, int64_t* val);
    bool            GetVal(uint16_t key, uint64_t* val);
    bool            GetVal(uint16_t key, char** val, uint32_t* len);
    bool            GetVal(uint16_t key, unsigned char** val, uint32_t* len);

    //
    // 建议作获取自定义数据类型使用
    // 获取单个数据项
    //
    template<typename T_ValType>
    bool            GetVal(uint16_t key, T_ValType** val);

    //
    // 获取自定义数据项数组
    //
    template<typename T_ValType>
    bool            GetValArray(uint16_t key, T_ValType** val,
    int32_t* array_items);

    void            UntachPackage();

    // 每次GetVal()前调用IsTryGetVal(),则GetVal()失败不会输出错误日志
    // 只影响最近一次GetVal()错误日志输出
    void            SetIsTryGetVal();

protected:
    bool            GetKeyVal(uint16_t key,
    ENUM_DATA_TYPE type,
    void** val,
    uint32_t* len);

    struct SegVal
    {
        uint32_t        seq;
        unsigned char   type;
        uint32_t        len;
        uint32_t        data_offset;
    };

    uint32_t        m_seq;
    SegVal*         m_seg_vals;
    bool            m_is_net_order;

    unsigned char*  m_reference_data;
    uint32_t        m_ref_data_len;

    //
    // Force skip CRC check
    //
    bool            m_force_skip_crc;

    // 多线程对象抢占debug
    int32_t         m_last_thread_id;
    void            CheckTID();

    // 用于判定本次getval是try还是必须获取,只影响getval失败时候的日志输出
    // 每次GetVal()前调用IsTryGetVal(),则GetVal()失败不会输出错误日志
    // 只在debug模式下有效
    bool            m_is_try_getval;
};
typedef CBaseProtocolUnpack_<void> CBaseProtocolUnpack;

//#define CHECK_TID CheckTID();
#define CHECK_TID

inline unsigned char CalCRC8(const unsigned char* data, uint32_t len);

//
// 可累进式CRC32, 移植于Winzip, PKZip等算法
//
inline uint32_t GetCRC32(uint32_t old_crc,
                         const unsigned char *data,
                         uint32_t len);

//
// 1:强行修改已经打好包包头中序列号
// 2:也可以通过打包前设置BasProtocolHead.SequenceNum来完成
//
inline void ModifySeqNum(unsigned char* pack,
                         uint32_t len,
                         uint32_t new_seq);

//
// 1:强行修改已经打好包包头中的用户数据
// 2:也可以通过打包前设置BasProtocolHead.user_data来完成
//
inline void ModifyUserDataInPack(unsigned char* pack,
                                 uint32_t len,
                                 uint32_t new_userdata);


//
// 1:从已经打包的数据块中获取序列号
// 2:也可以从BasProtocolHead.SequenceNum获取
//
inline bool GetSeqNumInPack(const char* pack,
                            uint32_t len,
                            uint32_t* seq);

//
// 从已经打包的数据块中获取包类型，请求包or响应包
//
inline bool GetPackageTypeInPack(const char* pack,
                                 uint32_t len,
                                 uint32_t* pkg_type);

//
// 1:Make seg head(具体包内数据seg的头部)
// 2:pRefPackage, 已经打好包的参考包(主要获取是否进行网络字节序转换信息)
//
inline void MakeSegHead(SegHead* seg_head,
                        uint16_t key,
                        uint32_t data_len,
                        unsigned char* ref_pack);

//
// 1:修改包头，增加长度(用于尾部追加发送大数据seg),
//             增加的长度为sizeof(SegHead)+DataLen
// 2:包CRC32校验会无效
//
inline void ModifyPackageLength(uint32_t append_len,
                                unsigned char* ref_pack);

// 强制修改包 pkg 的长度为 new_pkg_len
inline void ModifyPackageLength(unsigned char* pkg, uint32_t new_pkg_len);

// 强制修改包 pkg 包头中表示数据段开始的偏移值
inline void ModifyProtocolHeadFristSegPos(uint64_t new_first_seg_pos,
                                          unsigned char* pkg);


//
// 检查是否有效的数据包, 针对扫描策略, 在头部增加自校验
//
bool IsValidPack(BasProtocolHead* head_with_net_order);

//
//
//
inline const char* GetDataTypeStr(unsigned char type) {
    switch (type) {
    case DTYPE_UCHAR:
        return "DTYPE_UCHAR";
    case DTYPE_USHORT16:
        return "DTYPE_USHORT16";
    case DTYPE_UINT32:
        return "DTYPE_UINT32";
    case DTYPE_UINT64:
        return "DTYPE_UINT64";
    case DTYPE_BYTES:
        return "DTYPE_BYTES(bytes, string, or user define data type)";
    default:
        return "--unknpwn data type--";
    }
}

//
// FAQ
// 1:如何分段发送超大数据包
//      ----------------------------------------------------
//      先发送部分数据的协议包，再发送超大数据段
//      1:
//      CBaseProtocolPack pack;
//      pack.Init();
//      pack.SetServiceType(xx);
//      pack.SetKey(key1, xx);
//      pack.SetKey(key2, xx);
//
//      TBYTE* pack_data = NULL;
//      uint32_t pack_len = 0;
//      pack.GetPackage(&pack_data, &pack_len);
//
//      // 2: make seg head
//      SegHead seg_head;
//      MakeSegHead(&seg_head, Key3, Key3_data_len, pack_data);
//
//      // 3: update package length
//      ModifyPackageLength(sizeog(SegHead) + Key3_data_len, pack_data);
//
//      // 4: send package
//      send(pack_data, pack_len);
//      send(&seg_head, sizeof(SegHead));
//      send(key3_data, Key3_data_len);
//
//      Server side
//      if (1st_time)
//      {
//          if (!IsValidPack((BasProtocolHead*) recv_pack_data, recv_len))
//              return ErrCode;  // error
//      }
//      ----------------------------------------------------
//

#include "base_protocol_t.inl"

//_END_XFS_BASE_NAMESPACE_
} // namespace base
} // namespace xfs

#endif // LONGCONN_BASE_PROTOCOL_H_

