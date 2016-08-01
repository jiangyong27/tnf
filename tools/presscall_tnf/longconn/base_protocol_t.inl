// base_protocol_t.inl: implementation of the CBaseProtocolPack_
//                                            CBaseProtocolUnpack_
// wookin@tencent.com
// 2007-05-11
//////////////////////////////////////////////////////////////////////

#ifndef LONGCONN_BASE_PROTOCOL_T_INL_
#define LONGCONN_BASE_PROTOCOL_T_INL_

// ============================================================================
// class CBaseProtocolPack_
template<typename T>
void SetOption(uint32_t& old_val, xfs::base::ENUM_PROTOCOL_OPT opt, const bool enable){
    uint32_t val = opt;
    if (enable) {
        old_val = (old_val | val);
    } else {
        val = ~val;
        old_val = (old_val & val);
    }
}

/*
extern CXThreadMutex g_pack_seq_mutex;
// 打包时，序号自加
extern uint32_t volatile g_auto_increase_seq;
inline uint32_t GetUniquePackSeq() {
    CXThreadAutoLock locker(&g_pack_seq_mutex);
    ++g_auto_increase_seq;
    return g_auto_increase_seq;
}
*/

//  初始化CBaseProtocolPack_,进行一些默认设置
//
//  默认使用网络字节序列
//  默认使用accept和connect关系确定的单向问答式流式传输
//  自动维护序列号,
//  用户调用ResetContent()以后自动设置,
//  如果用户调用SetSeq(),
//  则使用用户设定的SeqNum
#define BASE_PROTOCOL_INIT m_last_key = 0; \
    m_last_key_type = 0;    \
    m_last_key_pos = 0; \
    xfs::base::SetOption<void>(m_head.option, BASEPROTOCOL_OPT_NETORDER, true); \
    xfs::base::SetOption<void>(m_head.option, BASEPROTOCOL_OPT_IS_DEFAULT_PACKAGE, true); \
    srand((uint32_t)time(0));

template<typename T>
CBaseProtocolPack_<T>::CBaseProtocolPack_(const unsigned char* buff, const uint32_t buff_len) {
    memset(m_single_block_pack_head, 0, sizeof(m_single_block_pack_head));

    m_use_internal_buff = false;
    m_pack_buff = buff;
    m_pack_buff_len = buff_len;

    BASE_PROTOCOL_INIT;
}

template<typename T>
CBaseProtocolPack_<T>::CBaseProtocolPack_(const unsigned char* buff,
        const uint32_t buff_len,
        const uint32_t valid_pack_len) {
    memset(m_single_block_pack_head, 0, sizeof(m_single_block_pack_head));

    m_use_internal_buff = false;
    m_pack_buff = buff;
    m_pack_buff_len = buff_len;

    BASE_PROTOCOL_INIT;

    //  设置已经打包好的一段内存
    if (valid_pack_len >= (uint32_t)sizeof(BasProtocolHead))
        memcpy(&m_head, buff, sizeof(BasProtocolHead));
}

template<typename T>
CBaseProtocolPack_<T>::CBaseProtocolPack_() {
    memset(m_single_block_pack_head, 0, sizeof(m_single_block_pack_head));

    m_use_internal_buff = true;
    m_pack_buff = NULL;
    m_pack_buff_len = 0;

    BASE_PROTOCOL_INIT;
}

template<typename T>
CBaseProtocolPack_<T>::~CBaseProtocolPack_() {
    Uninit();
}

template<typename T>
void CBaseProtocolPack_<T>::SetOption(ENUM_PROTOCOL_OPT opt, const bool enable) {
    xfs::base::SetOption<void>(m_head.option, opt, enable);
}

template<typename T>
void CBaseProtocolPack_<T>::Init() {
}

template<typename T>
void CBaseProtocolPack_<T>::Uninit() {
    if (m_use_internal_buff && m_pack_buff) {
        delete []m_pack_buff;
        m_pack_buff = 0;
        m_pack_buff_len = 0;
    }
}

template<typename T>
void CBaseProtocolPack_<T>::SetTTL(const unsigned char TTL) {
    m_head.TTL = TTL;
}

template<typename T>
void CBaseProtocolPack_<T>::SetSeq(const uint32_t seq) {
    m_head.sequence = seq;
}

template<typename T>
void CBaseProtocolPack_<T>::SetForceSkipCRC(const bool skip) {
    SetOption(BASEPROTOCOL_OPT_CRC_CHECK, !skip);
}

template<typename T>
bool    CBaseProtocolPack_<T>::CheckBufferLen(uint32_t new_data_len) {
    bool b = false;
    if (m_use_internal_buff) {
        if (m_head.length + new_data_len > m_pack_buff_len) {
            uint32_t new_len = m_head.length + new_data_len+256;
            unsigned char* buff = new unsigned char[new_len];
            if (buff) {
                if (m_pack_buff) {
                    memcpy(buff, m_pack_buff, m_head.length);
                    delete []m_pack_buff;
                }
                m_pack_buff = buff;
                m_pack_buff_len = new_len;
                b = true;
            } else {
                b = false;
                //LOG(WARNING) << "CBaseProtocolPack_::CheckBufferLen(), out of memory.";
            }
        } else
            b = true;
    } else {
        if (m_head.length + new_data_len <= m_pack_buff_len)
            b = true;
    }
    return b;
}

template<typename T>
bool CBaseProtocolPack_<T>::SetReservedData(const unsigned char* reserved,
        const uint16_t len) {
    bool b = false;
    if (reserved && len && CheckBufferLen(len)) {
        m_head.length = (uint32_t)sizeof(BasProtocolHead);
        memcpy(m_pack_buff + m_head.length, reserved, len);
        m_head.length += len;
        m_head.first_seg_pos = (uint16_t)m_head.length;
        b = true;
    }
    return b;
}

template<typename T>
void CBaseProtocolPack_<T>::ResetContent() {
    // 默认使用accept和connect关系确定的单向问答式流式传输
    SetOption(BASEPROTOCOL_OPT_IS_REQ_PACKAGE, false);
    SetOption(BASEPROTOCOL_OPT_IS_RSP_PACKAGE, false);
    SetOption(BASEPROTOCOL_OPT_IS_DEFAULT_PACKAGE, true);

    m_head.length = (uint32_t)sizeof(BasProtocolHead);
    m_head.first_seg_pos = (uint16_t)m_head.length;

    // auto seq number
    // m_head.sequence = GetUniquePackSeq();
    m_head.sequence = 0;
}

template<typename T>
void CBaseProtocolPack_<T>::SetServiceType(const uint16_t service) {
    m_head.service_type = service;
}

template<typename T>
void CBaseProtocolPack_<T>::SetUserData(const uint64_t user_data) {
    m_head.user_data = user_data;
}

template<typename T>
bool    CBaseProtocolPack_<T>::SetKeyVal(ENUM_DATA_TYPE type,
        uint16_t key,
        const unsigned char* data,
        uint32_t len) {
    bool b = false;
    uint32_t new_block_len = 0;
    switch (type) {
    case DTYPE_UCHAR:
    case DTYPE_USHORT16:
    case DTYPE_UINT32:
    case DTYPE_UINT64:
        new_block_len = (uint32_t)sizeof(key)+
                        1       //  data type
                        +len;   //  data
        break;
    case DTYPE_BYTES:
        len = (data == 0) ? 0 : len;              //  invalid length
        new_block_len = (uint32_t)sizeof(key) +
                        1                                   //  data type
                        +(uint32_t)sizeof(uint32_t)+        //  data length
                        len;                                //  data
        break;
    default:
        break;
    }

    // 打包长度为0的串
    if (CheckBufferLen(new_block_len)) {
        bool use_net_order =
            ((m_head.option & BASEPROTOCOL_OPT_NETORDER) ==
             BASEPROTOCOL_OPT_NETORDER) ?
            true :
            false;

        //  save last key info
        m_last_key = key;
        m_last_key_type = type;
        m_last_key_pos = m_head.length;

        //  seg name
        uint16_t seg_name = key;
        if (use_net_order)
            seg_name = htons(seg_name);
        memcpy(m_pack_buff + m_head.length, &seg_name, sizeof(seg_name));
        m_head.length += (uint32_t)sizeof(seg_name);

        //  data type
        memcpy(m_pack_buff + m_head.length, &type, sizeof(unsigned char));
        m_head.length += (uint32_t)sizeof(unsigned char);

        //  data length
        if (type == DTYPE_BYTES) {
            uint32_t data_len = len;
            if (use_net_order)
                data_len = htonl(data_len);
            memcpy(m_pack_buff + m_head.length, &data_len, sizeof(data_len));
            m_head.length += (uint32_t)sizeof(data_len);
        }

        //  real data
        if (type == DTYPE_USHORT16) {
            uint16_t short_real_data = 0;
            memcpy(&short_real_data, data, sizeof(uint16_t));
            if (use_net_order)
                short_real_data = htons(short_real_data);
            memcpy(m_pack_buff + m_head.length, &short_real_data,
                   sizeof(short_real_data));
            m_head.length += (uint32_t)sizeof(short_real_data);
        } else if (type == DTYPE_UINT32) {
            uint32_t num_real_data = 0;
            memcpy(&num_real_data, data, sizeof(uint32_t));
            if (use_net_order)
                num_real_data = htonl(num_real_data);
            memcpy(m_pack_buff + m_head.length, &num_real_data,
                   sizeof(num_real_data));
            m_head.length += (uint32_t)sizeof(num_real_data);
        } else if (type == DTYPE_UINT64) {
            uint64_t long_real_data = 0;
            memcpy(&long_real_data, data, sizeof(uint64_t));
            if (use_net_order)
                long_real_data = hton64(long_real_data);
            memcpy(m_pack_buff + m_head.length, &long_real_data,
                   sizeof(uint64_t));
            m_head.length += (uint32_t)sizeof(uint64_t);
        } else {
            memcpy(m_pack_buff + m_head.length, data, len);
            m_head.length += len;
        }
        b = true;
    }
    return b;
}

template<typename T>
bool CBaseProtocolPack_<T>::AppendKeyData(uint16_t key,
        const unsigned char* data,
        const uint32_t len) {
    bool b = false;
    uint32_t new_buff_len = len;
    if (data && len &&
            m_last_key == key &&
            m_last_key_type == DTYPE_BYTES &&
            CheckBufferLen(new_buff_len) &&
            m_head.length < m_pack_buff_len) {
        bool use_net_order = ((m_head.option & BASEPROTOCOL_OPT_NETORDER) ==
                              BASEPROTOCOL_OPT_NETORDER) ?
                             true :
                             false;
        // get data length
        uint32_t data_len = 0;
        memcpy(&data_len, m_pack_buff+m_last_key_pos+2+1, 4);
        if (use_net_order)
            data_len = ntohl(data_len);

        // append data and update m_stHead.Length
        memcpy(m_pack_buff+m_last_key_pos+2+1+4+data_len, data, len);
        data_len += len;
        m_head.length += len;

        // update data length
        if (use_net_order)
            data_len = htonl(data_len);
        memcpy(m_pack_buff + m_last_key_pos+2+1, &data_len, sizeof(uint32_t));
        b = true;
    }
    return b;
}

template<typename T>
void CBaseProtocolPack_<T>::GetPackedHeadOfBlock(uint16_t service_type,
        uint16_t key,
        uint32_t block_len,
        unsigned char** head,
        uint32_t* head_len) {
    BasProtocolHead* ptr_head =
        reinterpret_cast<BasProtocolHead*>(m_single_block_pack_head);

    memcpy(ptr_head, &m_head, sizeof(BasProtocolHead));

    // 不使用CRC check
    uint32_t opt = BASEPROTOCOL_OPT_CRC_CHECK;
    opt = ~opt;
    ptr_head->option &= opt;
    ptr_head->first_seg_pos = (uint32_t)sizeof(BasProtocolHead);
    ptr_head->service_type = service_type;

    unsigned char* ptr = ((unsigned char*)m_single_block_pack_head) +
                         (uint32_t)sizeof(BasProtocolHead);

    // set key
    uint16_t netorder_key = htons(key);
    memcpy(ptr, &netorder_key, sizeof(uint16_t));
    ptr += (uint32_t)sizeof(netorder_key);

    // set data type
    ENUM_DATA_TYPE type = DTYPE_BYTES;
    memcpy(ptr, &type, sizeof(ENUM_DATA_TYPE));
    ptr += (uint32_t)sizeof(type);

    // set block length
    uint32_t netorder_len = htonl(block_len);
    memcpy(ptr, &netorder_len, sizeof(uint32_t));
    // ptr += sizeof();

    // update length
    ptr_head->length = sizeof(BasProtocolHead)+7+block_len;
    ptr_head->ToNetOrder();

    // head+part of first seg. length
    *head_len = (uint32_t)sizeof(BasProtocolHead)+7;
    *head = (unsigned char*)ptr_head;
}


template<typename T>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key, const unsigned char val) {
    return SetKeyVal(DTYPE_UCHAR, key, &val, sizeof(val));
}

template<typename T>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key, const char val) {
    return SetKeyVal(DTYPE_UCHAR, key, (unsigned char*)&val, sizeof(val));
}

template<typename T>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key, const int16_t val) {
    return SetKeyVal(DTYPE_USHORT16, key, (unsigned char*)&val, sizeof(val));
}

template<typename T>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key, const uint16_t val) {
    return SetKeyVal(DTYPE_USHORT16, key, (unsigned char*)&val, sizeof(val));
}

template<typename T>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key, const int32_t val) {
    return SetKeyVal(DTYPE_UINT32, key, (unsigned char*)&val, sizeof(val));
}

template<typename T>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key, const uint32_t val) {
    return SetKeyVal(DTYPE_UINT32, key, (unsigned char*)&val, sizeof(val));
}

template<typename T>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key, const float val) {
    return SetKeyVal(DTYPE_UINT32, key, (unsigned char*)&val, sizeof(val));
}

template<typename T>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key, const double val) {
    return SetKeyVal(DTYPE_UINT64, key, (unsigned char*)&val, sizeof(val));
}

template<typename T>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key, const int64_t val) {
    return SetKeyVal(DTYPE_UINT64, key, (unsigned char*)&val, sizeof(val));
}

template<typename T>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key, const uint64_t val) {
    return SetKeyVal(DTYPE_UINT64, key, (unsigned char*)&val, sizeof(val));
}

template<typename T>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key,
                                   const unsigned char* data,
                                   const uint32_t len) {
    return SetKeyVal(DTYPE_BYTES, key, data, len);
}

template<typename T>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key, const char* val) {
    return SetKey(key, (unsigned char*)val, STRLEN(val)+1);
}

template<typename T>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key, char* val) {
    return SetKey(key, (const char*)val);
}

template<typename T>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key, unsigned char* val) {
    return SetKey(key, (const char*)val);
}

//
// 支持自定义类型, SetKey
//
template<typename T>
template<typename T_ValType>
bool CBaseProtocolPack_<T>::SetKey(uint16_t key, const T_ValType* val) {
    return SetKey(key, (unsigned char*)val, sizeof(T_ValType));
}

template<typename T>
void CBaseProtocolPack_<T>::GetPackage(unsigned char** package, uint32_t* len) {
    GetPackageImpl(reinterpret_cast<char**>(package), len);
}

template<typename T>
void CBaseProtocolPack_<T>::GetPackage(const char** package, uint32_t* len){
    char* data = NULL;
    GetPackageImpl(&data, len);
    *package = (const char*)data;
}

template<typename T>
void CBaseProtocolPack_<T>::GetPackageImpl(char** package, uint32_t* len) {
    // 检查是是否用户一个key都不设置, 只使用包头
    if (m_use_internal_buff && !m_pack_buff) {
        uint32_t num_len = 2*((uint32_t)sizeof(BasProtocolHead));
        m_pack_buff = new unsigned char[num_len];
        if (m_pack_buff) {
            m_pack_buff_len = num_len;
        }

        //VLOG(3) << "m_pack_buff's address is 0x" <<
        //          reinterpret_cast<void*>(m_pack_buff) << ", m_stHead.Length is " <<
        //          m_head.length << ", sizeof(BasProtocolHead) is " <<
        //          static_cast<uint32_t>(sizeof(BasProtocolHead));
    }

    if (package && m_pack_buff) {
        // ? cal. CRC32 of body
        // 且包体存在
        if ((m_head.option & BASEPROTOCOL_OPT_CRC_CHECK) ==
                BASEPROTOCOL_OPT_CRC_CHECK &&
                m_head.length>sizeof(BasProtocolHead)
           ) {
            //
            // 跳过包头, 只计算包体的checksum, 包体数据已经是网络字节序
            //
            m_head.checksum = GetCRC32((uint32_t)BASEPROTO_HEAD_CHKSUM_KEY,
                m_pack_buff + sizeof(BasProtocolHead),
                m_head.length - (uint32_t)sizeof(BasProtocolHead));
        }

        m_head.random_num = safe_rand();
        // 计算头部CRC32

        //
        // TODO(wookin): 现在按本机字节序计算head checksum,
        //      后面需要修改为保证头部数据为
        //      网络字节序的情况下进行计算
        //
        // 不计算自己的最后8bytes(head_checksum+checksum)
        m_head.head_checksum = GetCRC32((uint32_t)BASEPROTO_HEAD_CHKSUM_KEY,
                                        (unsigned char*)&m_head,
                                        (uint32_t)(sizeof(m_head) -
                                                sizeof(uint32_t) -
                                                sizeof(uint32_t)));


        m_head.ToNetOrder();
        memcpy(m_pack_buff, &m_head, sizeof(BasProtocolHead));

        // convert to host order
        m_head.ToHostOrder();
        *package = (char*)m_pack_buff;
        if (len)
            *len = m_head.length;
    }
}

// 1:Make seg head(具体包内数据seg的头部)
// 2:pRefPackage, 已经打好包的参考包(主要获取是否进行网络字节序转换信息)
inline void MakeSegHead(SegHead* seg_head, uint16_t key, uint32_t data_len,
                        unsigned char* ref_pack) {
    if (seg_head && ref_pack) {
        seg_head->key = key;
        seg_head->data_type = DTYPE_BYTES;
        seg_head->data_len = data_len;
        BasProtocolHead* head = reinterpret_cast<BasProtocolHead*>(ref_pack);

        //  packed head always in net order
        uint32_t opt = ntohl(head->option);
        if ((opt & BASEPROTOCOL_OPT_NETORDER) == BASEPROTOCOL_OPT_NETORDER) {
            seg_head->key = htons(seg_head->key);
            seg_head->data_len = htonl(seg_head->data_len);
        }
    }
}

inline void ModifyPackageLength(uint32_t append_len, unsigned char* ref_pack) {
    if (ref_pack) {
        BasProtocolHead* ptr_head =
            reinterpret_cast<BasProtocolHead*>(ref_pack);

        // get package length, packed head always in net order
        uint32_t pack_len = ntohl(ptr_head->length);

        // append length
        pack_len += append_len;
        pack_len = htonl(pack_len);

        // update new length
        ptr_head->length = pack_len;
    }
}

inline void ModifyPackageLength(unsigned char* pkg, uint32_t new_pkg_len){
    if (pkg) {
        BasProtocolHead* ptr_head =
            reinterpret_cast<BasProtocolHead*>(pkg);
        // update new length
        ptr_head->length = htonl(new_pkg_len);
    }
}

inline void ModifyProtocolHeadFristSegPos(uint64_t new_first_seg_pos,
                                          unsigned char* pkg){
    if (pkg) {
        BasProtocolHead* ptr_head =
            reinterpret_cast<BasProtocolHead*>(pkg);
        // update new length
        ptr_head->first_seg_pos = htons(static_cast<uint16_t>(new_first_seg_pos));
    }
}

inline void ModifySeqNum(unsigned char* pack, uint32_t len, uint32_t new_seq) {
    if (pack && len >= sizeof(BasProtocolHead)) {
        BasProtocolHead* ptr_head = reinterpret_cast<BasProtocolHead*>(pack);
        if (ptr_head->ver == ENUM_PROTOCOL_VER_2)
            ptr_head->sequence = htonl(new_seq);
    }
}

inline void ModifyUserDataInPack(unsigned char* pack, uint32_t len,
                                 uint64_t new_userdata) {
    if (pack && len >= sizeof(BasProtocolHead)) {
        BasProtocolHead* ptr_head = reinterpret_cast<BasProtocolHead*>(pack);
        if (ptr_head->ver == ENUM_PROTOCOL_VER_2)
            ptr_head->user_data = hton64(new_userdata);
    }
}

inline bool GetSeqNumInPack(const char* pack,
                            uint32_t len, uint32_t* seq) {
    bool b = false;
    if (seq && pack && len >= sizeof(BasProtocolHead)) {
        const BasProtocolHead* ptr_head = reinterpret_cast<const BasProtocolHead*>(pack);

        if (ptr_head->ver == ENUM_PROTOCOL_VER_2) {
            *seq = ntohl(ptr_head->sequence);
            b = true;
        }
    }
    return b;
}

inline bool GetPackageTypeInPack(const char* pack,
                                 uint32_t len,
                                 uint32_t* pkg_type) {
    bool b = false;
    if (pkg_type && pack && len >= sizeof(BasProtocolHead)) {
        const BasProtocolHead* ptr_head = reinterpret_cast<const BasProtocolHead*>(pack);

        if (ptr_head->ver == ENUM_PROTOCOL_VER_2) {
            *pkg_type = ntohl(ptr_head->option);
            b = true;
        }
    }
    return b;
}

// =========================================================================
// class CBaseProtocolUnpack_
template<typename T>
CBaseProtocolUnpack_<T>::CBaseProtocolUnpack_() {
    m_seq = 1;
    m_seg_vals = NULL;
    m_is_net_order = false;

    m_reference_data = NULL;
    m_ref_data_len = 0;

    m_force_skip_crc = true;

    m_last_thread_id = 0;
    m_is_try_getval = false;

    m_seg_vals = new SegVal[65536]; // max. uint16_t
    memset(m_seg_vals, 0, sizeof(SegVal)*65536);
}

template<typename T>
CBaseProtocolUnpack_<T>::~CBaseProtocolUnpack_() {
    Uninit();
}

template<typename T>
bool CBaseProtocolUnpack_<T>::Init() {
    return true;
}

template<typename T>
void CBaseProtocolUnpack_<T>::CheckTID(){
    /*
    if(m_last_thread_id == 0){
        m_last_thread_id = GetTID();
    }else{
        int32_t tid = GetTID();
        if(tid != m_last_thread_id){
            //LOG(WARNING) << "multi threads call same object, init in thread:" << m_last_thread_id
            //           << ", now called by thread:" << tid;
            abort();
        }
    }
    */
}

template<typename T>
void CBaseProtocolUnpack_<T>::Uninit() {
    delete []m_seg_vals;
    m_seg_vals = 0;
}

template<typename T>
void CBaseProtocolUnpack_<T>::SetForceSkipCRC(const bool skip) {
    m_force_skip_crc = skip;
}

template<typename T>
void CBaseProtocolUnpack_<T>::AttachPackage(const unsigned char* pack, const uint32_t len) {
    CHECK_TID;
    m_reference_data = (unsigned char*)pack;
    m_ref_data_len = len;
}

template<typename T>
uint16_t CBaseProtocolUnpack_<T>::GetServiceType() {
    uint16_t service = 0;
    if (m_reference_data && m_ref_data_len &&
            m_ref_data_len >= sizeof(BasProtocolHead)) {
        BasProtocolHead* head =
            reinterpret_cast<BasProtocolHead*>(m_reference_data);

        service = ntohs(head->service_type);
    }
    return service;
}

template<typename T>
uint64_t CBaseProtocolUnpack_<T>::GetUserData() {
    uint64_t user_data = 0;
    if (m_reference_data && m_ref_data_len &&
            m_ref_data_len >= sizeof(BasProtocolHead)) {
        BasProtocolHead* head =
            reinterpret_cast<BasProtocolHead*>(m_reference_data);

        user_data = ntoh64(head->user_data);
    }
    return user_data;
}

template<typename T>
uint32_t CBaseProtocolUnpack_<T>::GetOption() {
    uint32_t opt = 0;
    if (m_reference_data && m_ref_data_len &&
            m_ref_data_len >= sizeof(BasProtocolHead)) {
        BasProtocolHead* head =
            reinterpret_cast<BasProtocolHead*>(m_reference_data);

        opt = ntohl(head->option);
    }
    return opt;
}

template<typename T>
uint32_t    CBaseProtocolUnpack_<T>::GetTTL() {
    uint32_t TTL = 0;
    if (m_reference_data && m_ref_data_len &&
            m_ref_data_len >= sizeof(BasProtocolHead)) {
        BasProtocolHead* head =
            reinterpret_cast<BasProtocolHead*>(m_reference_data);

        TTL = ntohl(head->TTL);
    }
    return TTL;
}

template<typename T>
uint32_t    CBaseProtocolUnpack_<T>::GetSeq() {
    uint32_t seq = 0;
    if (m_reference_data && m_ref_data_len &&
            m_ref_data_len >= sizeof(BasProtocolHead)) {
        BasProtocolHead* head =
            reinterpret_cast<BasProtocolHead*>(m_reference_data);

        seq = ntohl(head->sequence);
    }
    return seq;
}

template<typename T>
ENUM_DATA_TYPE    CBaseProtocolUnpack_<T>::GetDataType(uint16_t key,
        bool* in_use) {
    ENUM_DATA_TYPE type = DTYPE_UCHAR;
    if (m_seg_vals) {
        if (m_seg_vals[key].seq == m_seq && in_use)
            *in_use = true;
        type = (ENUM_DATA_TYPE)m_seg_vals[key].type;
    } else {
        //LOG(WARNING) << "***ERROR, Level 3***, "
        //           "GetDataType() fail, m_seg_vals is NULL.";
    }
    return type;
}

template<typename T>
bool    CBaseProtocolUnpack_<T>::GetReservedData(unsigned char** reserved,
        uint32_t* len) {
    bool b = false;
    if (m_reference_data && m_ref_data_len &&
            m_ref_data_len >= sizeof(BasProtocolHead)) {
        BasProtocolHead head;
        memcpy(&head, m_reference_data, sizeof(BasProtocolHead));
        head.ToHostOrder();
        if (reserved)
            *reserved = m_reference_data+sizeof(BasProtocolHead);
        if (len)
            *len = head.first_seg_pos-sizeof(BasProtocolHead);
        b = true;
    }
    return b;
}

template<typename T>
bool CBaseProtocolUnpack_<T>::Unpack() {
    CHECK_TID;
    bool b = false;
    if (m_reference_data && m_ref_data_len &&
            m_ref_data_len >= sizeof(BasProtocolHead) &&
            m_seg_vals) {
        BasProtocolHead head;
        memcpy(&head, m_reference_data, sizeof(head));
        head.ToHostOrder();

        // check version
        if (head.ver != ENUM_PROTOCOL_VER_2)
            return false;

        // ? need CRC check
        // ? cal. CRC32 of body
        // 包体存在才校验CRC32
        if (!m_force_skip_crc &&
                (head.option & BASEPROTOCOL_OPT_CRC_CHECK) ==
                BASEPROTOCOL_OPT_CRC_CHECK &&
                head.length>sizeof(BasProtocolHead)
           ) {
            uint32_t val_crc = GetCRC32(BASEPROTO_HEAD_CHKSUM_KEY,
                                        m_reference_data + sizeof(BasProtocolHead),
                                        head.length - (uint32_t)sizeof(BasProtocolHead));
            if (head.checksum != val_crc) {
                //   CRC check fail
                //LOG(WARNING) << "***ERROR, Level 3***, check CRC32 of body fail.";
                return false;
            }
        }

        uint32_t pos = head.first_seg_pos;
        m_is_net_order = ((head.option & BASEPROTOCOL_OPT_NETORDER) ==
                          BASEPROTOCOL_OPT_NETORDER);
        m_seq++;
        while (head.length > pos) {
            //  seg name
            uint16_t seg_name = 0;
            memcpy(&seg_name, m_reference_data + pos, sizeof(uint16_t));

            if (m_is_net_order)
                seg_name = ntohs(seg_name);
            m_seg_vals[seg_name].seq = m_seq;
            pos += (uint32_t)sizeof(uint16_t);

            //  data type
            unsigned char    type = 0;
            memcpy(&type, m_reference_data+pos, sizeof(unsigned char));
            m_seg_vals[seg_name].type = type;
            pos += (uint32_t)sizeof(unsigned char);

            //  data length
            //  real data
            switch (type) {
            case DTYPE_UCHAR:
                m_seg_vals[seg_name].len = 1;
                m_seg_vals[seg_name].data_offset = pos;
                pos += 1;
                break;
            case DTYPE_USHORT16:
                m_seg_vals[seg_name].len = 2;
                m_seg_vals[seg_name].data_offset = pos;
                pos += 2;
                break;
            case DTYPE_UINT32:
                m_seg_vals[seg_name].len = 4;
                m_seg_vals[seg_name].data_offset = pos;
                pos += 4;
                break;
            case DTYPE_UINT64:
                m_seg_vals[seg_name].len = 8;
                m_seg_vals[seg_name].data_offset = pos;
                pos += 8;
                break;
            case DTYPE_BYTES: {
                uint32_t real_len = 0;
                memcpy(&real_len,
                       m_reference_data + pos,
                       sizeof(uint32_t));

                if (m_is_net_order)
                    real_len = ntohl(real_len);
                pos += (uint32_t)sizeof(uint32_t);
                m_seg_vals[seg_name].len = real_len;
                m_seg_vals[seg_name].data_offset = pos;

                // real data
                pos += real_len;
            }
            break;
            default:
                break;
            }
        }
        b = true;
    } else {
        //LOG(WARNING) << "CBaseProtocolUnpack, Unpack(), "
        //           "invalid parameter or not Init()";
    }

    return b;
}

template<typename T>
bool CBaseProtocolUnpack_<T>::GetKeyVal(uint16_t key,
                                        ENUM_DATA_TYPE type,
                                        void** val,
                                        uint32_t* len) {
    CHECK_TID;
    bool b = false;
    if (val && m_reference_data && m_seg_vals) {
        if (m_seg_vals[key].seq == m_seq &&    //  ? valid item
                m_seg_vals[key].type == type) {    //  check data type
            switch (type) {
            case DTYPE_UCHAR:
                memcpy(val,
                       m_reference_data+m_seg_vals[key].data_offset,
                       sizeof(unsigned char));
                break;
            case DTYPE_USHORT16:
                memcpy(val,
                       m_reference_data+m_seg_vals[key].data_offset,
                       sizeof(uint16_t));
                if (m_is_net_order)
                    *reinterpret_cast<uint16_t*>(val) =
                        ntohs(*reinterpret_cast<uint16_t*>(val));
                break;
            case DTYPE_UINT32:
                memcpy(val,
                       m_reference_data+m_seg_vals[key].data_offset,
                       sizeof(uint32_t));
                if (m_is_net_order)
                    *reinterpret_cast<uint32_t*>(val) =
                        ntohl(*reinterpret_cast<uint32_t*>(val));
                break;
            case DTYPE_UINT64:
                memcpy(val,
                       m_reference_data+m_seg_vals[key].data_offset,
                       sizeof(uint64_t));
                if (m_is_net_order)
                    *reinterpret_cast<uint64_t*>(val) =
                        ntoh64(*reinterpret_cast<uint64_t*>(val));
                break;
            case DTYPE_BYTES:
                *val = reinterpret_cast<unsigned char*>(m_reference_data) +
                       m_seg_vals[key].data_offset;
                break;
            default:
                break;
            }
            if (len)
                *len = m_seg_vals[key].len;
            b = true;
        } else {
            if (m_seg_vals[key].seq == m_seq) {
                //LOG(WARNING) << "Get key value fail, data type mismatch. key = " << key
                //  << ", key-type:" << GetDataTypeStr(type) << ", received data type : "
                //  << GetDataTypeStr(m_seg_vals[key].type) << ", service type:" << GetServiceType();
            } else {
                // 如果是try get, 则不输出错误日志
                if(m_is_try_getval){
                    m_is_try_getval = false;
                }else{
                    //LOG(WARNING) << "Get key value fail, key data not exist, key = " << key
                    //           <<", service type:" << GetServiceType();
                }
            }
        }
    }
    return b;
}

template<typename T>
bool CBaseProtocolUnpack_<T>::GetVal(uint16_t key, unsigned char* val) {
    return GetKeyVal(key, DTYPE_UCHAR, reinterpret_cast<void**>(val), 0);
}

template<typename T>
bool CBaseProtocolUnpack_<T>::GetVal(uint16_t key, char* val) {
    return GetKeyVal(key, DTYPE_UCHAR, reinterpret_cast<void**>(val), 0);
}

template<typename T>
bool CBaseProtocolUnpack_<T>::GetVal(uint16_t key, int16_t* val) {
    return GetKeyVal(key, DTYPE_USHORT16, reinterpret_cast<void**>(val), 0);
}

template<typename T>
bool CBaseProtocolUnpack_<T>::GetVal(uint16_t key, uint16_t* val) {
    return GetKeyVal(key, DTYPE_USHORT16, reinterpret_cast<void**>(val), 0);
}

template<typename T>
bool CBaseProtocolUnpack_<T>::GetVal(uint16_t key, int32_t* val) {
    return GetKeyVal(key, DTYPE_UINT32, reinterpret_cast<void**>(val), 0);
}

template<typename T>
bool CBaseProtocolUnpack_<T>::GetVal(uint16_t key, uint32_t* val) {
    return GetKeyVal(key, DTYPE_UINT32, reinterpret_cast<void**>(val), 0);
}

template<typename T>
bool CBaseProtocolUnpack_<T>::GetVal(uint16_t key, float* val) {
    return GetKeyVal(key, DTYPE_UINT32, reinterpret_cast<void**>(val), 0);
}

template<typename T>
bool CBaseProtocolUnpack_<T>::GetVal(uint16_t key, double* val) {
    return GetKeyVal(key, DTYPE_UINT64, reinterpret_cast<void**>(val), 0);
}

template<typename T>
bool    CBaseProtocolUnpack_<T>::GetVal(uint16_t key, int64_t* val) {
    return GetKeyVal(key, DTYPE_UINT64, reinterpret_cast<void**>(val), 0);
}

template<typename T>
bool    CBaseProtocolUnpack_<T>::GetVal(uint16_t key, uint64_t* val) {
    return GetKeyVal(key, DTYPE_UINT64, reinterpret_cast<void**>(val), 0);
}

template<typename T>
bool CBaseProtocolUnpack_<T>::GetVal(uint16_t key,
                                     char** val,
                                     uint32_t* len) {
    return GetKeyVal(key, DTYPE_BYTES, reinterpret_cast<void**>(val), len);
}

template<typename T>
bool CBaseProtocolUnpack_<T>::GetVal(uint16_t key,
                                     unsigned char** val,
                                     uint32_t* len) {
    return GetKeyVal(key, DTYPE_BYTES, reinterpret_cast<void**>(val), len);
}

//
// 建议作获取自定义数据类型使用
// 获取单个数据项
//
template<typename T>
template<typename T_ValType>
bool CBaseProtocolUnpack_<T>::GetVal(uint16_t key, T_ValType** val) {
    CHECK_TID;
    bool b = false;
    void* ptr = 0;
    uint32_t len = 0;
    if (GetKeyVal(key, DTYPE_BYTES, &ptr, &len)) {
        if (len == sizeof(T_ValType)) {
            *val = reinterpret_cast<T_ValType*>(ptr);
            b = true;
        } else {
            //LOG(WARNING) << "get val fail, mismatch sizeof(data type). sizeof(T_ValType) : "
            //  << static_cast<int32_t>(sizeof(T_ValType)) << ", received data type len:" << len;
        }
    }
    return b;
}

//
// 建议作获取自定义数据类型使用
// 获取自定义数据项数组
//
template<typename T>
template<typename T_ValType>
bool CBaseProtocolUnpack_<T>::GetValArray(uint16_t key, T_ValType** val, int32_t* array_items) {
    CHECK_TID;
    bool b = false;
    void* ptr = 0;
    uint32_t len = 0;
    if (GetKeyVal(key, DTYPE_BYTES, &ptr, &len)) {
        if ((len%sizeof(T_ValType)) == 0) {
            *val = reinterpret_cast<T_ValType*>(ptr);
            *array_items = int32_t(len/sizeof(T_ValType));
            b = true;
        } else {
            //LOG(WARNING) << "get val fail, mismatch sizeof(data type). sizeof(T_ValType) : "
            //  << sizeof(T_ValType) << ", received data type len:"
            //  << static_cast<float>(len) / static_cast<float>(sizeof(T_ValType));
        }
    }
    return b;
}

template<typename T>
void CBaseProtocolUnpack_<T>::UntachPackage() {
    m_reference_data = 0;
    m_ref_data_len = 0;
}

template<typename T>
void CBaseProtocolUnpack_<T>::SetIsTryGetVal(){
    m_is_try_getval = true;
}


inline unsigned char CalCRC8(const unsigned char* data, uint32_t len) {
    //
    // CRC余式表
    //
    static const uint32_t crc_table[256] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
    };

    uint32_t crc = 0;
    const uint8_t *p = reinterpret_cast<const uint8_t *>(data);

    for (size_t i = 0; i < len; ++i) {
        // 以8位二进制数的形式暂存CRC的高8位
        uint8_t da   = (uint8_t)(crc >> 8);

        // 左移8位，相当于CRC的低8位乘以
        crc <<= 8;

        uint8_t val   = p[i];
        uint8_t index = (uint8_t)(da ^ val);

        // 高8位和当前字节相加后再查表求CRC ，再加上以前的CRC
        crc ^= crc_table[index];
    }

    return static_cast<uint8_t>(crc);
}

// ////////////////////////////////////////////////////////////////////////////

//
// CRC32算法
// 描述:
//       PKZip、WinZip 和 Ethernet 中的CRC算法, 做了累进改进, 改进见代码
// 备注:
//       适用于进行不定长累加的场合
//
inline uint32_t Reflect(uint32_t ref, char ch) {
    int32_t i;
    uint32_t value = 0;

    // bit 0 与 bit 7 交换
    // bit 1 与 bit 6 交换，如此类推...
    for (i = 1; i < (ch + 1); i++) {
        if (ref & 1)
            value |= 1 << (ch - i);
        ref >>= 1;
    }
    return value;
}

//
// 创建查表数组
//
inline void Gen_CRC32_Table() {
    //  该CRC-32多项式算法广泛应用于PKZip, WinZip和Ethernet中
    uint32_t polynomial = 0x04c11db7;
    int32_t i, j;
    FILE* fp = NULL;

    uint32_t crc32_tab[256] = {0};
    for (i = 0; i <= 0xFF; i++) {
        crc32_tab[i] = Reflect(i, 8) << 24;
        for (j = 0; j < 8; j++)
            crc32_tab[i] = (crc32_tab[i] << 1) ^
                           (crc32_tab[i] & (1 << 31) ? polynomial : 0);
        crc32_tab[i] = Reflect(crc32_tab[i], 32);
    }

    fp = fopen("c:\\crc32.txt", "ab");
    for (i = 0; i <= 255; i++) {
        fprintf(fp, " 0x%08x, ", crc32_tab[i]);
        if ((i+1)%8 == 0)
            fprintf(fp, "\r\n");
    }
    fclose(fp);
}

inline uint32_t GetCRC32(uint32_t old_crc,
                         const unsigned char* data,
                         uint32_t len) {
    //
    // CRC32算法
    // 描述:
    //       PKZip、WinZip 和 Ethernet 中的CRC算法, 做了累进改进, 改进见代码
    // 备注:
    //       适用于进行不定长累加的场合
    //
    static const uint32_t  crc32_table[256] = {
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
        0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
        0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
        0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
        0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
        0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
        0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
        0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
        0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
        0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
        0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
        0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
        0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
        0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
        0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
        0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
        0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
        0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
        0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
        0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
        0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
        0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
        0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
        0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
        0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
        0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
        0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
        0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
        0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
        0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
        0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
        0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
        0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
        0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
        0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
        0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
        0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
        0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
        0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
        0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
        0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
        0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
        0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
        0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
        0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
        0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
        0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
        0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
        0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
        0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
        0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
        0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
        0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
        0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
        0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
        0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
        0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
        0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
        0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
        0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
        0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
        0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
        0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
        0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
    };

    uint32_t  crc = old_crc;    //  初始化为这个值0xffffffff
    const unsigned char* buffer = data;

    // init table
    // 改变polynomial的值的时候才需要重新生成crc32 table
    // Gen_CRC32_Table<void>();

    for (uint32_t u = 0; u < len; ++u)
        crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ *buffer++];
    return crc;                  //  需要继续累加
    // return crc^0xffffffff;    //  结束累加的算法
}

//
// 检查是否有效的数据包, 针对扫描策略, 在头部增加自校验
//
inline bool IsValidPack(BasProtocolHead* head_with_net_order) {
    bool b = false;
    if (head_with_net_order) {
        // 转为本机字节序
        head_with_net_order->ToHostOrder();
        uint32_t head_checksum = GetCRC32((uint32_t)BASEPROTO_HEAD_CHKSUM_KEY,
                                          (unsigned char*)head_with_net_order,
                                          (uint32_t)(sizeof(BasProtocolHead) -
                                                  sizeof(uint32_t) -
                                                  sizeof(uint32_t)));
        // 不需要body checksum和head checksum部分
        b = (head_checksum == head_with_net_order->head_checksum);
        head_with_net_order->ToNetOrder();
    }
    return b;
}

#endif // LONGCONN_BASE_PROTOCOL_T_INL_
