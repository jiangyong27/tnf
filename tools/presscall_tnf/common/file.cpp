// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2014-08-24
//

#include "file.h"

/** ��ʽ��д�ļ� */
bool FileUtility::WriteFile(const std::string& sFileName, const char* fmt, ...)
{
    ofstream out_file;
    out_file.open(sFileName.c_str(),ios::app);
    if(!out_file.is_open())
    {
        PRINT_ERROR("open file["<<sFileName<<"] failed");
        return false;
    }

    /**һ�����д21k */
    char szBuf [1024*32] = {0};

    //��������д��������
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(szBuf, sizeof(szBuf), fmt, ap);
    va_end(ap);

    out_file << szBuf;
    if(!out_file.good())
    {
        PRINT_ERROR("write file["<<sFileName<<"] failed");
        return false;
    }
    out_file.close();

    return true;
}

/** д���ַ��� */
bool FileUtility::Write(const std::string& sFileName, const std::string& value)
{
    return WriteFile(sFileName,"%s",value.c_str());
}
/** д������ */
bool FileUtility::Write(const std::string& sFileName,const int& value)
{
    return WriteFile(sFileName,"%d",value);
}
bool FileUtility::Write(const std::string& sFileName,const uint32_t& value)
{
    return WriteFile(sFileName,"%u",value);
}
/**д��64λ�޷������� */
bool FileUtility::Write(const std::string& sFileName,const uint64_t& value)
{
    return WriteFile(sFileName,"%lu",value);
}

/** ���м����ļ� */
bool FileUtility::LoadLines(const std::string& path,vector<std::string>* lines)
{
    //�ж��Ƿ��ļ�
    if(!IsFile(path))
        return false;

    //���ļ�
    ifstream in_file;
    in_file.open(path.c_str(),ios::in);
    if(!in_file.is_open())
        return false;
    std::string tmp;
    lines->clear();

    //���ж�ȡ
    getline(in_file,tmp);
    while(in_file.good())
    {
        lines->push_back(tmp);
        getline(in_file,tmp);
    }

    //�ر��ļ�
    in_file.close();
    return true;
}

/**���ļ�����Ϊ�ַ��� */
bool FileUtility::LoadString(const std::string& path,std::string* str)
{
    ifstream in(path.c_str());
    if (in.fail()) {
        std::cerr << "File not found: " << path << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << in.rdbuf();
    in.close();
    str->assign(buffer.str());
    return true;
}

/**ɾ��Ŀ¼ */
bool FileUtility::DeleteDir(const std::string& path)
{
    //�ж��Ƿ���Ŀ¼
    if(!IsDir(path))
        return false;

    char chBuf[256];
    DIR * dir = NULL;
    struct dirent *ptr;
    int ret = 0;
    dir = opendir(path.c_str());
    if(NULL == dir)
        return false;

    //�ݹ��ȡĿ¼ɾ��Ŀ¼���ļ���Ŀ¼
    while((ptr = readdir(dir)) != NULL)
    {
        //������ǰĿ¼��Ŀ¼
        ret = strcmp(ptr->d_name, ".");
        if(0 == ret)
            continue;
        ret = strcmp(ptr->d_name, "..");
        if(0 == ret)
            continue;

        snprintf(chBuf, 256, "%s/%s", path.c_str(), ptr->d_name);

        //�����Ŀ¼�ݹ�ɾ��
        if(IsDir(chBuf))
        {
            if(!DeleteDir(chBuf))
                return false;
        }
        else
        {
            ret = remove(chBuf);
            if(0 != ret)
            {
                cout <<"["<<__FILE__<<"]["<<__LINE__<<"]";
                cout <<"[path="<<chBuf<<"][error="<<strerror(errno)<<"]"<<endl;
                return false;
            }
        }
    }

    //�ر�Ŀ¼
    closedir(dir);
    ret = remove(path.c_str());
    if(0 != ret)
    {
        cout <<"["<<__FILE__<<"]["<<__LINE__<<"]";
        cout <<"[path="<<path<<"]["<<strerror(errno)<<"]"<<endl;
        return false;
    }
    //cout <<"delete dir["<<path<<"]"<<endl;
    return true;
}

/**ɾ���ļ� */
bool FileUtility::DeleteFile(const std::string& path)
{
    if(!IsFile(path))
    {
        return false;
    }
    if(remove(path.c_str())!=0)
    {
        cout <<"["<<__FILE__<<"]["<<__LINE__<<"]";
        cout <<"[path="<<path<<"]["<<strerror(errno)<<"]"<<endl;
        return false;
    }
    return true;
}

/**��ȡ��ǰĿ¼ */
bool FileUtility::GetCurrentDir(std::string *dir)
{
    char buffer[1024];
    if(getcwd(buffer,1024)==NULL)
    {
        cout <<"["<<__FILE__<<"]["<<__LINE__<<"]";
        cout <<"[err="<<strerror(errno)<<"]"<<endl;
        return false;
    }
    *dir = buffer;
    *dir +="/";
    return true;
}

/** �������ļ�*/
bool FileUtility::Rename(const std::string& src,const std::string& dst)
{
    if(!IsFileExist(src))
        return false;
    return rename(src.c_str(),dst.c_str())==0;
}

/** �����ļ� ֻ֧���ļ� ��֧��Ŀ¼*/
bool FileUtility::Copy(const std::string& src,const std::string& dst)
{
    if(!IsFileExist(src))
        return false;

    //�������ļ�
    ifstream in;
    in.open(src.c_str(),ios::binary);
    if(in.fail())
        return false;

    //������ļ�
    ofstream out;
    out.open(dst.c_str(),ios::binary);
    if(out.fail())
    {
        in.close();
        return false;
    }

    //�������ļ���ȡ���ݵ�����ļ�
    out << in.rdbuf();
    in.close();
    out.close();
    return true;
}

/** ����Ŀ¼ */
bool FileUtility::CreateDir(const std::string& path)
{
    return mkdir(path.c_str(),0777)==0;
}

/** �����հ��ļ� */
bool FileUtility::Touch(const std::string& sFileName)
{
    if(IsFileExist(sFileName))
        return false;
    ofstream out;
    out.open(sFileName.c_str(),ios::app);
    if(out.fail())
        return false;
    return true;
}

/**�������� */
bool FileUtility::CreateLink(const std::string& src,const std::string& dst)
{
    std::string new_dst = dst;

    //Ŀ������Ϊ��
    if(dst.length()==0)
    {
        //���ԭ�ļ���
        std::string::size_type pos = src.find_last_of("/");
        if(pos == std::string::npos)
        {
            cout <<"["<<__FILE__<<"]["<<__LINE__<<"]";
            cout << "[src="<<src<<"][dst="<<dst<<"][src=dst]"<<endl;
            return false;
        }
        new_dst = src.substr(pos+1);
        if(new_dst.length()==0)
        {
            std::string::size_type pos2 = src.find_last_of("/",pos-1);
            if(pos2 == std::string::npos)
            {
                cout <<"["<<__FILE__<<"]["<<__LINE__<<"]";
                cout << "[src="<<src<<"][dst="<<dst<<"][src=dst]"<<endl;
                return false;
            }
            new_dst = src.substr(pos2,pos-pos2);
            std::string cur;
            if(!GetCurrentDir(&cur))
                return false;
            new_dst = cur +new_dst;
        }
    }

    //��������
    if(symlink(src.c_str(),new_dst.c_str()) != 0)
    {
        cout <<"["<<__FILE__<<"]["<<__LINE__<<"]";
        cout << "[src="<<src<<"][dst="<<new_dst<<"][err="<<strerror(errno)<<"]"<<endl;
        return false;
    }
    return true;
}

/**����ļ�״̬ */
bool FileUtility::GetStat(const std::string& path,struct stat* st)
{
    if(stat(path.c_str(),st)!=0)
    {
        PRINT_ERROR("[path="+path+"][error="+strerror(errno)+"]");
        return false;
    }
    return true;
}

/** �ж��ļ��Ƿ�Ϊ�û��ļ�*/
bool FileUtility::IsUserFile(const std::string& path)
{
    if(!IsFile(path))
        return false;
    struct stat st;
    if(!GetStat(path,&st))
        return false;
    return st.st_uid==getuid();
}

/** ����ļ���С*/
int FileUtility::GetFileSize(const std::string& path)
{
    if(!IsFile(path))
        return -1;
    struct stat st;
    if(!GetStat(path,&st))
        return -2;
    return st.st_size;
}

/**����ļ��޸�ʱ�� */
time_t FileUtility::GetModifyTime(const std::string& path)
{
    struct stat st;
    if(!GetStat(path,&st))
        return 0;
    return st.st_mtime;
}

/**����ļ�������ʱ�� */
time_t FileUtility::GetAccessTime(const std::string& path)
{
    struct stat st;
    if(!GetStat(path,&st))
        return 0;
    return st.st_atime;
}

/**�ж��Ƿ���Ŀ¼ */
bool FileUtility::IsDir(const std::string& path)
{
    struct stat st;
    if(!GetStat(path,&st))
        return false;
    if(!S_ISDIR(st.st_mode))
        return false;
    return true;
}

/** �ж��Ƿ����ļ� **/
bool FileUtility::IsFile(const std::string& path)
{
    struct stat st;
    if(!GetStat(path,&st))
        return false;
    if(!S_ISREG(st.st_mode))
        return false;
    return true;
}
/** �ļ��Ƿ���� */
bool FileUtility::IsFileExist(const std::string& path)
{
    if(access(path.c_str(),F_OK)==0)
        return true;
    return false;
}

/** ����ļ��б�*/
bool FileUtility::GetFileList(const std::string& path, std::vector<std::string>* files)
{
    if (!IsDir(path)) {
        return false;
    }
    files->clear();
    DIR *dir;
    struct dirent *ptr;
    dir = opendir(path.c_str());
    while ((ptr = readdir(dir)) != NULL) {
        if (strcmp(".", ptr->d_name) == 0 || strcmp("..", ptr->d_name) == 0) {
            continue;
        }
        files->push_back(ptr->d_name);
    }
    return true;
}

bool FileUtility::SplitPath(const std::string& Path, std::string *FilePath, std::string *FileName)
{
    std::string::size_type Pos;
    Pos = Path.find_last_of("/\\");
    if (Pos == std::string::npos) {
        return false;
    }
    FilePath->assign(Path.substr(0, Pos));
    FileName->assign(Path.substr(Pos+1));
    return true;
}
bool FileUtility::SplitFileName(const std::string& FileName, std::string *BaseName, std::string *SuffixName)
{
    std::string::size_type Pos;
    Pos = FileName.find_last_of(".");
    if (Pos == std::string::npos) {
        return false;
    }
    BaseName->assign(FileName.substr(0, Pos));
    SuffixName->assign(FileName.substr(Pos+1));
    return true;
}

/** *************************** CFile ******************************/
CFile::~CFile()
{
    m_file.close();
}

CFile::CFile(const std::string& path, int mode)
{
    m_path = path;
    m_mode = mode;
    if (mode == MODE_READ) {
        m_file.open(m_path.c_str(), ios::in);
        m_read = true;
    } else if (mode == MODE_TRUNC) {
        m_file.open(m_path.c_str(), ios::out|ios::trunc);
        m_read = false;
    } else if (mode == MODE_WRITE) {
        m_file.open(m_path.c_str(), ios::out|ios::app);
        m_read = false;
    } else {
        std::cout << "file mode[" << mode << "] is error!" << std::endl;
    }
}
bool CFile::Good()
{
    return m_file.good();
}

bool CFile::ReadFile(char* buf, uint32_t len)
{
    if (!m_read || !m_file.is_open()) {
        return false;
    }
    m_file.read(buf, len);
    return m_file.good();
}

bool CFile::ReadLine(std::string* line)
{
    if (!m_read || !m_file.is_open()) {
        return false;
    }
    line->clear();
    getline(m_file, *line);
    return m_file.good();
}

bool CFile::WriteFile(const char* fmt, uint32_t len)
{
    if (m_read || !m_file.is_open()) {
        return false;
    }
    m_file.write(fmt, len);
    return m_file.good();
}

bool CFile::WriteFile(const std::string& line)
{
    if (m_read || !m_file.is_open()) {
        return false;
    }
    m_file.write(line.c_str(), line.size());
    return m_file.good();
}

bool CFile::WriteLine(const std::string& line)
{
    if (WriteFile(line) && WriteFile("\n")) {
        return true;
    }
    return false;
}

/** ��ʽ��д�ļ� */
bool CFile::WriteFile(const char* fmt, ...)
{
    if (m_read || !m_file.is_open()) {
        return false;
    }

    /**һ�����д21k */
    char szBuf [1024*32] = {0};

    //��������д��������
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(szBuf, sizeof(szBuf), fmt, ap);
    va_end(ap);
    m_file << szBuf;
    return m_file.good();
}

uint64_t CFile::Tell()
{
    return (uint64_t)m_file.tellg();
}
void CFile::Seek(uint64_t pos)
{
    m_file.seekg(pos);
}

void CFile::Flush()
{
    m_file.flush();
}
