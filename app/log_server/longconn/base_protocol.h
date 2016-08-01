// base_protocol.h: interface for the BaseProtocol class.
// wookin@tencent.com
// 2007-05-11
//
// ˵��:ͨ��Э����, �ӽ��
//        1.ʹ��key-value��ģ��
//
//        2.֧�ֵ���������
//                          1bytes(char/unsigned char),=
//                          2bytes(SHORT16/USHORT16),
//                          4bytes(INT32/UINT32),
//                          n bytes(BYTES)
//
//        3.�ֽ���ת��������趨BASEPROTOCOL_OPT_NETORDER���Զ����ֽ���ת����
//          Ӧ�ò㲻��ת��, BYTES�����ֽ�����Ҫ�û����д���.
//
//        4.�û��������ͣ������Լ�����ṹ��BYTES�������ݴ���
//
//        5.����������Դ���, �ڷ���˵�һ����ĳ�����������ݵ�ʱ��, ����������
//          ���ڵ���sizeof(BasProtocolHead), ����IsValidPack()����Ƿ���ȷ��
//          ���ݰ���
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
//        | *data len*    4B (type = Bytes����)|
//        | data...[len = data len]            |
//        +------------------------------------+
// ////////////////////////////////////////////////////////////////////

//
// update list
// ___________________________
// data        by     detail
// 2010-07-28  wookin ֧���Զ�����������, GetKeyʱ����˫���������ʹ�С�Ƿ�һ��
// 2011-02-17  lonely ֧��˫�����ݴ��䣬һ����·��һ�����Է�������Ҳ���Է�����Ӧ
// 2011-02-19  loney  ��ͷ����û������ֶ�

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
    BASEPROTOCOL_OPT_CRC_CHECK          = 0x02,   // �����ҪCRC check, ��Option
    // ��λһ��Byte����CRC8���,
    // ����CRC8��ȡHead����

    BASEPROTOCOL_OPT_KEEPALIVE          = 0x04,   // KeepAlive = 1, ������,
    // ������ɶ������

    BASEPROTOCOL_OPT_TRANS_PIPELINING   = 0x08,   // ����п���, �뽫�����ݰ�����
    // ��pipelining(��ˮ��)��ʽת��

    BASEPROTOCOL_OPT_IS_PIPELINING_HEAD = 0x10,   // ��BASEPROTOCOL_OPT_TRANS_
    // PIPELINING����,
    // ���ڸ�ѡ����˵�������������
    // pipelining head
    //
    // ����3��optionֻ����server��Ҫ������client�������ݰ��ǲ���Ҫ��Ӧ����������
    //
    BASEPROTOCOL_OPT_IS_REQ_PACKAGE    = 0x0100, // �����
    BASEPROTOCOL_OPT_IS_RSP_PACKAGE    = 0x0200, // ��Ӧ��
    BASEPROTOCOL_OPT_IS_DEFAULT_PACKAGE= 0x0400, // ȱʡ������,��������Ӧ����
    // accept��connect��ϵȷ��
};

enum ENUM_PROTOCOL_VER {
    //
    // ENUM_PROTOCOL_VER_1 = 1,
    //
    ENUM_PROTOCOL_VER_2 = 2,                    //  ����Э��ͷ����У��,
    // Ӧ��ɨ�����
};

//
// Checksum�㷨��ʼ��key,
// key��ͬ��ͬ�Ľ����õ���ͬ��checksum
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

// for safe_rand(),�̰߳�ȫrand
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
    // ������Դ���,
    // Ӧ�Ա�ĳ���ɨ�����
    // ÿ�ζ���ͬ�������, ����ĳЩ���ܵ�cache����
    uint32_t        random_num;

    //
    // checksum of head
    // ���ֶ�֮ǰ�������ֶε�CRC32()���
    //
    uint32_t        head_checksum;

    //
    // checksum of body
    // �����checksum
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
//  ��������ΪBytes��Ч
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
    //  ����buffer, �������Ѿ���ð��ĳ���
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
    bool SetKey(uint16_t key, const char* val); // �ַ���
    bool SetKey(uint16_t key, char* val);
    bool SetKey(uint16_t key, unsigned char* val);
    bool SetKey(uint16_t key, const unsigned char* data, // 1:���ַ�������, len + 1;
                                                         // 2:��Bytes*������, ���Ȳ�Ҫ + 1
                const uint32_t len);

    //
    // ֧���Զ�������, SetKey
    //
    template<typename T_ValType>
    bool SetKey(uint16_t key, const T_ValType* val);

    //
    // �����һ��Key����׷������(key������ϴ�SetKey��key��ͬ)
    //
    bool AppendKeyData(uint16_t key, const unsigned char* data, const uint32_t len);

    //
    // ��ȡ�������
    //
    void GetPackage(unsigned char** package, uint32_t* len);

    //
    // ��ȡ�������
    // ������ӿ���ȫһ��,ʹ������ӿڸ���ȫ
    //
    void GetPackage(const char** package, uint32_t* len);

    //
    // ��ȡֻ�Ե���������ݿ������ͷ������
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
    // ���һ���������ݵ�key����������, ����AppendKeyData
    //
    uint16_t        m_last_key;
    ENUM_DATA_TYPE  m_last_key_type;

    //
    // ���һ��Key����seg��ʼλ��
    //
    uint32_t        m_last_key_pos;

    //
    // ������ͷ�����ݿ�, ���ڴ�����������ݿ�
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
    // ��������ȡ�Զ�����������ʹ��
    // ��ȡ����������
    //
    template<typename T_ValType>
    bool            GetVal(uint16_t key, T_ValType** val);

    //
    // ��ȡ�Զ�������������
    //
    template<typename T_ValType>
    bool            GetValArray(uint16_t key, T_ValType** val,
    int32_t* array_items);

    void            UntachPackage();

    // ÿ��GetVal()ǰ����IsTryGetVal(),��GetVal()ʧ�ܲ������������־
    // ֻӰ�����һ��GetVal()������־���
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

    // ���̶߳�����ռdebug
    int32_t         m_last_thread_id;
    void            CheckTID();

    // �����ж�����getval��try���Ǳ����ȡ,ֻӰ��getvalʧ��ʱ�����־���
    // ÿ��GetVal()ǰ����IsTryGetVal(),��GetVal()ʧ�ܲ������������־
    // ֻ��debugģʽ����Ч
    bool            m_is_try_getval;
};
typedef CBaseProtocolUnpack_<void> CBaseProtocolUnpack;

//#define CHECK_TID CheckTID();
#define CHECK_TID

inline unsigned char CalCRC8(const unsigned char* data, uint32_t len);

//
// ���۽�ʽCRC32, ��ֲ��Winzip, PKZip���㷨
//
inline uint32_t GetCRC32(uint32_t old_crc,
                         const unsigned char *data,
                         uint32_t len);

//
// 1:ǿ���޸��Ѿ���ð���ͷ�����к�
// 2:Ҳ����ͨ�����ǰ����BasProtocolHead.SequenceNum�����
//
inline void ModifySeqNum(unsigned char* pack,
                         uint32_t len,
                         uint32_t new_seq);

//
// 1:ǿ���޸��Ѿ���ð���ͷ�е��û�����
// 2:Ҳ����ͨ�����ǰ����BasProtocolHead.user_data�����
//
inline void ModifyUserDataInPack(unsigned char* pack,
                                 uint32_t len,
                                 uint32_t new_userdata);


//
// 1:���Ѿ���������ݿ��л�ȡ���к�
// 2:Ҳ���Դ�BasProtocolHead.SequenceNum��ȡ
//
inline bool GetSeqNumInPack(const char* pack,
                            uint32_t len,
                            uint32_t* seq);

//
// ���Ѿ���������ݿ��л�ȡ�����ͣ������or��Ӧ��
//
inline bool GetPackageTypeInPack(const char* pack,
                                 uint32_t len,
                                 uint32_t* pkg_type);

//
// 1:Make seg head(�����������seg��ͷ��)
// 2:pRefPackage, �Ѿ���ð��Ĳο���(��Ҫ��ȡ�Ƿ���������ֽ���ת����Ϣ)
//
inline void MakeSegHead(SegHead* seg_head,
                        uint16_t key,
                        uint32_t data_len,
                        unsigned char* ref_pack);

//
// 1:�޸İ�ͷ�����ӳ���(����β��׷�ӷ��ʹ�����seg),
//             ���ӵĳ���Ϊsizeof(SegHead)+DataLen
// 2:��CRC32У�����Ч
//
inline void ModifyPackageLength(uint32_t append_len,
                                unsigned char* ref_pack);

// ǿ���޸İ� pkg �ĳ���Ϊ new_pkg_len
inline void ModifyPackageLength(unsigned char* pkg, uint32_t new_pkg_len);

// ǿ���޸İ� pkg ��ͷ�б�ʾ���ݶο�ʼ��ƫ��ֵ
inline void ModifyProtocolHeadFristSegPos(uint64_t new_first_seg_pos,
                                          unsigned char* pkg);


//
// ����Ƿ���Ч�����ݰ�, ���ɨ�����, ��ͷ��������У��
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
// 1:��ηֶη��ͳ������ݰ�
//      ----------------------------------------------------
//      �ȷ��Ͳ������ݵ�Э������ٷ��ͳ������ݶ�
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

