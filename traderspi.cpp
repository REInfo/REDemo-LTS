/*!
* \file traderspi.cpp
* \brief 实现交易接口
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
#include <iostream>
#include <vector>

using namespace std;
#include "api/trade/win/public/SecurityFtdcTraderApi.h"
#include "traderspi.h"
#include "windows.h"
#pragma warning(disable :4996)

TSecurityFtdcBrokerIDType appId;		// 应用单元
TSecurityFtdcUserIDType userId;		// 投资者代码


extern int requestId; 
extern HANDLE g_hEvent;

// 会话参数
int	 frontId;	//前置编号
int	 sessionId;	//会话编号
char orderRef[13];

vector<CSecurityFtdcOrderField*> orderList;
vector<CSecurityFtdcTradeField*> tradeList;

char MapDirection(char src, bool toOrig);
char MapOffset(char src, bool toOrig);



void traderspi::OnFrontConnected()
{
	cerr<<" 连接交易前置...成功"<<endl;
	SetEvent(g_hEvent);
}

void traderspi::ReqUserLogin(TSecurityFtdcBrokerIDType	vAppId,
	TSecurityFtdcUserIDType	vUserId,	TSecurityFtdcPasswordType	vPasswd)
{

	CSecurityFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, vAppId); strcpy(appId, vAppId); 
	strcpy(req.UserID, vUserId);  strcpy(userId, vUserId); 
	strcpy(req.Password, vPasswd);
	int ret = pUserApi->ReqUserLogin(&req, ++requestId);
	cerr<<" 请求 | 发送登录..."<<((ret == 0) ? "成功" :"失败") << endl;	
}

void traderspi::OnRspUserLogin(CSecurityFtdcRspUserLoginField *pRspUserLogin,
	CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if ( !IsErrorRspInfo(pRspInfo) && pRspUserLogin ) {  
		// 保存会话参数	
		frontId = pRspUserLogin->FrontID;
		sessionId = pRspUserLogin->SessionID;
		int nextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		sprintf(orderRef, "%d", ++nextOrderRef);
		cerr<<" 响应 | 登录成功...当前交易日:"
			<<pRspUserLogin->TradingDay<<endl;     
	}
	if(bIsLast) 
		SetEvent(g_hEvent);
}

void traderspi::ReqQryInstrument(TSecurityFtdcInstrumentIDType instId)
{
	CSecurityFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, instId);//为空表示查询所有合约
	int ret = pUserApi->ReqQryInstrument(&req, ++requestId);
	cerr<<" 请求 | 发送合约查询..."<<((ret == 0)?"成功":"失败")<<endl;
}

void traderspi::OnRspQryInstrument(CSecurityFtdcInstrumentField *pInstrument, 
	CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 	
	if ( !IsErrorRspInfo(pRspInfo) &&  pInstrument){
		cerr<<" 响应 | 合约:"<<pInstrument->InstrumentID
			<<" 交易所代码:"<<pInstrument->ExchangeID<<endl; 
	}
	if(bIsLast) SetEvent(g_hEvent);
}

void traderspi::ReqQryTradingAccount()
{
	CSecurityFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, appId);
	strcpy(req.InvestorID, userId);
	int ret = pUserApi->ReqQryTradingAccount(&req, ++requestId);
	cerr<<" 请求 | 发送资金查询..."<<((ret == 0)?"成功":"失败")<<endl;

}

void traderspi::OnRspQryTradingAccount(
	CSecurityFtdcTradingAccountField *pTradingAccount, 
	CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 
	if (!IsErrorRspInfo(pRspInfo) &&  pTradingAccount){
		cerr<<" 响应 | 权益:"<<pTradingAccount->Balance
			<<" 可用:"<<pTradingAccount->Available   
			<<" 保证金:"<<pTradingAccount->CurrMargin
			<<" 手续费:"<<pTradingAccount->Commission
			<<" 冻结保证金:"<<pTradingAccount->FrozenMargin
			//<<" 冻结手续费:"<<pTradingAccount->FrozenCommission 
			<< endl;    
	}
	if(bIsLast) SetEvent(g_hEvent);
}

void traderspi::ReqQryInvestorPosition(TSecurityFtdcInstrumentIDType instId)
{
	CSecurityFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, appId);
	strcpy(req.InvestorID, userId);
	strcpy(req.InstrumentID, instId);	
	int ret = pUserApi->ReqQryInvestorPosition(&req, ++requestId);
	cerr<<" 请求 | 发送持仓查询..."<<((ret == 0)?"成功":"失败")<<endl;
}

void traderspi::OnRspQryInvestorPosition(
	CSecurityFtdcInvestorPositionField *pInvestorPosition, 
	CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 
	if( !IsErrorRspInfo(pRspInfo) &&  pInvestorPosition ){
		cerr<<" 响应 | 合约:"<<pInvestorPosition->InstrumentID
			<<" 方向:"<<MapDirection(pInvestorPosition->PosiDirection-2,false)
			<<" 总持仓:"<<pInvestorPosition->Position
			<<" 昨仓:"<<pInvestorPosition->YdPosition 
			<<" 今仓:"<<pInvestorPosition->TodayPosition<<endl;
	}
	if(bIsLast) 
		SetEvent(g_hEvent);	
}

void traderspi::ReqOrderInsert(TSecurityFtdcInstrumentIDType instId,
	TSecurityFtdcDirectionType dir, TSecurityFtdcCombOffsetFlagType kpp,
	TSecurityFtdcPriceType price,   TSecurityFtdcVolumeType vol, 
	TSecurityFtdcExchangeIDType exchangID)
{
	CSecurityFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));	
	strcpy(req.BrokerID, appId);  //应用单元代码	
	strcpy(req.InvestorID, userId); //投资者代码	
	strcpy(req.ExchangeID, exchangID);//交易所代码
	strcpy(req.InstrumentID, instId); //合约代码	
	strcpy(req.OrderRef, orderRef);  //报单引用
	int nextOrderRef = atoi(orderRef);
	sprintf(orderRef, "%d", ++nextOrderRef);

	TSecurityFtdcStockPriceType stockPrice;
	sprintf(stockPrice, "%f", price);
	lstrcpyn(req.LimitPrice, stockPrice, sizeof(req.LimitPrice));	//价格

	req.OrderPriceType = SECURITY_FTDC_OPT_LimitPrice;//价格类型=限价	
	req.TimeCondition = SECURITY_FTDC_TC_GFD;  //有效期类型:当日有效
	
	req.Direction = MapDirection(dir,true);  //买卖方向	
	req.CombOffsetFlag[0] = MapOffset(kpp[0],true); //组合开平标志:开仓
	req.CombHedgeFlag[0] = SECURITY_FTDC_HF_Speculation;	  //组合投机套保标志	
	req.VolumeTotalOriginal = vol;	///数量		
	req.VolumeCondition = SECURITY_FTDC_VC_AV; //成交量类型:任何数量
	req.MinVolume = 1;	//最小成交量:1	
	req.ContingentCondition = SECURITY_FTDC_CC_Immediately;  //触发条件:立即

	//TThostFtdcPriceType	StopPrice;  //止损价
	req.ForceCloseReason =SECURITY_FTDC_FCC_NotForceClose;	//强平原因:非强平	
	req.IsAutoSuspend = 0;  //自动挂起标志:否	
	req.UserForceClose = 0;   //用户强评标志:否

	int ret = pUserApi->ReqOrderInsert(&req, ++requestId);
	cerr<<" 请求 | 发送报单..."<<((ret == 0)?"成功":"失败")<< endl;
}

void traderspi::OnRspOrderInsert(CSecurityFtdcInputOrderField *pInputOrder, 
	CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pInputOrder ){
		cerr<<"响应 | 报单提交成功...报单引用:"<<pInputOrder->OrderRef<<endl;  
	}
	if(bIsLast) SetEvent(g_hEvent);	
}

void traderspi::ReqOrderAction(TSecurityFtdcSequenceNoType orderSeq)
{
	CSecurityFtdcOrderField* pOrder;
	bool found=false; unsigned int i=0;
	for(i=0;i<orderList.size();i++){
		if(orderList[i]->BrokerOrderSeq == orderSeq)
		{ 
			pOrder = orderList[i];
			found = true; 
			break;
		}
	}
	if(!found){cerr<<" 请求 | 报单不存在."<<endl; return;} 

	CSecurityFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, appId);   //经纪公司代码	
	strcpy(req.InvestorID, userId); //投资者代码
	strcpy(req.OrderRef, pOrder->OrderRef); //报单引用	
	req.FrontID = pOrder->FrontID;           //前置编号	
	req.SessionID = pOrder->SessionID;       //会话编号
	strcpy(req.ExchangeID, orderList[i]->ExchangeID);
	strcpy(req.InstrumentID, orderList[i]->InstrumentID);
	req.ActionFlag = SECURITY_FTDC_AF_Delete;  //操作标志 

	int ret = pUserApi->ReqOrderAction(&req, ++requestId);
	cerr<< " 请求 | 发送撤单..." <<((ret == 0)?"成功":"失败") << endl;
}

void traderspi::OnRspOrderAction(
	CSecurityFtdcInputOrderActionField *pInputOrderAction, 
	CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
	if (!IsErrorRspInfo(pRspInfo) && pInputOrderAction){
		cerr<< " 响应 | 撤单成功..."
			<< "交易所:"<<pInputOrderAction->ExchangeID
			<<" 合约代码:"<<pInputOrderAction->InstrumentID<<endl;
	}
	if(bIsLast) SetEvent(g_hEvent);	
}

///报单回报
void traderspi::OnRtnOrder(CSecurityFtdcOrderField *pOrder)
{	
	CSecurityFtdcOrderField* order = new CSecurityFtdcOrderField();
	memcpy(order,  pOrder, sizeof(CSecurityFtdcOrderField));
	bool founded=false;    unsigned int i=0;
	for(i=0; i<orderList.size(); i++){
		if(orderList[i]->BrokerOrderSeq == order->BrokerOrderSeq) {
			founded=true;    break;
		}
	}
	if(founded) orderList[i]= order;   
	else  orderList.push_back(order);
	cerr<<" 回报 | 报单已提交...序号:"<<order->BrokerOrderSeq
		<<"OrderStatus:"<<order->OrderStatus
		<<"CombHedgeFlag:"<<order->CombHedgeFlag
		<<"CombOffsetFlag:"<<order->CombOffsetFlag
		<<"Direction:"<<order->Direction
		<<"InstrumentID:"<<order->InstrumentID
		<<"LimitPrice:"<<order->LimitPrice
		<<"MinVolume:"<<order->MinVolume
		<<"OrderPriceType:"<<order->OrderPriceType
		<<"StatusMsg:"<<order->StatusMsg
		<<endl;
	
	SetEvent(g_hEvent);	
}

///成交通知
void traderspi::OnRtnTrade(CSecurityFtdcTradeField *pTrade)
{
	CSecurityFtdcTradeField* trade = new CSecurityFtdcTradeField();
	memcpy(trade,  pTrade, sizeof(CSecurityFtdcTradeField));
	bool founded=false;     unsigned int i=0;
	for(i=0; i<tradeList.size(); i++){
		if(tradeList[i]->TradeID == trade->TradeID) {
			founded=true;   break;
		}
	}
	if(founded) tradeList[i] = trade;   
	else  tradeList.push_back(trade);
	cerr<<" 回报 | 报单已成交...成交编号:"<<trade->TradeID<<endl;
	SetEvent(g_hEvent);
}

void traderspi::OnFrontDisconnected(int nReason)
{
	cerr<<" 响应 | 连接中断..." 
		<< " reason=" << nReason << endl;
}

void traderspi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr<<" 响应 | 心跳超时警告..." 
		<< " TimerLapse = " << nTimeLapse << endl;
}

void traderspi::OnRspError(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}

bool traderspi::IsErrorRspInfo(CSecurityFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool ret = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (ret){
		cerr<<" 响应 | "<<pRspInfo->ErrorMsg<<endl;
	}
	return ret;
}

void traderspi::PrintOrders(){
	CSecurityFtdcOrderField* pOrder; 
	for(unsigned int i=0; i<orderList.size(); i++){
		pOrder = orderList[i];
		cerr<<" 报单 | 合约:"<<pOrder->InstrumentID
			<<" 方向:"<<MapDirection(pOrder->Direction,false)
			<<" 开平:"<<MapOffset(pOrder->CombOffsetFlag[0],false)
			<<" 价格:"<<pOrder->LimitPrice
			<<" 数量:"<<pOrder->VolumeTotalOriginal
			<<" 序号:"<<pOrder->BrokerOrderSeq 
			<<" 报单编号:"<<pOrder->OrderSysID
			<<" 状态:"<<pOrder->StatusMsg<<endl;
	}
	SetEvent(g_hEvent);
}
void traderspi::PrintTrades(){
	CSecurityFtdcTradeField* pTrade;
	for(unsigned int i=0; i<tradeList.size(); i++){
		pTrade = tradeList[i];
		cerr<<" 成交 | 合约:"<< pTrade->InstrumentID 
			<<" 方向:"<<MapDirection(pTrade->Direction,false)
			<<" 开平:"<<MapOffset(pTrade->OffsetFlag,false) 
			<<" 价格:"<<pTrade->Price
			<<" 数量:"<<pTrade->Volume
			<<" 报单编号:"<<pTrade->OrderSysID
			<<" 成交编号:"<<pTrade->TradeID<<endl;
	}
	SetEvent(g_hEvent);
}
char MapDirection(char src, bool toOrig=true){
	if(toOrig){
		if('b'==src||'B'==src){src='0';}else if('s'==src||'S'==src){src='1';}
	}else{
		if('0'==src){src='B';}else if('1'==src){src='S';}
	}
	return src;
}
char MapOffset(char src, bool toOrig=true){
	if(toOrig){
		if('o'==src||'O'==src){src='0';}
		else if('c'==src||'C'==src){src='1';}
		else if('j'==src||'J'==src){src='3';}
	}else{
		if('0'==src){src='O';}
		else if('1'==src){src='C';}
		else if('3'==src){src='J';}
	}
	return src;
}

