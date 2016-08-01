// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2014-08-24
//

#include "file.h"

/** 格式化写文件 */
bool FileUtility::WriteFile(const std::string& sFileName, const char* fmt, ...)
{
    ofstream out_file;
    out_file.open(sFileName.c_str(),ios::app);
    if(!out_file.is_open())
    {
        PRINT_ERROR("open file["<<sFileName<<"] failed");
        return false;
    }

    /**一次最多写21k */
    char szBuf [1024*32] = {0};

    //把内容先写到缓冲区
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

/** 写入字符串 */
bool FileUtility::Write(const std::string& sFileName, const std::string& value)
{
    return WriteFile(sFileName,"%s",value.c_str());
}
/** 写入整数 */
bool FileUtility::Write(const std::string& sFileName,const int& value)
{
    return WriteFile(sFileName,"%d",value);
}
bool FileUtility::Write(const std::string& sFileName,const uint32_t& value)
{
    return WriteFile(sFileName,"%u",value);
}
/**写入64位无符号整数 */
bool FileUtility::Write(const std::string& sFileName,const uint64_t& value)
{
    return WriteFile(sFileName,"%lu",value);
}

/** 按行加载文件 */
bool FileUtility::LoadLines(const std::string& path,vector<std::string>* lines)
{
    //判断是否文件
    if(!IsFile(path))
        return false;

    //打开文件
    ifstream in_file;
    in_file.open(path.c_str(),ios::in);
    if(!in_file.is_open())
        return false;
    std::string tmp;
    lines->clear();

    //按行读取
    getline(in_file,tmp);
    while(in_file.good())
    {
        lines->push_back(tmp);
        getline(in_file,tmp);
    }

    //关闭文件
    in_file.close();
    return true;
}

/**将文件加载为字符串 */
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

/**删除目录 */
bool FileUtility::DeleteDir(const std::string& path)
{
    //判断是否是目录
    if(!IsDir(path))
        return false;

    char chBuf[256];
    DIR * dir = NULL;
    struct dirent *ptr;
    int ret = 0;
    dir = opendir(path.c_str());
    if(NULL == dir)
        return false;

    //递归读取目录删除目录下文件或目录
    while((ptr = readdir(dir)) != NULL)
    {
        //跳过当前目录或父目录
        ret = strcmp(ptr->d_name, ".");
        if(0 == ret)
            continue;
        ret = strcmp(ptr->d_name, "..");
        if(0 == ret)
            continue;

        snprintf(chBuf, 256, "%s/%s", path.c_str(), ptr->d_name);

        //如果是目录递归删除
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

    //关闭目录
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

/**删除文件 */
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

/**获取当前目录 */
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

/** 重命名文件*/
bool FileUtility::Rename(const std::string& src,const std::string& dst)
{
    if(!IsFileExist(src))
        return false;
    return rename(src.c_str(),dst.c_str())==0;
}

/** 复制文件 只支持文件 不支持目录*/
bool FileUtility::Copy(const std::string& src,const std::string& dst)
{
    if(!IsFileExist(src))
        return false;

    //打开输入文件
    ifstream in;
    in.open(src.c_str(),ios::binary);
    if(in.fail())
        return false;

    //打开输出文件
    ofstream out;
    out.open(dst.c_str(),ios::binary);
    if(out.fail())
    {
        in.close();
        return false;
    }

    //从输入文件读取数据到暑促文件
    out << in.rdbuf();
    in.close();
    out.close();
    return true;
}

/** 创建目录 */
bool FileUtility::CreateDir(const std::string& path)
{
    return mkdir(path.c_str(),0777)==0;
}

/** 创建空白文件 */
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

/**创建连接 */
bool FileUtility::CreateLink(const std::string& src,const std::string& dst)
{
    std::string new_dst = dst;

    //目标连接为空
    if(dst.length()==0)
    {
        //获得原文件名
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

    //创建链接
    if(symlink(src.c_str(),new_dst.c_str()) != 0)
    {
        cout <<"["<<__FILE__<<"]["<<__LINE__<<"]";
        cout << "[src="<<src<<"][dst="<<new_dst<<"][err="<<strerror(errno)<<"]"<<endl;
        return false;
    }
    return true;
}

/**获得文件状态 */
bool FileUtility::GetStat(const std::string& path,struct stat* st)
{
    if(stat(path.c_str(),st)!=0)
    {
        PRINT_ERROR("[path="+path+"][error="+strerror(errno)+"]");
        return false;
    }
    return true;
}

/** 判断文件是否为用户文件*/
bool FileUtility::IsUserFile(const std::string& path)
{
    if(!IsFile(path))
        return false;
    struct stat st;
    if(!GetStat(path,&st))
        return false;
    return st.st_uid==getuid();
}

/** 获得文件大小*/
int FileUtility::GetFileSize(const std::string& path)
{
    if(!IsFile(path))
        return -1;
    struct stat st;
    if(!GetStat(path,&st))
        return -2;
    return st.st_size;
}

/**获得文件修改时间 */
time_t FileUtility::GetModifyTime(const std::string& path)
{
    struct stat st;
    if(!GetStat(path,&st))
        return 0;
    return st.st_mtime;
}

/**获得文件最后访问时间 */
time_t FileUtility::GetAccessTime(const std::string& path)
{
    struct stat st;
    if(!GetStat(path,&st))
        return 0;
    return st.st_atime;
}

/**判断是否是目录 */
bool FileUtility::IsDir(const std::string& path)
{
    struct stat st;
    if(!GetStat(path,&st))
        return false;
    if(!S_ISDIR(st.st_mode))
        return false;
    return true;
}

/** 判断是否是文件 **/
bool FileUtility::IsFile(const std::string& path)
{
    struct stat st;
    if(!GetStat(path,&st))
        return false;
    if(!S_ISREG(st.st_mode))
        return false;
    return true;
}
/** 文件是否存在 */
bool FileUtility::IsFileExist(const std::string& path)
{
    if(access(path.c_str(),F_OK)==0)
        return true;
    return false;
}

/** 获得文件列表*/
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

/** 格式化写文件 */
bool CFile::WriteFile(const char* fmt, ...)
{
    if (m_read || !m_file.is_open()) {
        return false;
    }

    /**一次最多写21k */
    char szBuf [1024*32] = {0};

    //把内容先写到缓冲区
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
