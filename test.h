/*!
* \file test.h
* \brief 示例代码主程序接口
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
#ifndef TEST_H_
#define TEST_H_

#include "config.h"
#include "api/trade/win/public/SecurityFtdcTraderApi.h"
#include "api/trade/win/public/SecurityFtdcMdApi.h"
#include "traderspi.h"
#include "mdspi.h"
#include "windows.h"
#include <iostream>
using namespace std;

HANDLE g_hEvent;

void ShowTraderCommand(traderspi* p, bool print=false){
	if(print){
		cerr<<"-----------------------------------------------"<<endl;
		cerr<<" [1] ReqUserLogin              -- 登录"<<endl;
		cerr<<" [2] ReqQryInstrument          -- 查询合约"<<endl;
		cerr<<" [3] ReqQryTradingAccount      -- 查询资金"<<endl;
		cerr<<" [4] ReqQryInvestorPosition    -- 查询持仓"<<endl;
		cerr<<" [5] ReqOrderInsert            -- 报单"<<endl;
		cerr<<" [6] ReqOrderAction            -- 撤单"<<endl;
		cerr<<" [7] PrintOrders               -- 显示报单"<<endl;
		cerr<<" [8] PrintTrades               -- 显示成交"<<endl;
		cerr<<" [0] Exit                      -- 退出"<<endl;
		cerr<<"----------------------------------------------"<<endl;
	}   
	TSecurityFtdcBrokerIDType	    brokerId;
	TSecurityFtdcUserIDType	        userId;
	TSecurityFtdcPasswordType	    passwd;
	TSecurityFtdcInstrumentIDType    instId;
	TSecurityFtdcDirectionType       dir;
	TSecurityFtdcCombOffsetFlagType  kpp;
	TSecurityFtdcPriceType           price;
	TSecurityFtdcVolumeType          vol;
	TSecurityFtdcExchangeIDType		exchangID;
	TSecurityFtdcSequenceNoType      orderSeq;

	int cmd;  cin>>cmd;
	switch(cmd){
	case 1: {
		cerr<<" 经纪公司代码（2011） > ";cin>>brokerId;
		cerr<<" 投资者代码 > ";cin>>userId;
		cerr<<" 交易密码 > ";cin>>passwd;
		p->ReqUserLogin(brokerId,userId,passwd); break;
			}

	case 2: {
		cerr<<" 合约代码 > "; cin>>instId; 
		p->ReqQryInstrument(instId); break;
			}
	case 3: p->ReqQryTradingAccount(); break;
	case 4: {
		cerr<<" 合约代码 > "; cin>>instId; 
		p->ReqQryInvestorPosition(instId); break;
			}
	case 5: {
		cerr<<" 合约代码 > "; cin>>instId; 
		cerr<<" 方向（买0卖1） > "; cin>>dir; 
		cerr<<" 开(0)平(1) > "; cin>>kpp;
		cerr<<" 价格 > "; cin>>price;
		cerr<<" 数量 > "; cin>>vol;      
		cerr<<" 交易所代码(sse/sze) > "; cin>>exchangID;              
		p->ReqOrderInsert(instId,dir,kpp,price,vol,exchangID); break;
			}
	case 6: { 
		cerr<<" 序号 > "; cin>>orderSeq;
		p->ReqOrderAction(orderSeq);break;
			}
	case 7: p->PrintOrders();break;
	case 8: p->PrintTrades();break;
	case 0: exit(0);
	}  
	WaitForSingleObject(g_hEvent,INFINITE);
	ResetEvent(g_hEvent);
	ShowTraderCommand(p);
}

void ShowMdCommand(mdspi* p, bool print=false){
	if(print){
		cerr<<"-----------------------------------------------"<<endl;
		cerr<<" [1] ReqUserLogin              -- 登录"<<endl;
		cerr<<" [2] SubscribeMarketData       -- 行情订阅"<<endl;
		cerr<<" [0] Exit                      -- 退出"<<endl;
		cerr<<"----------------------------------------------"<<endl;
	}
	TSecurityFtdcBrokerIDType appId;
	TSecurityFtdcUserIDType	 userId;
	TSecurityFtdcPasswordType passwd;
	char instIdList[100];
	char exchangID[12];

	int cmd;  cin>>cmd;
	switch(cmd){
	case 1: {
		cerr<<" 经纪公司代码（2011） > ";cin>>appId;
		cerr<<" 投资者代码 > ";cin>>userId;
		cerr<<" 交易密码 > ";cin>>passwd;
		p->ReqUserLogin(appId,userId,passwd); break;
			}
	case 2: {
		cerr<<" 合约（逗号分隔多个合约） > "; cin>>instIdList; 
		cerr<<" 交易所代码((sse/sze)） > "; cin>>exchangID; 
		p->SubscribeMarketData(instIdList,exchangID); break;
			}
	case 0: exit(0);
	}
	WaitForSingleObject(g_hEvent,INFINITE);
	ResetEvent(g_hEvent);
	ShowMdCommand(p);
}
#endif
