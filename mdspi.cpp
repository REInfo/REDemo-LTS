/*!
* \file mdspi.cpp
* \brief 行情接口实现
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
#include "mdspi.h"
#include <iostream>
#include <vector>
#include "windows.h"

using namespace std;
#pragma warning(disable : 4996)

extern int requestId;  
extern HANDLE g_hEvent;

void mdspi::OnRspError(CSecurityFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}

void mdspi::OnFrontDisconnected(int nReason)
{
	cerr<<" 响应 | 连接中断..." 
		<< " reason=" << nReason << endl;
}

void mdspi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr<<" 响应 | 心跳超时警告..." 
		<< " TimerLapse = " << nTimeLapse << endl;
}

void mdspi::OnFrontConnected()
{
	cerr<<" 连接交易前置...成功"<<endl;
	SetEvent(g_hEvent);
}

void mdspi::ReqUserLogin(TSecurityFtdcBrokerIDType	appId,
	TSecurityFtdcUserIDType	userId,	TSecurityFtdcPasswordType	passwd)
{
	CSecurityFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, appId);
	strcpy(req.UserID, userId);
	strcpy(req.Password, passwd);
	int ret = pUserApi->ReqUserLogin(&req, ++requestId);
	cerr<<" 请求 | 发送登录..."<<((ret == 0) ? "成功" :"失败") << endl;	
	//SetEvent(g_hEvent);
}

void mdspi::OnRspUserLogin(CSecurityFtdcRspUserLoginField *pRspUserLogin,
	CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) && pRspUserLogin)
	{
		cerr<<" 响应 | 登录成功...当前交易日:"
			<<pRspUserLogin->TradingDay<<endl;
	}
	if(bIsLast) SetEvent(g_hEvent);
}

void mdspi::SubscribeMarketData(char* instIdList, char* exchangeID)
{
	vector<char*> list;
	char *token = strtok(instIdList, ",");
	while( token != NULL ){
		list.push_back(token); 
		token = strtok(NULL, ",");
	}
	unsigned int len = list.size();
	char** pInstId = new char* [len];  
	for(unsigned int i=0; i<len;i++)  pInstId[i]=list[i]; 
	int ret=pUserApi->SubscribeMarketData(pInstId, len,exchangeID);
	cerr<<" 请求 | 发送行情订阅... "<<((ret == 0) ? "成功" : "失败")<< endl;
	//SetEvent(g_hEvent);
}

void mdspi::OnRspSubMarketData(
	CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, 
	CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr<<" 响应 |  行情订阅...成功"<<endl;
	if(bIsLast)  SetEvent(g_hEvent);
}

void mdspi::OnRspUnSubMarketData(
	CSecurityFtdcSpecificInstrumentField *pSpecificInstrument,
	CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr<<" 响应 |  行情取消订阅...成功"<<endl;
	if(bIsLast)  SetEvent(g_hEvent);
}

void mdspi::OnRtnDepthMarketData(
	CSecurityFtdcDepthMarketDataField *pDepthMarketData)
{
	cerr<<" 行情 | 合约:"<<pDepthMarketData->InstrumentID
		<<" 现价:"<<pDepthMarketData->LastPrice
		<<" 最高价:" << pDepthMarketData->HighestPrice
		<<" 最低价:" << pDepthMarketData->LowestPrice
		<<" 卖一价:" << pDepthMarketData->AskPrice1 
		<<" 卖一量:" << pDepthMarketData->AskVolume1 
		<<" 买一价:" << pDepthMarketData->BidPrice1
		<<" 买一量:" << pDepthMarketData->BidVolume1
		<<" 持仓量:"<< pDepthMarketData->OpenInterest <<endl;
}

bool mdspi::IsErrorRspInfo(CSecurityFtdcRspInfoField *pRspInfo)
{	
	bool ret = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (ret){
		cerr<<" 响应 | "<<pRspInfo->ErrorMsg<<endl;
	}
	return ret;
}