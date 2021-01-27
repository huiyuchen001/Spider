#include<fstream>
#include<iostream>
#include<string.h>
#include<stdio.h>
#include<regex>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

string g_host;    //����
string g_object;  //��Դ·��
SOCKET g_socket;  //�׽���
vector<string>vecurl;   //�½�url


//��ʼ��ȡ
bool StartCatch(string url);
//����url
bool AnalyseURL(string url);
//��ʼ������
bool InitNet();
//������վ
bool Connect();
//��ȡ��ҳ
wstring GetHtml();
//������ҳ����
bool AnalyHtml(wstring html);
//�����½���ҳ������
bool AnalyHtml2(std::wofstream f,wstring html);
//����ÿ��С˵
bool DloadNovel(std::wofstream f,string url);
//����С˵
bool DownloadNovel();
//��stringת����wstring  
wstring string2wstring(string str);
//��������������ҳ
wstring Net(string url);
std::wstring s2ws(const std::string& s);

int main()
{
	cout << "****************************************************" << endl\
		 << "*                    ���棺С˵                    *" << endl\
		 << "****************************************************" << endl;
	cout << "������Ҫץȡ��url" << endl;
	string starturl;
	//starturl = "http://www.ltoooo.com/0_301/";
	cin >> starturl;

	//�����洢С˵���ļ���
	//CreateDirectory(L"novels",NULL);
	//��ʼ��ȡ
	StartCatch(starturl);
	return 0;
}
//��ʼץȡ
bool StartCatch(string url)
{
	//��ʼ������
	if (false == InitNet())
		return false;
	cout << "��ʼ������ɹ�" << endl;

	//����url��ͳһ��Դ��λ����  ��Э�飬��������Դ·�����������
	//http://www.ltoooo.com/0_301/
	if (false == AnalyseURL(url))
		return false;
	cout << "���������ɹ�" << endl;

	//��������
	//������վ
	if (false == Connect())
		return false;
	cout << "���������ӳɹ�" << endl;
	//��ȡ��ҳhtml
	wstring html = GetHtml();
	//wcout << html << endl;
	cout << "��ȡ��ҳ�ɹ�" << endl;
	//������ҳ��ȡ�½�url��vecurl
	AnalyHtml(html);		
	cout << "��ȡ�½�url�ɹ�" << endl;
	//����С˵
	if (true == DownloadNovel())
		cout << "С˵���سɹ�" << endl;
	return true;
}
//����URL
bool AnalyseURL(string url)
{
	if (url.find("http://") == string::npos)
		return false;
	if (url.length() <= 7)
		return false;
	//��ȡ��������Դ·��
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
	//��ӡ����
//	cout << "������" << g_host << endl << "��Դ·����" << g_object << endl;
	return true;
}
//��ʼ������
bool InitNet()
{
	WSADATA wd;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wd))
		return false;
	//�����׽���
	g_socket = socket(AF_INET, SOCK_STREAM,0);
	if (g_socket == INVALID_SOCKET)
		return false;

}
//������վ
bool Connect()
{
	//������������ip��ַ
	hostent* host = gethostbyname(g_host.c_str());
	in_addr in_addr_string;
	memcpy(&in_addr_string, host->h_addr,4);
//	cout<<"IP��ַΪ��"<<inet_ntoa(in_addr_string)<<endl;

	//����web������
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(80);
	sa.sin_addr = in_addr_string;

	if (SOCKET_ERROR == connect(g_socket, (sockaddr*)&sa, sizeof(sockaddr)))
		return false;

	return true;
}
//��ȡ��ҳ
wstring GetHtml()
{
	//ƴ�����������������͵���Ϣ
	string info;
	info = info + "GET " + "http://" + g_host + g_object + " HTTP/1.1\r\n";
	info = info + "HOST: " + g_host+"\r\n";
	info = info + "Connection: close\r\n";
	info = info + "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Safari/537.36\r\n";
	info = info + "\r\n";

	//������Ϣ
	if(SOCKET_ERROR==send(g_socket,info.c_str(),info.length(),0))
		return (L"");
	//���ܷ��������ص���Ϣ
	char ch;
	string html;
	while (recv(g_socket, &ch, sizeof(char), 0))
	{
		html = html + ch;
	}
	//cout << html << endl;
	//��stringת����wstring  
	//wstring whtml=string2wstring(html);
	wstring whtml = s2ws(html);
	setlocale(LC_ALL,"");
	//wcout << whtml << endl;
	//cout << "why" << endl;
	return whtml;
}
//������ҳ����
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
//����ÿ��С˵
bool DloadNovel(std::wofstream f,string url)
{
	wstring html = Net(url);
	//�����½���ҳ������
	AnalyHtml2(f,html);

}

//����С˵
bool DownloadNovel()
{
	wcout.imbue(std::locale("chs")); //������������Ϊ�й�
	//��һ���ı��ļ�
	std::wofstream f(L"\\some file.txt");
	
	FILE* fp = fopen(filename.c_str(), "a+");
	if (NULL == fp)
		return false;
		
	//����ÿ��url
	for (int i = 0; i < vecurl.size(); i++)
	{
		DloadNovel(f,vecurl[i]);
	}
	return true;
}
*/
//�����½���ҳ������
bool AnalyHtml2(std::wofstream f,wstring whtml)
{
	wsmatch wmat;
	wregex wrex(L"<h1>[^<>]+");				//ƥ���½ڱ���
	//wregex wrexcontent(L"\[u4e00-u9fa5\]");					//ƥ������
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

//��stringת����wstring  
wstring string2wstring(string str)
{
	wstring result;
	//��ȡ��������С��������ռ䣬��������С���ַ�����  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	//���ֽڱ���ת���ɿ��ֽڱ���  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';             //����ַ�����β  
	//ɾ��������������ֵ  
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

//��������������ҳ
wstring Net(string url)
{
	//��ʼ������
	if (false == InitNet())
		return (L"");
	//http://www.ltoooo.com/0_301/
	if (false == AnalyseURL(url))
		return (L"");
	//��������
	//������վ
	if (false == Connect())
		return (L"");
	//��ȡ��ҳhtml
	wstring html = GetHtml();
	return html;
}
//����С˵
bool DownloadNovel()
{
	wcout.imbue(std::locale("chs")); //������������Ϊ�й�
	//��һ���ı��ļ�
	std::wofstream f;
	f.open(L"C:\\Users\\HF\\Desktop\\1.txt", ios::out | ios::app);
	/*
	FILE* fp = fopen(filename.c_str(), "a+");
	if (NULL == fp)
		return false;
		*/
		//����ÿ��url
	for (int i = 0; i < vecurl.size(); i++)
	{
		wstring head;
		f.open(L"C:\\Users\\HF\\Desktop\\1.txt", ios::out | ios::app);
		wstring whtml = Net(vecurl[i]);
		wsmatch wmat;
		wregex wrex(L"<h1>[^<>]+");				//ƥ���½ڱ���
		wregex wrexcontent(L"\[\u4e00-\u9fa5\u3002\uff1b\uff0c\uff1a\u201c\u201d\uff08\uff09\u3001\uff1f\u300a\u300b\]");	//ƥ������				//ƥ������
		std::wstring::const_iterator start1 = whtml.begin();
		std::wstring::const_iterator end1 = whtml.end();
		while (regex_search(start1, end1, wmat, wrex))		//�����½ڱ���
		{
			wstring per(wmat[0].first, wmat[0].second);
			per = per.substr(4);
			head = per;
			wcout << per <<"��������..."<<endl;
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
		while (regex_search(start, end, wmat, wrexcontent))		//��������
		{
			wstring per(wmat[0].first, wmat[0].second);
		//	wcout << per << endl;
			f.imbue(std::locale("chs"));
			f << per.c_str();
			start = wmat[0].second;
		}
		f << endl<<endl<<endl;
		//cout << "��" << i << "�����سɹ�" << endl;
		wcout << head<<L"���سɹ�" << endl;
		f.close();
	}
	f.close();
	return true;
}