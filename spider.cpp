#include<fstream>
#include<iostream>
#include<string.h>
#include<stdio.h>
#include<regex>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

string g_host;    //主机
string g_object;  //资源路径
SOCKET g_socket;  //套接字
vector<string>vecurl;   //章节url


//开始爬取
bool StartCatch(string url);
//解析url
bool AnalyseURL(string url);
//初始化网络
bool InitNet();
//连接网站
bool Connect();
//获取网页
wstring GetHtml();
//解析网页内容
bool AnalyHtml(wstring html);
//解析章节网页并下载
bool AnalyHtml2(std::wofstream f,wstring html);
//下载每章小说
bool DloadNovel(std::wofstream f,string url);
//下载小说
bool DownloadNovel();
//将string转换成wstring  
wstring string2wstring(string str);
//连接网络下载网页
wstring Net(string url);
std::wstring s2ws(const std::string& s);

int main()
{
	cout << "****************************************************" << endl\
		 << "*                    爬虫：小说                    *" << endl\
		 << "****************************************************" << endl;
	cout << "请输入要抓取的url" << endl;
	string starturl;
	//starturl = "http://www.ltoooo.com/0_301/";
	cin >> starturl;

	//创建存储小说的文件夹
	//CreateDirectory(L"novels",NULL);
	//开始爬取
	StartCatch(starturl);
	return 0;
}
//开始抓取
bool StartCatch(string url)
{
	//初始化网络
	if (false == InitNet())
		return false;
	cout << "初始化网络成功" << endl;

	//解析url（统一资源定位器）  由协议，域名，资源路径三部分组成
	//http://www.ltoooo.com/0_301/
	if (false == AnalyseURL(url))
		return false;
	cout << "解析域名成功" << endl;

	//解析域名
	//连接网站
	if (false == Connect())
		return false;
	cout << "服务器连接成功" << endl;
	//获取网页html
	wstring html = GetHtml();
	//wcout << html << endl;
	cout << "获取网页成功" << endl;
	//分析网页提取章节url到vecurl
	AnalyHtml(html);		
	cout << "提取章节url成功" << endl;
	//下载小说
	if (true == DownloadNovel())
		cout << "小说下载成功" << endl;
	return true;
}
//解析URL
bool AnalyseURL(string url)
{
	if (url.find("http://") == string::npos)
		return false;
	if (url.length() <= 7)
		return false;
	//截取域名和资源路径
	int pos = url.find('/', 7);
	if (pos == string::npos)
	{
		g_host = url.substr(7);
		g_object = "/";
	}
	else
	{
		g_host=url.substr(7, pos - 7);
		g_object = url.substr(pos);
	}
	if (g_host.empty())
		return false;
	//打印测试
//	cout << "域名：" << g_host << endl << "资源路径：" << g_object << endl;
	return true;
}
//初始化网络
bool InitNet()
{
	WSADATA wd;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wd))
		return false;
	//创建套接字
	g_socket = socket(AF_INET, SOCK_STREAM,0);
	if (g_socket == INVALID_SOCKET)
		return false;

}
//连接网站
bool Connect()
{
	//把域名解析成ip地址
	hostent* host = gethostbyname(g_host.c_str());
	in_addr in_addr_string;
	memcpy(&in_addr_string, host->h_addr,4);
//	cout<<"IP地址为："<<inet_ntoa(in_addr_string)<<endl;

	//链接web服务器
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(80);
	sa.sin_addr = in_addr_string;

	if (SOCKET_ERROR == connect(g_socket, (sockaddr*)&sa, sizeof(sockaddr)))
		return false;

	return true;
}
//获取网页
wstring GetHtml()
{
	//拼接浏览器向服务器发送的信息
	string info;
	info = info + "GET " + "http://" + g_host + g_object + " HTTP/1.1\r\n";
	info = info + "HOST: " + g_host+"\r\n";
	info = info + "Connection: close\r\n";
	info = info + "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Safari/537.36\r\n";
	info = info + "\r\n";

	//发送信息
	if(SOCKET_ERROR==send(g_socket,info.c_str(),info.length(),0))
		return (L"");
	//接受服务器返回的信息
	char ch;
	string html;
	while (recv(g_socket, &ch, sizeof(char), 0))
	{
		html = html + ch;
	}
	//cout << html << endl;
	//将string转换成wstring  
	//wstring whtml=string2wstring(html);
	wstring whtml = s2ws(html);
	setlocale(LC_ALL,"");
	//wcout << whtml << endl;
	//cout << "why" << endl;
	return whtml;
}
//解析网页内容
bool AnalyHtml(wstring html)
{
	wsmatch mat;
	wregex rex(L"a href =\"/[^\\s'\"<>()]+");
	wstring::const_iterator start = html.begin();
	wstring::const_iterator end = html.end();
	while (regex_search(start, end, mat, rex))
	{
		string per(mat[0].first, mat[0].second);
		per = per.substr(9);
		per = "http://" + g_host + per;
		vecurl.push_back(per); 
		start = mat[0].second;
	}


	return true;
}
/*
//下载每章小说
bool DloadNovel(std::wofstream f,string url)
{
	wstring html = Net(url);
	//解析章节网页并下载
	AnalyHtml2(f,html);

}

//下载小说
bool DownloadNovel()
{
	wcout.imbue(std::locale("chs")); //设置语言区域为中国
	//打开一个文本文件
	std::wofstream f(L"\\some file.txt");
	
	FILE* fp = fopen(filename.c_str(), "a+");
	if (NULL == fp)
		return false;
		
	//遍历每章url
	for (int i = 0; i < vecurl.size(); i++)
	{
		DloadNovel(f,vecurl[i]);
	}
	return true;
}
*/
//解析章节网页并下载
bool AnalyHtml2(std::wofstream f,wstring whtml)
{
	wsmatch wmat;
	wregex wrex(L"<h1>[^<>]+");				//匹配章节标题
	//wregex wrexcontent(L"\[u4e00-u9fa5\]");					//匹配正文
	std::wstring::const_iterator start = whtml.begin();
	std::wstring::const_iterator end = whtml.end();
	while (regex_search(start, end, wmat, wrex))
	{
		wstring per(wmat[0].first, wmat[0].second);
		per = per.substr(5);
		f << per.c_str();
		start = wmat[0].second;
	}

	return true;
}

//将string转换成wstring  
wstring string2wstring(string str)
{
	wstring result;
	//获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	//多字节编码转换成宽字节编码  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';             //添加字符串结尾  
	//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}
std::wstring s2ws(const std::string& s)
{
	setlocale(LC_ALL, "chs");
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t* _Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest, _Source, _Dsize);
	std::wstring result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, "C");
	return result;
}

//连接网络下载网页
wstring Net(string url)
{
	//初始化网络
	if (false == InitNet())
		return (L"");
	//http://www.ltoooo.com/0_301/
	if (false == AnalyseURL(url))
		return (L"");
	//解析域名
	//连接网站
	if (false == Connect())
		return (L"");
	//获取网页html
	wstring html = GetHtml();
	return html;
}
//下载小说
bool DownloadNovel()
{
	wcout.imbue(std::locale("chs")); //设置语言区域为中国
	//打开一个文本文件
	std::wofstream f;
	f.open(L"C:\\Users\\HF\\Desktop\\1.txt", ios::out | ios::app);
	/*
	FILE* fp = fopen(filename.c_str(), "a+");
	if (NULL == fp)
		return false;
		*/
		//遍历每章url
	for (int i = 0; i < vecurl.size(); i++)
	{
		wstring head;
		f.open(L"C:\\Users\\HF\\Desktop\\1.txt", ios::out | ios::app);
		wstring whtml = Net(vecurl[i]);
		wsmatch wmat;
		wregex wrex(L"<h1>[^<>]+");				//匹配章节标题
		wregex wrexcontent(L"\[\u4e00-\u9fa5\u3002\uff1b\uff0c\uff1a\u201c\u201d\uff08\uff09\u3001\uff1f\u300a\u300b\]");	//匹配正文				//匹配正文
		std::wstring::const_iterator start1 = whtml.begin();
		std::wstring::const_iterator end1 = whtml.end();
		while (regex_search(start1, end1, wmat, wrex))		//下载章节标题
		{
			wstring per(wmat[0].first, wmat[0].second);
			per = per.substr(4);
			head = per;
			wcout << per <<"正在下载..."<<endl;
			f.imbue(std::locale("chs"));
			f << per.c_str();
			start1 = wmat[0].second;
		}
		f << endl;
		//whtml = whtml(whtml.find(L"<div id=\"content\" "),whtml.find(L"<div class=\"page_chapter\">");
		size_t pos1 = whtml.find(L"<div id=\"content\" ");
		size_t pos2 = whtml.find(L"<br /><br /></div>");
		whtml = whtml.substr(pos1, pos2-pos1);
		std::wstring::const_iterator start = whtml.begin();
		std::wstring::const_iterator end = whtml.end();
		while (regex_search(start, end, wmat, wrexcontent))		//下载正文
		{
			wstring per(wmat[0].first, wmat[0].second);
		//	wcout << per << endl;
			f.imbue(std::locale("chs"));
			f << per.c_str();
			start = wmat[0].second;
		}
		f << endl<<endl<<endl;
		//cout << "第" << i << "章下载成功" << endl;
		wcout << head<<L"下载成功" << endl;
		f.close();
	}
	f.close();
	return true;
}