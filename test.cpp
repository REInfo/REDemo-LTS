/*!
* \file test.cpp
* \brief 示例代码主程序
*
* 本项目是基于华宝技术LTS证券接口开发的示例程序，用于展示如何在LTS
* 环境下进行开发。示例代码演示了LTS各类接口的调用，在编写相关项目时
* 可以参考。
* 由尔易信息提供开源，最新代码可从http://github.com/REInfo获取。
* 上海尔易信息科技有限公司提供证券、期货、期权、现货等市场交易、结算、
* 风控业务的客户化定制服务。
*
* \author Christian
* \version 1.0
* \date 2014-6-16
* 
*/
#include "test.h"

void test_order(void)
{
	//初始化UserApi
	CSecurityFtdcTraderApi* pUserApi = CSecurityFtdcTraderApi::CreateFtdcTraderApi();
	traderspi* pUserSpi = new traderspi(pUserApi);
	pUserApi->RegisterSpi((CSecurityFtdcTraderSpi*)pUserSpi);         // 注册事件类
	pUserApi->SubscribePublicTopic(SECURITY_TERT_RESTART);			  // 注册公有流
	pUserApi->SubscribePrivateTopic(SECURITY_TERT_RESTART);			  // 注册私有流
	pUserApi->RegisterFront(tradeFront);							  // 注册交易前置地址

	pUserApi->Init();
	ShowTraderCommand(pUserSpi,true); 
	pUserApi->Join();  
	pUserApi->Release();
}


void test_md(void)
{
	///初始化UserApi
	CSecurityFtdcMdApi* pUserApi=CSecurityFtdcMdApi::CreateFtdcMdApi();
	mdspi* pUserSpi=new mdspi(pUserApi); ///创建回调处理类对象MdSpi
	pUserApi->RegisterSpi(pUserSpi);			 /// 回调对象注入接口类
	pUserApi->RegisterFront(mdFront);		     /// 注册行情前置地址
	pUserApi->Init();      ///接口线程启动, 开始工作
	ShowMdCommand(pUserSpi,true); 
	pUserApi->Join();      ///等待接口线程退出
	pUserApi->Release(); ///接口对象释放
}
void main(int argc, const char* argv[]){
	g_hEvent=CreateEvent(NULL, true, false, NULL); 

	if(argc < 2)  cerr<<"使用参数 --md 或 --trade."<<endl;  
	else if(strcmp(argv[1],"--md")==0)    test_md();
	else if(strcmp(argv[1],"--trade")==0) test_order();

}